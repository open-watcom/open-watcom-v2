@echo off

rem *************************************************************
rem A few notes: when running some of these tests, wmake may
rem exit before the command line is fully processed. Hence the
rem -l switch may not work and we should use stdout/stderr
rem redirection to capture output. This is a sensible thing to
rem do anyway because that way we know processing of the -l
rem switch will not interfere with the tests in any way.
rem Also note that -l only logs errors (stderr), not normal
rem output (stdout). If a test needs to capture both, it has to
rem use redirection.
rem *************************************************************

set ERRORS=0

echo # =============================
echo # Error Tests
echo # =============================

if .%2 == . goto usage
set PRG=%1
set ERRLOG=%2

:test03
set TEST=03
head err%TEST% -3
del tmp.out
%PRG% "-." > tmp%TEST%a.lst 2>&1
egrep Error tmp%TEST%a.lst > test%TEST%a.lst
diff err%TEST%a.chk test%TEST%a.lst
call :result a

del tmp.out
%PRG% "- " > tmp%TEST%b.lst 2>&1
egrep Error tmp%TEST%b.lst > test%TEST%b.lst
diff -b err%TEST%b.chk test%TEST%b.lst
call :result b

set TEST=04
head err%TEST% -3
%PRG% -f > tmp%TEST%.lst 2>&1
egrep Error tmp%TEST%.lst > test%TEST%.lst
diff err%TEST%.chk test%TEST%.lst
call :result

set TEST=13
head err%TEST% -3
%PRG% -h "-" 2> test%TEST%.lst
%PRG% -h - 2>> test%TEST%.lst
%PRG% -h "-\" 2>> test%TEST%.lst
diff err%TEST%.chk test%TEST%.lst
call :result

set TEST=17
head err%TEST% -3
%PRG% -h -f err%TEST%a >  test%TEST%.lst 2>&1
%PRG% -h -f err%TEST%b >> test%TEST%.lst 2>&1
%PRG% -h -f err%TEST%c >> test%TEST%.lst 2>&1
%PRG% -h -f err%TEST%d >> test%TEST%.lst 2>&1
%PRG% -h -f err%TEST%e >> test%TEST%.lst 2>&1
%PRG% -h -f err%TEST%f >> test%TEST%.lst 2>&1
diff -i err%TEST%.chk test%TEST%.lst
call :result

set TEST=36
head err%TEST% -3
%PRG% -h -f err%TEST% .c.obj > test%TEST%.lst 2>&1
diff err%TEST%.chk test%TEST%.lst
call :result

set TEST=39
head err%TEST% -3
echo. >ditty.c
%PRG% -h -f err%TEST% ditty.obj > test%TEST%.lst 2>&1
diff err%TEST%.chk test%TEST%.lst
call :result

del ditty.*
set TEST=40
head err%TEST% -3
echo. >err%TEST%.tst
chmod +r err%TEST%.tst >test%TEST%.lst
%PRG% -h -a -t -f err%TEST% >> test%TEST%.lst 2>&1
chmod -r err%TEST%.tst
del err%TEST%.tst
diff err%TEST%.chk test%TEST%.lst
call :result

for %%i in (05 07 10 11 12 15 16 18 19 20 21 22 23 24 25 26 28 29a 29b 29c 30 31a 31b 31c 31d 31e 31f 32 33 34a 34b 37a 37b 38 41 42 43 44a 44b 44c) DO @call work %%i
for %%j in (27 35) DO @call debug %%j

rem if exist *.obj del *.obj
if %ERRORS% == 0 del *.lst
goto end

:usage
    echo usage: %0 prgname errorfile
goto end

:result
    if errorlevel 1 goto resulterr
    @echo #        Test %1 successful
goto end
:resulterr
    @echo ## ERROR %TEST% ## >> %ERRLOG%
    @echo # Error: Test %1 unsuccessful!!! | tee -a %ERRLOG%
    set ERRORS=1
goto end

:work
    set TEST=%1
    head err%TEST% -3
    %PRG% -f err%TEST% -h > test%TEST%.lst 2>&1
    diff -i err%TEST%.chk test%TEST%.lst
    call :result
goto end

:debug
    set TEST=%1
    head err%TEST% -3
    %PRG% -f err%TEST% -h -d > tmp%TEST%.lst 2>&1
    egrep W%TEST% tmp%TEST%.lst > test%TEST%.lst
    diff -i err%TEST%.chk test%TEST%.lst
    call :result
:end
