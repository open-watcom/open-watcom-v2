@echo off

set ERRORS=0

echo # ===========================
echo # Update Tests
echo # ===========================

if .%1 == . goto usage
set WMK=%1
set ERRLOG=..\error.out

set TEST=01
call :header
%WMK% -h -f upd%TEST%
call :result a
%WMK% -h -f upd%TEST% -m -sn -y > test%TEST%.lst
call :result b
diff upd%TEST%.chk test%TEST%.lst
call :result c

set TEST=02
call :header
%WMK% -h -f upd%TEST% /c
call :result

set TEST=03
call :header
%WMK% -h -f upd%TEST%
call :result

set TEST=04
call :header
%WMK% -h -f upd%TEST% > test%TEST%.lst 2>&1
diff -b upd%TEST%.chk test%TEST%.lst
call :result

set TEST=05
call :header
%WMK% -h -s -f upd%TEST% > test%TEST%.lst
diff -b upd%TEST%.chk test%TEST%.lst
call :result

set TEST=06
call :header
%WMK% -h -f upd%TEST% > test%TEST%.lst
diff upd%TEST%.chk test%TEST%.lst
call :result

set TEST=07
call :header
%WMK% -h -f upd%TEST% > test%TEST%.lst
diff upd%TEST%.chk test%TEST%.lst
call :result

REM set TEST=08
REM call :header
REM %WMK% -h -f upd%TEST% >test%TEST%.lst
REM call :result a
REM diff upd%TEST%.chk test%TEST%.lst
REM call :result b

:test9

set TEST=09
call :header
%WMK% -h -f upd%TEST%
call :result

set TEST=10
call :header
%WMK% -h -f upd%TEST%
call :result

set TEST=11
call :header
%WMK% -h -ms -f upd%TEST% > test%TEST%.lst 2>&1
diff -b upd%TEST%.chk test%TEST%.lst
call :result

if %ERRORS% == 0 del *.lst
goto end

:usage
    echo usage: %0 prgname errorfile
    goto end

:header
    echo # ---------------------------
    echo #  Update Test %TEST%
    echo # ---------------------------
    goto end

:result
if errorlevel 1 goto resulterr
    echo #        Test %1 successful
    goto end
:resulterr
    echo ## UPDTEST %TEST% ## >> %ERRLOG%
    echo # Error: Test %1 unsuccessful!!! | tee -a %ERRLOG%
    set ERRORS=1
:end
