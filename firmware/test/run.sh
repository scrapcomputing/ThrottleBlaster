#!/bin/bash
# Host tests: real ThrottleBlaster logic, Pico hardware replaced by stubs.
# Usage: run.sh [path/to/firmware/src]
HERE=$(cd "$(dirname "$0")" && pwd)
SRC=$(cd "${1:-$HERE/../src}" && pwd)
W=$(mktemp -d); trap "rm -rf $W" EXIT
FLAGS="-std=c++17 -g -I. -w"
REAL="CommonLogic.cpp CommonLogic.h Presets.cpp Presets.h DutyCycle.h ConfigOpts.h Debug.h Button.cpp RotaryLogic.cpp RotaryLogic.h PotentiometerLogic.cpp PotentiometerLogic.h TwoButtonLogic.cpp TwoButtonLogic.h"
LOGIC="CommonLogic.cpp Presets.cpp Button.cpp RotaryLogic.cpp PotentiometerLogic.cpp TwoButtonLogic.cpp"
prep() { rm -rf "$W"; mkdir -p "$W"; for F in $REAL; do cp "$SRC/$F" "$W/"; done; cp -r "$HERE"/stubs/* "$HERE"/test_*.cpp "$W/"; }
RC=0
prep; cd "$W"
g++ $FLAGS -O1 -fsanitize=address,undefined -fno-sanitize-recover=undefined -c $LOGIC || exit 2
for N in serial frontpanel fuzz; do
  g++ $FLAGS -O1 -fsanitize=address,undefined -fno-sanitize-recover=undefined test_$N.cpp *.o -o t_$N || exit 2
  echo "--- $N"; ./t_$N; E=$?; echo "exit=$E"; [ $E -eq 0 ] || RC=1
done
echo "--- mutations (each must make a test fail)"
mutate() { # name file sed-expr
  prep; cd "$W"; sed -i "$3" "$2"
  if cmp -s "$2" "$SRC/$2"; then echo "$1: NOT APPLIED"; RC=1; return; fi
  g++ $FLAGS $LOGIC test_serial.cpp -o t1 2>/dev/null && g++ $FLAGS $LOGIC test_frontpanel.cpp -o t2 2>/dev/null || { echo "$1: did not compile"; RC=1; return; }
  if ./t1 >/dev/null && ./t2 >/dev/null; then echo "$1: SURVIVED"; RC=1; else echo "$1: caught"; fi
}
mutate no-flash-clear CommonLogic.cpp '0,/^    Disp.setFlash(false);$/{/^    Disp.setFlash(false);$/d}'
mutate deleted-selectable Presets.h 's/ \&\& (IncludeDeleted || !Table\[I\].Deleted)//'
mutate no-pot-save PotentiometerLogic.cpp 's/NewMode == Mode::Uart || NewMode == Mode::Presets/NewMode == Mode::Uart/'
mutate no-boot-guard CommonLogic.cpp 's/if (getMode() == Mode::Boot)/if (false)/'
mutate no-next-after-delete CommonLogic.cpp 's/^      Presets.next();$/      ;/'
mutate w-not-saved CommonLogic.cpp '/setTuning/{n;d}'
mutate w-allows-max CommonLogic.cpp 's/if (Idx < 0 || KHz == Presets.getMaxKHz())/if (Idx < 0)/'
for F in RotaryLogic PotentiometerLogic TwoButtonLogic; do
  mutate panel-reset-stale-$F $F.cpp '/Presets.resetToDefaults(Flash);/{n;d}'
done
exit $RC
