@echo off
echo # ===========================
echo # Start Preprocessor Test 
echo # ===========================

if .%2 == . goto usage

echo # ---------------------------
echo #   PreProcess Test 1
echo # ---------------------------

del tmp.out
%1 -h -f PREP01 -l tmp.out
diff -b PREP01.CMP tmp.out
if errorlevel 1 goto err1
    @echo # PREP01 successful
    goto test2
:err1 
    @echo ## PREPROCESS ## >> %2
    @echo Error: PREPROCESS #1 unsuccessful!!! | tee -a %2

:test2

echo # ---------------------------
echo #   PreProcess Test 2
echo # ---------------------------

del tmp.out
%1 -h -f PREP02 -ms -l tmp.out
diff PREP02.CMP tmp.out
if errorlevel 1 goto err2
    @echo # PREP02 successful
    goto test3
:err2 
    @echo ## PREPROCESS ## >> %2
    @echo Error: PREPROCESS #2 unsuccessful!!! | tee -a %2

:test3

goto done
:usage
echo usage: %0 prgname errorfile
:done
if exist tmp.out del tmp.out
