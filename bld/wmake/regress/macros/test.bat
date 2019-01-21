@echo %verbose% off

set ERRORS=0

echo # ===========================
echo # Macro Tests
echo # ===========================

if .%2 == . goto usage
set PRG=%1
set ERRLOG=%2

set TEST=01
call :header
%PRG% -ms -m -h -f macro%TEST% > test%TEST%.lst 2>&1
diff -b macro%TEST%.chk test%TEST%.lst
call :result

set TEST=02
call :header
%PRG% -h -f macro%TEST% > test%TEST%a.lst 2>&1
diff -b macro%TEST%a.chk test%TEST%a.lst
call :result a

set TEST=02
call :header
%PRG% -h -ms -m -f macro%TEST% > test%TEST%b.lst 2>&1
diff -b macro%TEST%b.chk test%TEST%b.lst
call :result b

set TEST=03
:: set TRMEM_CODE=3
call :header
%PRG% -h -f macro%TEST% > test%TEST%a.lst 2>&1
diff -b macro%TEST%a.chk test%TEST%a.lst
call :result a

set TEST=03
:: set TRMEM_CODE=1
call :header
set fubar=test depends on this environment variable
%PRG% -h -ms -m -f macro%TEST% > test%TEST%b.lst 2>&1
set fubar=
diff -b -i macro%TEST%b.chk test%TEST%b.lst
call :result b

set TEST=04
call :header
%PRG% -h -f macro%TEST% > test%TEST%a.lst 2>&1
diff -b macro%TEST%a.chk test%TEST%a.lst
call :result a

set TEST=04
call :header
%PRG% -h -ms -m -f macro%TEST% > test%TEST%b.lst 2>&1
diff -b macro%TEST%b.chk test%TEST%b.lst
call :result b

set TEST=05
call :header
echo. >hello.boo
echo. >hello.tmp
echo. >hello.c
%PRG% -h -ms -m -f macro%TEST% > test%TEST%.lst 2>&1
diff -b macro%TEST%.chk test%TEST%.lst
call :result

del hello.boo hello.tmp hello.c

set TEST=06
call :header
echo. >hello.obj
echo. >hello2.obj
echo. >hello.c
echo. >hello2.c
%PRG% -h -ms -m -f macro%TEST% -a > test%TEST%.lst 2>&1
diff -b macro%TEST%.chk test%TEST%.lst
call :result

set TEST=07
call :header
echo. >hello.obj
echo. >hello2.obj
echo. >hello.c
echo. >hello2.c
%PRG% -h -ms -m -f macro%TEST% -a cc=wcl386 > test%TEST%.lst 2>&1
diff -b macro%TEST%.chk test%TEST%.lst
call :result

set TEST=08
call :header
%PRG% -h -f macro%TEST% > test%TEST%.lst 2>&1
diff -b macro%TEST%.chk test%TEST%.lst
call :result

del hello.obj hello2.obj hello.boo hello.tmp hello.c hello2.c 

if %ERRORS% == 0 del *.lst
goto end

:usage
    echo usage: %0 prgname errorfile
    goto end

:header
    echo # ---------------------------
    echo #  Macro Test %TEST%
    echo # ---------------------------
    goto end

:result
if errorlevel 1 goto resulterr
    echo #        Test %1 successful
    goto end
:resulterr
    echo ## MACRO %TEST% ## >> %ERRLOG%
    echo # Error: Test %1 unsuccessful!!! | tee -a %ERRLOG%
    set ERRORS=1
:end
