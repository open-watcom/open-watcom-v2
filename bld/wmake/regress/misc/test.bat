@echo off
echo # ===========================
echo # Start Miscellaneous Test 
echo # ===========================

if .%2 == . goto usage

echo # ---------------------------
echo #   Miscellaneous Test 1
echo # ---------------------------

del tmp.out
%1 -c -h -f MISC01 -l tmp.out
diff -b MISC01.CMP tmp.out
if errorlevel 1 goto err1
    @echo # MISC01 successful
    goto test2
:err1 
    @echo ## MISC ## >> %2
    @echo Error: MISC #1 unsuccessful!!! | tee -a %2

:test2

echo # ---------------------------
echo #   Miscellaneous Test 2
echo # ---------------------------

del tmp.out
%1 -c -h -f MISC02 -l tmp.out
diff -b MISC02.CMP tmp.out
if errorlevel 1 goto err2
    @echo # MISC02 successful
    goto test3
:err2
    @echo ## MISC ## >> %2
    @echo Error: MISC #2 unsuccessful!!! | tee -a %2

:test3

echo # ---------------------------
echo #   Miscellaneous Test 3
echo # ---------------------------

del tmp.out
%1 -a -c -h -f MISC03 -l tmp.out
diff -b MISC03.CMP tmp.out
if errorlevel 1 goto err3
    @echo # MISC03 successful
    goto test4
:err3
    @echo ## MISC ## >> %2
    @echo Error: MISC #3 unsuccessful!!! | tee -a %2

:test4

echo # ---------------------------
echo #   Miscellaneous Test 4
echo # ---------------------------

del tmp.out
%1 -a -c -h -f MISC04 -l tmp.out
diff -b MISC04.CMP tmp.out
if errorlevel 1 goto err4
    @echo # MISC04 successful
    goto test5
:err4
    @echo ## MISC ## >> %2
    @echo Error: MISC #4 unsuccessful!!! | tee -a %2

:test5

echo # ---------------------------
echo #   Miscellaneous Test 5
echo # ---------------------------

del tmp.out
%1 -a -c -h -f MISC05 test1 test2 test3 -l tmp.out
diff -b MISC05.CMP tmp.out
if errorlevel 1 goto err5
    @echo # MISC05 successful
    goto test6
:err5
    @echo ## MISC ## >> %2
    @echo Error: MISC #5 unsuccessful!!! | tee -a %2

:test6

goto done
:usage
echo usage: %0 prgname errorfile
:done
if exist tmp.out del tmp.out
