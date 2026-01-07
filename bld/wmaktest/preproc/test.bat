@echo %verbose% off

set ERRORS=0

echo # ===========================
echo # Preprocessor Tests
echo # ===========================

if .%1 == . goto usage
set WMK=%1
set ERRLOG=..\error.out

set TEST=01
call :header
%WMK% -h -f prep%TEST% > test%TEST%.lst 2>&1
diff -b -i prep%TEST%.chk test%TEST%.lst
call :result

set TEST=02
call :header
%WMK% -h -f prep%TEST% -m -ms > test%TEST%a.lst 2>&1
diff prep%TEST%.chk test%TEST%a.lst
call :result a
%WMK% -h -f prep%TEST% -m > test%TEST%b.lst 2>&1
diff prep%TEST%.chk test%TEST%b.lst
call :result b

set TEST=03
call :header
%WMK% -h -f prep%TEST% > test%TEST%.lst 2>&1
diff -b -i prep%TEST%.chk test%TEST%.lst
call :result

set TEST=04
call :header
%WMK% -h -f prep%TEST% > test%TEST%.lst 2>&1
diff -b prep%TEST%.chk test%TEST%.lst
call :result

if %ERRORS% == 0 del *.lst
goto end

:usage
    echo usage: %0 prgname errorfile
    goto end

:header
    echo # ---------------------------
    echo #  Preprocessor Test %TEST%
    echo # ---------------------------
    goto end

:result
if errorlevel 1 goto resulterr
    echo #        Test %1 successful
    goto end
:resulterr
    echo ## PREPROCESS %TEST% ## >> %ERRLOG%
    echo # Error: Test %1 unsuccessful!!! | tee -a %ERRLOG%
    set ERRORS=1
:end
