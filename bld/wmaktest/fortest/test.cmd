echo off

set ERRORS=0

echo # ===========================
echo # For Loop Tests
echo # ===========================

if .%1 == . goto usage
set PRG=%1
set ERRLOG=%2

set TEST=01
call :header
%PRG% -h -f for%TEST% > test%TEST%.lst 2>&1
diff for%TEST%.chk test%TEST%.lst
call :result

set TEST=02
call :header
%PRG% -h -f for%TEST% > test%TEST%.lst 2>&1
diff for%TEST%.chk test%TEST%.lst
call :result

set TEST=03
call :header
%PRG% -h -f for%TEST% > test%TEST%.lst 2>&1
diff for%TEST%.chk test%TEST%.lst
call :result

set TEST=04
call :header
type for%TEST%a.chk > tmp%TEST%.lst
dir /b >> tmp%TEST%.lst
type for%TEST%b.chk >> tmp%TEST%.lst
dir for?? /b >> tmp%TEST%.lst
type for%TEST%c.chk >> tmp%TEST%.lst
%PRG% -h -f for%TEST% > test%TEST%.lst 2>&1
diff tmp%TEST%.lst test%TEST%.lst
call :result

set TEST=05
call :header
%PRG% -h -f for%TEST% > test%TEST%.lst 2>&1
diff for%TEST%.chk test%TEST%.lst
call :result

set TEST=06
call :header
%PRG% -h -f for%TEST% > test%TEST%.lst 2>&1
diff for%TEST%.chk test%TEST%.lst
call :result

set TEST=07
call :header
%PRG% -h -f for%TEST% > test%TEST%.lst 2>&1
diff for%TEST%.chk test%TEST%.lst
call :result

set TEST=08
call :header
rem Need to set prompt, otherwise the test fails...
prompt $p$g
type for%TEST%.chk > tmp%TEST%.lst
..\cmds\prntdir "echo a" >> tmp%TEST%.lst
echo a >> tmpfile.tmp
..\cmds\prntdir "echo b" >> tmp%TEST%.lst
echo b >> tmpfile.tmp
..\cmds\prntdir "echo c" >> tmp%TEST%.lst
echo c >> tmp%TEST%.lst
%PRG% -h -f for%TEST% > test%TEST%.lst 2>&1
diff -b tmp%TEST%.lst test%TEST%.lst
call :result

rem if exist *.obj del *.obj
if %ERRORS% == 0 del *.lst
goto end

:usage
    echo usage: %0 prgname errorfile
goto end

:header
    echo # ---------------------------
    echo #  For Loop Test %TEST%
    echo # ---------------------------
    goto end

:result
    if errorlevel 1 goto resulterr
    @echo #        Test successful
goto end
:resulterr
    @echo ## FORTEST %TEST% ## >> %ERRLOG%
    @echo # Error: Test unsuccessful!!! | tee -a %ERRLOG%
    set ERRORS=1
:end
