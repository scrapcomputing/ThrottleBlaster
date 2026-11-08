@ECHO OFF
REM SPEED.BAT - Set the Throttle Blaster speed over the serial port.
REM Needs KERMIT.EXE (MS-Kermit) in the PATH. Example: SPEED 486
REM
REM ======== Your settings: edit this section ========
SET TBPORT=COM1
SET TB=
REM  F<MHz>P<Period> : any speed up to the maximum CPU frequency.
REM  S<MHz>          : a saved preset, with its tuned frequency and period.
IF "%1"=="8088" SET TB=F4.77P8
IF "%1"=="8086" SET TB=F8P8
IF "%1"=="286" SET TB=F12P8
IF "%1"=="386" SET TB=F33P8
IF "%1"=="486" SET TB=F66P8
IF "%1"=="586" SET TB=F133P8
IF "%1"=="686" SET TB=F233P8
IF "%1"=="PII266" SET TB=F266P8
IF "%1"=="PII300" SET TB=F300P8
IF "%1"=="PII350" SET TB=F350P8
IF "%1"=="PII400" SET TB=F400P8
IF "%1"=="PII450" SET TB=F450P8
IF "%1"=="PIII500" SET TB=F500P8
IF "%1"=="PIII600" SET TB=F600P8
IF "%1"=="MAX" SET TB=SMAX
IF "%1"=="max" SET TB=SMAX
REM ======== End of settings ========

IF "%TB%"=="" GOTO HELP
REM Waits up to 3 seconds for the OK reply.
kermit set port %TBPORT%, set speed 9600, output %TB%\13, input 3 OK
GOTO END

:HELP
ECHO Usage: SPEED name
ECHO Names: 8088 8086 286 386 486 586 686 MAX
ECHO        PII266 PII300 PII350 PII400 PII450
ECHO        PIII500 PIII600 (use capitals)

:END
SET TB=
SET TBPORT=
