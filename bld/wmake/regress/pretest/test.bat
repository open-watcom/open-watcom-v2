@echo %verbose% off

set ERRORS=0

ECHO # ===================================
ECHO # Preprocessor IF Tests
ECHO # ===================================

if .%2 == . goto usage
set PRG=%1
set ERRLOG=%2

set TEST=01
call :header
:: 2>nul to hide debug-build, memory-tracking diagnostic
%PRG% -h -f pre%TEST%a pre%TEST%a -f pre%TEST%b pre%TEST%b 2>nul
call :result

set TEST=02
call :header
%PRG% -h -f pre%TEST% pre%TEST% > test%TEST%.lst 2>&1
diff -i pre%TEST%.chk test%TEST%.lst
call :result

set TEST=03
call :header
%PRG% -h -f pre%TEST% pre%TEST%
call :result

set TEST=04
call :header
%PRG% -h -f pre%TEST% pre%TEST%
call :result

set TEST=05
call :header
%PRG% -h -f pre%TEST% pre%TEST%
call :result

set TEST=06
call :header
%PRG% -h -f pre%TEST% > test%TEST%.lst 2>&1
diff -i pre%TEST%.chk test%TEST%.lst
call :result

set TEST=07
call :header
%PRG% -h -f pre%TEST% > test%TEST%.lst 2>&1
call :result a
diff pre%TEST%.chk test%TEST%.lst
call :result b

if %ERRORS% == 0 del *.lst
goto end

:usage
    echo usage: %0 prgname errorfile
    goto end

:header
    echo # ---------------------------
    echo #  Preprocessor IF Test %TEST%
    echo # ---------------------------
    goto end

:result
if errorlevel 1 goto resulterr
    echo #        Test %1 successful
    goto end
:resulterr
    echo ## PRETEST %TEST% ## >> %ERRLOG%
    echo # Error: Test %1 unsuccessful!!! | tee -a %ERRLOG%
    set ERRORS=1
:end
