@ECHO OFF
REM Set the Throttle Blaster speed. Needs KERMIT.EXE in the PATH.
REM SPEED 133     133 MHz, keeps the current PWM period
REM SPEED 133 16  133 MHz with PWM period 16
REM SPEED 0       maximum frequency
IF "%1"=="" GOTO HELP
SET TB=F%1
IF NOT "%2"=="" SET TB=F%1P%2
kermit set port COM1, set speed 9600, output %TB%\13, input 3 OK, input 1 \10
SET TB=
GOTO END

:HELP
ECHO Usage: SPEED MHz or SPEED MHz Period

:END
