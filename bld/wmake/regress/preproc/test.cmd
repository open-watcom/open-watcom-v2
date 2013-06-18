@echo %verbose% off
echo # ===========================
echo # Start Preprocessor Test
echo # ===========================

if .%2 == . goto usage

echo # ---------------------------
echo #   PreProcess Test 1
echo # ---------------------------

%1 -h -f prep01 > tmp.out 2>&1
diff -b -i prep01.chk tmp.out
if errorlevel 1 goto err1
    @echo # prep01 successful
    goto test2
:err1
    @echo ## PREPROCESS ## >> %2
    @echo Error: PREPROCESS #1 unsuccessful!!! | tee -a %2

:test2

echo # ---------------------------
echo #   PreProcess Test 2
echo # ---------------------------

%1 -h -f prep02 -m -ms > tmp.out
diff prep02.chk tmp.out
if errorlevel 1 goto err2
%1 -h -f prep02 -m     > tmp.out
diff prep02.chk tmp.out
if errorlevel 1 goto err2
    @echo # prep02 successful
    goto test3
:err2
    @echo ## PREPROCESS ## >> %2
    @echo Error: PREPROCESS #2 unsuccessful!!! | tee -a %2

:test3

echo # ---------------------------
echo #   PreProcess Test 3
echo # ---------------------------

%1 -h -f prep03 > tmp.out 2>&1
diff -b -i prep03.chk tmp.out
if errorlevel 1 goto err3
    @echo # prep03 successful
    goto test4
:err3
    @echo ## PREPROCESS ## >> %2
    @echo Error: PREPROCESS #3 unsuccessful!!! | tee -a %2

:test4

echo # ---------------------------
echo #   PreProcess Test 4 
echo # ---------------------------

%1 -h -f prep04 > tmp.out 2>&1
diff -b prep04.chk tmp.out
if errorlevel 1 goto err4
    @echo # prep04 successful
    goto test5
:err4
    @echo ## PREPROCESS ## >> %2
    @echo Error: PREPROCESS #4 unsuccessful!!! | tee -a %2

:test5

goto done
:usage
echo usage: %0 prgname errorfile
:done
rm -f tmp.out
