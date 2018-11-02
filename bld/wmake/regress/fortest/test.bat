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
%PRG% -h -f for$TEST > for$TEST.lst 2>&1
diff for$TEST.chk for$TEST.lst
call :result

set TEST=02
call :header
%PRG% -h -f for$TEST > for$TEST.lst 2>&1
diff for$TEST.chk for$TEST.lst
call :result

set TEST=03
call :header
%PRG% -h -f for$TEST > for$TEST.lst 2>&1
diff for$TEST.chk for$TEST.lst
call :result

set TEST=04
call :header
echo. > for$TEST.lst
type for${TEST}a.chk > tmp$TEST.chk
dir /b /-o       >> tmp$TEST.chk
type for${TEST}b.chk >> tmp$TEST.chk
dir /b /-o for?? >> tmp$TEST.chk
type for${TEST}c.chk >> tmp$TEST.chk
%PRG% -h -f for$TEST > for$TEST.lst 2>&1
diff tmp$TEST.chk for$TEST.lst
call :result

set TEST=05
call :header
%PRG% -h -f for$TEST > for$TEST.lst 2>&1
diff for$TEST.chk for$TEST.lst
call :result

set TEST=06
call :header
%PRG% -h -f for$TEST > for$TEST.lst 2>&1
diff for$TEST.chk for$TEST.lst
call :result

set TEST=07
call :header
%PRG% -h -f for$TEST > for$TEST.lst 2>&1
diff for$TEST.chk for$TEST.lst
call :result

set TEST=08
call :header
echo. > for$TEST.lst
rem Need to set prompt, otherwise the test fails...
prompt $p$g
type for$TEST.chk > tmp$TEST.chk
..\cmds\prntdir "echo a" >> tmp$TEST.chk
echo a >> tmp$TEST.chk
..\cmds\prntdir "echo b" >> tmp$TEST.chk
echo b >> tmp$TEST.chk
..\cmds\prntdir "echo c" >> tmp$TEST.chk
echo c >> tmp$TEST.chk
%PRG% -h -f for$TEST > for$TEST.lst 2>&1
diff -b tmp$TEST.chk for$TEST.lst
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
