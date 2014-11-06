@echo off
echo # ===========================
echo # Start Miscellaneous Test
echo # ===========================

if .%2 == . goto usage

echo # ---------------------------
echo #   Miscellaneous Test 1
echo # ---------------------------

%1 -c -h -f misc01 > tmp.out 2>&1
diff -b misc01.chk tmp.out
if errorlevel 1 goto err1
    @echo # misc01 successful
    goto test2
:err1
    @echo ## MISC ## >> %2
    @echo Error: MISC #1 unsuccessful!!! | tee -a %2

:test2

echo # ---------------------------
echo #   Miscellaneous Test 2
echo # ---------------------------

%1 -c -h -f misc02 > tmp.out 2>&1
diff -b misc02.chk tmp.out
if errorlevel 1 goto err2
    @echo # misc02 successful
    goto test3
:err2
    @echo ## MISC ## >> %2
    @echo Error: MISC #2 unsuccessful!!! | tee -a %2

:test3

echo # ---------------------------
echo #   Miscellaneous Test 3
echo # ---------------------------

%1 -a -c -h -f misc03 > tmp.out 2>&1
diff -b misc03.chk tmp.out
if errorlevel 1 goto err3
    @echo # misc03 successful
    goto test4
:err3
    @echo ## MISC ## >> %2
    @echo Error: MISC #3 unsuccessful!!! | tee -a %2

:test4

echo # ---------------------------
echo #   Miscellaneous Test 4
echo # ---------------------------

%1 -a -c -h -f misc04 > tmp.out 2>&1
diff -b -i misc04.chk tmp.out
if errorlevel 1 goto err4
    @echo # misc04 successful
    goto test5
:err4
    @echo ## MISC ## >> %2
    @echo Error: MISC #4 unsuccessful!!! | tee -a %2

:test5

echo # ---------------------------
echo #   Miscellaneous Test 5
echo # ---------------------------

%1 -a -c -h -f misc05 test1 test2 test3 > tmp.out 2>&1
diff -b misc05.chk tmp.out
if errorlevel 1 goto err5
    @echo # misc05 successful
    goto test6
:err5
    @echo ## MISC ## >> %2
    @echo Error: MISC #5 unsuccessful!!! | tee -a %2

:test6

echo # ---------------------------
echo #   Miscellaneous Test 6
echo # ---------------------------

%1 -a -c -h -f misc06 > tmp.out 2>&1
diff -b -i misc06.chk tmp.out
if errorlevel 1 goto err6
    @echo # misc06 successful
    goto test7
:err6
    @echo ## MISC ## >> %2
    @echo Error: MISC #6 unsuccessful!!! | tee -a %2

:test7

echo # ---------------------------
echo #   Miscellaneous Test 7
echo # ---------------------------

%1 -a -c -h -f misc07 > tmp.out 2>&1
diff -b misc07.chk tmp.out
if errorlevel 1 goto err7
    @echo # misc07 successful
    goto test8
:err7
    @echo ## MISC ## >> %2
    @echo Error: MISC #7 unsuccessful!!! | tee -a %2

:test8

goto done
:usage
echo usage: %0 prgname errorfile
:done
del tmp.out
