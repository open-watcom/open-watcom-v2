@echo %verbose% off

set ERRORS=0

echo # ===========================
echo # Long FileName Tests
echo # ===========================

if .%1 == . goto usage
set WMK=%1
set ERRLOG=..\error.out

set TEST=01
call :header
echo LONGFILENAME OK > "HELLO TMP.TMP"
echo. >hello.h
%WMK% -h -a -f long%TEST% > test%TEST%.lst 2>&1
diff -b long%TEST%.chk test%TEST%.lst
call :result

set TEST=02
del "HELLO TMP.TMP"
del hello.h
call :header
%WMK% -h -ms -m -a -f long%TEST% > test%TEST%.lst 2>&1
diff -b long%TEST%.chk test%TEST%.lst
call :result

set TEST=03
call :header
rem This one MUST NOT use -a switch!
%WMK% -h -ms -m -f long%TEST% > test%TEST%.lst 2>&1
diff -b long%TEST%.chk test%TEST%.lst
call :result

set TEST=04
call :header
%WMK% -h -m -f long%TEST% > test%TEST%.lst 2>&1
diff -b long%TEST%.chk test%TEST%.lst
call :result

set TEST=05
call :header
%WMK% -h -m -f long%TEST% > test%TEST%.lst 2>&1
diff -b long%TEST%.chk test%TEST%.lst
call :result

set TEST=06
call :header
%WMK% -h -m -f long%TEST% > test%TEST%.lst 2>&1
diff -b long%TEST%.chk test%TEST%.lst
call :result

set TEST=07
call :header
%WMK% -h -m -f long%TEST% > test%TEST%.lst 2>&1
diff -b long%TEST%.chk test%TEST%.lst
call :result

if exist *.obj del *.obj
if %ERRORS% == 0 del *.lst
goto end

:usage
    echo usage: %0 prgname errorfile
goto end

:header
    echo # ---------------------------
    echo #  Long FileName Test %TEST%
    echo # ---------------------------
goto end

:result
if errorlevel 1 goto resulterr
    @echo #        Test successful
    goto end
:resulterr
    @echo ## Long FileName %TEST% ## >> %ERRLOG%
    @echo # Error: Test unsuccessful!!! | tee -a %ERRLOG%
    set ERRORS=1
:end
