@echo off

set ERRORS=0

echo # ===========================
echo # Long FileName Tests
echo # ===========================

if .%2 == . goto usage
set PRG=%1
set ERRLOG=%2

set TEST=01
call :header
echo LONGFILENAME OK > "HELLO TMP.TMP"
echo. >hello.h
%1 -h -a -f long%TEST% > test%TEST%.lst 2>&1
diff -b long%TEST%.chk test%TEST%.lst
call :result

del "HELLO TMP.TMP"
del hello.h

set TEST=02
call :header
%1 -h -ms -a -f long%TEST% > test%TEST%.lst 2>&1
diff -b long%TEST%.chk test%TEST%.lst
call :result

set TEST=03
call :header
rem This one MUST NOT use -a switch!
%1 -h -ms -f long%TEST% > test%TEST%.lst 2>&1
diff -b long%TEST%.chk test%TEST%.lst
call :result

set TEST=04
call :header
%1 -h -m -f long%TEST% > test%TEST%.lst 2>&1
diff -b long%TEST%.chk test%TEST%.lst
call :result

set TEST=05
call :header
%1 -h -m -f long%TEST% > test%TEST%.lst 2>&1
diff -b long%TEST%.chk test%TEST%.lst
call :result

set TEST=06
call :header
%1 -h -m -f long%TEST% > test%TEST%.lst 2>&1
diff -b long%TEST%.chk test%TEST%.lst
call :result

set TEST=07
call :header
%1 -h -m -f long%TEST% > test%TEST%.lst 2>&1
diff -b long%TEST%.chk test%TEST%.lst
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
    @echo #        Test successful
goto end
:resulterr
    @echo ## FORTEST %TEST% ## >> %ERRLOG%
    @echo # Error: Test unsuccessful!!! | tee -a %ERRLOG%
    set ERRORS=1
:end
