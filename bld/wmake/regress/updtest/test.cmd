@echo off

set ERRORS=0

echo # ===========================
echo # Update Tests
echo # ===========================

if .%2 == . goto usage
set PRG=%1
set ERRLOG=%2

set TEST=01
call :header
%PRG% -h -f upd%TEST%
call :result a
%PRG% -h -f upd%TEST% -m -sn -y > test%TEST%.lst
call :result b
diff upd%TEST%.chk test%TEST%.lst
call :result c

set TEST=02
call :header
%PRG% -h -f upd%TEST% /c
call :result

set TEST=03
call :header
%PRG% -h -f upd%TEST%
call :result

set TEST=04
call :header
%1 -h -f upd%TEST% > test%TEST%.lst 2>&1
diff -b upd%TEST%.chk test%TEST%.lst
call :result

set TEST=05
call :header
%1 -h -s -f upd%TEST% > test%TEST%.lst 2>&1
diff -b upd%TEST%.chk test%TEST%.lst
call :result

set TEST=06
call :header
%1 -h -f upd%TEST% > test%TEST%.lst 2>&1
diff upd%TEST%.chk test%TEST%.lst
call :result

set TEST=07
call :header
%1 -h -f upd%TEST% > test%TEST%.lst 2>&1
diff upd%TEST%.chk test%TEST%.lst
call :result

REM set TEST=08
REM call :header
REM %1 -h -f upd%TEST% >test%TEST%.lst
REM call :result a
REM diff upd%TEST%.chk test%TEST%.lst
REM call :result b

set TEST=09
call :header
%1 -h -f upd%TEST%
call :result

set TEST=10
call :header
%1 -h -f upd%TEST%
call :result

set TEST=11
call :header
%1 -h -ms -f upd%TEST% > test%TEST%.lst 2>&1
diff -b upd%TEST%.chk test%TEST%.lst
call :result

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
    @echo #        Test %1 successful
goto end
:resulterr
    @echo ## FORTEST %TEST% ## >> %ERRLOG%
    @echo # Error: Test %1 unsuccessful!!! | tee -a %ERRLOG%
    set ERRORS=1
:end
