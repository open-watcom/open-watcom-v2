@echo %verbose% off
echo # ===========================
echo # Start INLINE File Test
echo # ===========================

if .%2 == . goto usage

echo # ---------------------------
echo #   Inline File Test 1
echo # ---------------------------

%1 -h -f inline01 > tmp.out 2>&1
diff inline01.chk tmp.out
if errorlevel 1 goto err1
for %%e in (1 2 3) do if not exist test.%%e goto err1
    @echo # inline01 successful
    goto test2
:err1
    @echo ## INLINE ## >> %2
    @echo Error: INLINE #1 unsuccessful!!! | tee -a %2

:test2

echo # ---------------------------
echo #   Inline File Test 2
echo # ---------------------------

%1 -h -f inline02 > tmp.out 2>&1
diff inline02.chk tmp.out
if errorlevel 1 goto err2
for %%e in (1 2 3) do if exist test.%%e goto err2
    @echo # inline02 successful
    goto test3
:err2
    @echo ## INLINE ## >> %2
    @echo Error: INLINE #2 unsuccessful!!! | tee -a %2

:test3

echo # ---------------------------
echo #   Inline File Test 3
echo # ---------------------------
%1 -h -f inline03 > tmp.out 2>&1
diff inline03.chk tmp.out
if errorlevel 1 goto err3
    @echo # inline03 successful
    goto test4
:err3
    @echo ## INLINE ## >> %2
    @echo Error: INLINE #3 unsuccessful!!! | tee -a %2

:test4
echo # ---------------------------
echo #   Inline File Test 4
echo # ---------------------------

%1 -h -f inline04 > tmp.out 2>&1
diff inline04.chk tmp.out
if errorlevel 1 goto err4
    @echo # inline04 successful
    goto test5
:err4
    @echo ## INLINE ## >> %2
    @echo Error: INLINE #4 unsuccessful!!! | tee -a %2

:test5

echo # ---------------------------
echo #   Inline File Test 5
echo # ---------------------------

%1 -h -f inline05 > tmp.out 2>&1
diff -i inline05.chk tmp.out
if errorlevel 1 goto err5
    @echo # inline05 successful
    goto test6
:err5
    @echo ## INLINE ## >> %2
    @echo Error: INLINE #5 unsuccessful!!! | tee -a %2

:test6

echo # ---------------------------
echo #   Inline File Test 6
echo # ---------------------------

%1 -h -f inline06 > tmp.out 2>&1
diff -i inline06.chk tmp.out
if errorlevel 1 goto err6
    @echo # inline06 successful
    goto test7
:err6
    @echo ## INLINE ## >> %2
    @echo Error: INLINE #6 unsuccessful!!! | tee -a %2

:test7

echo # ---------------------------
echo #   Inline File Test 7
echo # ---------------------------

%1 -h -f inline07 > tmp.out 2>&1
diff inline07.chk tmp.out
if errorlevel 1 goto err7
    @echo # inline07 successful
    goto test8
:err7
    @echo ## INLINE ## >> %2
    @echo Error: INLINE #7 unsuccessful!!! | tee -a %2

:test8
goto done
:usage
echo usage: %0 prgname errorfile
:done
del tmp.out
