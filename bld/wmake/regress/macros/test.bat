@echo off
echo # ===========================
echo # Start Macro Test 
echo # ===========================

if .%2 == . goto usage

echo # ---------------------------
echo #   Macro Test 1
echo # ---------------------------

del tmp.out
%1 -ms -h -f MACRO01 -l tmp.out
diff -b MACRO01.CMP tmp.out
if errorlevel 1 goto err1
    @echo # MACRO01 successful
    goto test2
:err1 
    @echo ## MACRO ## >> %2
    @echo Error: MACRO #1 unsuccessful!!! | tee -a %2

:test2

echo # ---------------------------
echo #   Macro Test 2A
echo # ---------------------------

del tmp.out
%1 -h -f MACRO02 -l tmp.out
diff -b MACRO02A.CMP tmp.out
if errorlevel 1 goto err2a
    @echo # MACRO02A successful
    goto test2b
:err2a
    @echo ## MACRO ## >> %2
    @echo Error: MACRO #2A unsuccessful!!! | tee -a %2

:test2b
echo # ---------------------------
echo #   Macro Test 2B
echo # ---------------------------

del tmp.out
%1 -h -ms -f MACRO02 -l tmp.out
diff -b MACRO02B.CMP tmp.out
if errorlevel 1 goto err2b
    @echo # MACRO02B successful
    goto test3
:err2b
    @echo ## MACRO ## >> %2
    @echo Error: MACRO #2B unsuccessful!!! | tee -a %2

:test3

echo # ---------------------------
echo #   Macro Test 3A
echo # ---------------------------
set TRMEM_CODE=3
del tmp.out
%1 -h -f MACRO03 -l tmp.out
diff -b MACRO03A.CMP tmp.out
if errorlevel 1 goto err3a
    @echo # MACRO03A successful
    goto test3b
:err3a
    @echo ## MACRO ## >> %2
    @echo Error: MACRO #3A unsuccessful!!! | tee -a %2

:test3b
set TRMEM_CODE=1
echo # ---------------------------
echo #   Macro Test 3B
echo # ---------------------------

del tmp.out
%1 -h -ms -f MACRO03 -l tmp.out
diff -b MACRO03B.CMP tmp.out
if errorlevel 1 goto err3b
    @echo # MACRO03B successful
    goto test4
:err3b
    @echo ## MACRO ## >> %2
    @echo Error: MACRO #3B unsuccessful!!! | tee -a %2

:test4

echo # ---------------------------
echo #   Macro Test 4A
echo # ---------------------------

del tmp.out
%1 -h -f MACRO04 -l tmp.out
diff -b MACRO04A.CMP tmp.out
if errorlevel 1 goto err4a
    @echo # MACRO04A successful
    goto test4b
:err4a
    @echo ## MACRO ## >> %2
    @echo Error: MACRO #4A unsuccessful!!! | tee -a %2

:test4b
echo # ---------------------------
echo #   Macro Test 4B
echo # ---------------------------

del tmp.out
%1 -h -ms -f MACRO04 -l tmp.out
diff -b MACRO04B.CMP tmp.out
if errorlevel 1 goto err4b
    @echo # MACRO0r4B successful
    goto test5
:err4b
    @echo ## MACRO ## >> %2
    @echo Error: MACRO #4B unsuccessful!!! | tee -a %2

:test5

echo # ---------------------------
echo #   Macro Test 5
echo # ---------------------------

wtouch hello.boo hello.tmp hello.c
del tmp.out
%1 -h -ms -f MACRO05 -l tmp.out
diff -b MACRO05.CMP tmp.out
if errorlevel 1 goto err5
    @echo # MACRO05 successful
    goto test6
:err5
    @echo ## MACRO ## >> %2
    @echo Error: MACRO #5 unsuccessful!!! | tee -a %2

:test6

echo # ---------------------------
echo #   Macro Test 6
echo # ---------------------------

wtouch hello.obj hello2.obj 
wtouch hello.c  hello2.c
del tmp.out
%1 -h -ms -f MACRO06 -a -l tmp.out
diff -b MACRO06.CMP tmp.out
if errorlevel 1 goto err6
    @echo # MACRO06 successful
    goto test7
:err6
    @echo ## MACRO ## >> %2
    @echo Error: MACRO #6 unsuccessful!!! | tee -a %2

:test7

rm hello.obj hello2.obj hello.c hello2.c
goto done
:usage
echo usage: %0 prgname errorfile
:done
if exist tmp.out del tmp.out
