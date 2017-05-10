@echo %verbose% off
echo # ===========================
echo # Start Macro Test
echo # ===========================

if .%2 == . goto usage

echo # ---------------------------
echo #   Macro Test 1
echo # ---------------------------

%1 -ms -m -h -f macro01 > macro01.out 2>&1
diff -b macro01.chk macro01.out
if errorlevel 1 goto err1
    @echo # MACRO01 successful
    del macro01.out
    goto test2
:err1
    @echo ## MACRO ## >> %2
    @echo Error: MACRO #1 unsuccessful!!! | tee -a %2

:test2

echo # ---------------------------
echo #   Macro Test 2A
echo # ---------------------------

%1 -h -f macro02 > macro02a.out 2>&1
diff -b macro02a.chk macro02a.out
if errorlevel 1 goto err2a
    @echo # MACRO02A successful
    del macro02a.out
    goto test2b
:err2a
    @echo ## MACRO ## >> %2
    @echo Error: MACRO #2A unsuccessful!!! | tee -a %2

:test2b
echo # ---------------------------
echo #   Macro Test 2B
echo # ---------------------------

%1 -h -ms -m -f macro02 > macro02b.out 2>&1
diff -b macro02b.chk macro02b.out
if errorlevel 1 goto err2b
    @echo # MACRO02B successful
    del macro02b.out
    goto test3
:err2b
    @echo ## MACRO ## >> %2
    @echo Error: MACRO #2B unsuccessful!!! | tee -a %2

:test3

echo # ---------------------------
echo #   Macro Test 3A
echo # ---------------------------
:: set TRMEM_CODE=3
%1 -h -f macro03 > macro03a.out 2>&1
diff -b macro03a.chk macro03a.out
if errorlevel 1 goto err3a
    @echo # MACRO03A successful
    del macro03a.out
    goto test3b
:err3a
    @echo ## MACRO ## >> %2
    @echo Error: MACRO #3A unsuccessful!!! | tee -a %2

:test3b
:: set TRMEM_CODE=1
echo # ---------------------------
echo #   Macro Test 3B
echo # ---------------------------

set fubar=test depends on this environment variable
%1 -h -ms -m -f macro03 > macro03b.out 2>&1
set fubar=
diff -b -i macro03b.chk macro03b.out
if errorlevel 1 goto err3b
    @echo # MACRO03B successful
    del macro03b.out
    goto test4
:err3b
    @echo ## MACRO ## >> %2
    @echo Error: MACRO #3B unsuccessful!!! | tee -a %2

:test4

echo # ---------------------------
echo #   Macro Test 4A
echo # ---------------------------

%1 -h -f macro04 > macro04a.out 2>&1
diff -b macro04a.chk macro04a.out
if errorlevel 1 goto err4a
    @echo # MACRO04A successful
    del macro04a.out
    goto test4b
:err4a
    @echo ## MACRO ## >> %2
    @echo Error: MACRO #4A unsuccessful!!! | tee -a %2

:test4b
echo # ---------------------------
echo #   Macro Test 4B
echo # ---------------------------

%1 -h -ms -m -f macro04 > macro04b.out 2>&1
diff -b macro04b.chk macro04b.out
if errorlevel 1 goto err4b
    @echo # MACRO0r4B successful
    del macro04b.out
    goto test5
:err4b
    @echo ## MACRO ## >> %2
    @echo Error: MACRO #4B unsuccessful!!! | tee -a %2

:test5

echo # ---------------------------
echo #   Macro Test 5
echo # ---------------------------

echo. >hello.boo
echo. >hello.tmp
echo. >hello.c
%1 -h -ms -m -f macro05 > macro05.out 2>&1
diff -b macro05.chk macro05.out
if errorlevel 1 goto err5
    @echo # MACRO05 successful
    del macro05.out
    goto test6
:err5
    @echo ## MACRO ## >> %2
    @echo Error: MACRO #5 unsuccessful!!! | tee -a %2

:test6
del hello.boo hello.tmp hello.c

echo # ---------------------------
echo #   Macro Test 6
echo # ---------------------------

echo. >hello.obj
echo. >hello2.obj
echo. >hello.c
echo. >hello2.c
%1 -h -ms -m -f macro06 -a > macro06.out 2>&1
diff -b macro06.chk macro06.out
if errorlevel 1 goto err6
    @echo # MACRO06 successful
    del macro06.out
    goto test7
:err6
    @echo ## MACRO ## >> %2
    @echo Error: MACRO #6 unsuccessful!!! | tee -a %2

:test7

echo # ---------------------------
echo #   Macro Test 7
echo # ---------------------------

echo. >hello.obj
echo. >hello2.obj
echo. >hello.c
echo. >hello2.c
%1 -h -ms -m -f macro07 -a cc=wcl386 > macro07.out 2>&1
diff -b macro07.chk macro07.out
if errorlevel 1 goto err7
    @echo # MACRO07 successful
    del macro07.out
    goto test8
:err7
    @echo ## MACRO ## >> %2
    @echo Error: MACRO #7 unsuccessful!!! | tee -a %2

:test8
echo # ---------------------------
echo #   Macro Test 8
echo # ---------------------------

%1 -h -f macro08 > macro08.out 2>&1
diff -b macro08.chk macro08.out
if errorlevel 1 goto err8
    @echo # MACRO08 successful
    del macro08.out
    goto test9
:err8
    @echo ## MACRO ## >> %2
    @echo Error: MACRO #8 unsuccessful!!! | tee -a %2


:test9

del hello.obj hello2.obj hello.boo hello.tmp hello.c hello2.c 
goto done
:usage
echo usage: %0 prgname errorfile
:done
