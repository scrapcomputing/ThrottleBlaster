@ECHO OFF
IF "%1"=="" GOTO HELP
kermit set port COM1, set speed 9600, output %1\13, input 3 \10
GOTO END
:HELP
ECHO Usage: THROTTLE command
ECHO   F133P8  Run at 133 MHz with PWM period 8. P is optional.
ECHO   F0      Run at the maximum frequency.
ECHO   S33     Select the 33 MHz preset. SMAX selects the maximum.
ECHO   W25     Save the current frequency and period to preset 25.
ECHO   D8      Delete preset 8, or restore it if deleted.
ECHO   M1200   Set the maximum CPU frequency to 1200 MHz.
ECHO   R       Return to the presets and the front panel.
ECHO   X       Reset to factory defaults.
ECHO   ?       Show the current state.
:END
