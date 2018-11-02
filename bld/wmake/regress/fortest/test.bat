@echo %verbose% off

set ERRORS=0

echo # ===========================
echo # Start FORTEST
echo # ===========================

if .%1 == . goto usage
set PRG=%1
set ERRLOG=%2

set TEST=01
call :header
if exist for01.out del for01.out
%PRG% -h -f for01 > for01.out 2>&1
diff for01.out for01.chk
call :result

set TEST=02
call :header
if exist for02.out del for02.out
%PRG% -h -f for02 > for02.out 2>&1
diff for02.out for02.chk
call :result

set TEST=03
call :header
if exist for03.out del for03.out
%PRG% -h -f for03 > for03.out 2>&1
diff for03.out for03.chk
call :result

set TEST=04
call :header
if exist tmp04.chk del tmp04.chk
echo. > for04.out
type for04a.chk   > tmp04.chk
dir /b /-o       >> tmp04.chk
type for04b.chk  >> tmp04.chk
dir /b /-o for?? >> tmp04.chk
type for04c.chk  >> tmp04.chk
%PRG% -h -f for04 > for04.out 2>&1
diff for04.out tmp04.chk
call :result

set TEST=05
call :header
if exist for05.out del for05.out
%PRG% -h -f for05 > for05.out 2>&1
diff for05.out for05.chk
call :result

set TEST=06
call :header
if exist for06.out del for06.out
%PRG% -h -f for06 > for06.out 2>&1
diff for06.out for06.chk
call :result

set TEST=07
call :header
if exist for07.out del for07.out
%PRG% -h -f for07 > for07.out 2>&1
diff for07.out for07.chk
call :result

set TEST=08
call :header
if exist for08.out del for08.out
echo. > for08.out
rem Need to set prompt, otherwise the test fails...
prompt $p$g
type for08.chk > tmp08.chk
..\cmds\prntdir "echo a" >> tmp08.chk
echo a >> tmp08.chk
..\cmds\prntdir "echo b" >> tmp08.chk
echo b >> tmp08.chk
..\cmds\prntdir "echo c" >> tmp08.chk
echo c >> tmp08.chk
%PRG% -h -f for08 > for08.out 2>&1
diff -b for08.out tmp08.chk
call :result

if exist *.obj del *.obj
if %ERRORS% == 0 del *.lst
goto end

:usage
    echo usage: %0 prgname errorfile
    goto end

:header
    echo # ---------------------------
    echo # For Loop Test %TEST%
    echo # ---------------------------
    goto end

:result
if errorlevel 1 goto resulterr
    echo #        Test successful
    goto end
:resulterr
    echo ## FORTEST ## >> %ERRLOG%
    echo # Error: Test unsuccessful!!! | tee -a %ERRLOG%
    set ERRORS=1
:end
