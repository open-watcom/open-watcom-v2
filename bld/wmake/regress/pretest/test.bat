@echo %verbose% off
ECHO # ===================================
ECHO # Start DOPRE
ECHO # ===================================


if .%2 == . goto usage

ECHO # -----------------------------
ECHO #   Test 1
ECHO # -----------------------------
:: 2>nul to hide debug-build, memory-tracking diagnostic
%1 -h -f pre01 pre01 -f pre02 pre02 2>nul
if errorlevel 1 goto err1
    echo # Test1 successful
    goto test2
:err1
    echo ## PRETEST ## >> %2
    echo Error: Test1 unsuccessful!!! | tee -a %2


:test2
ECHO # -----------------------------
ECHO #   Test 2
ECHO # -----------------------------
if exist pre03.out del pre03.out
%1 -h -f pre03 pre03 > pre03.out 2>&1
diff -i pre03.chk pre03.out
if errorlevel 1 goto err2
    echo # Test2 successful
    del pre03.out
    goto test3
:err2
    echo ## PRETEST ## >> %2
    echo Error: Test2 unsuccessful!!! | tee -a %2

:test3
ECHO # -----------------------------
ECHO #   Test 3
ECHO # -----------------------------
%1 -h -f pre04 pre04
if errorlevel 1 goto err3
    echo # Test3 successful
    goto test4
:err3
    echo ## PRETEST ## >> %2
    echo Error: Test3 unsuccessful!!! | tee -a %2

:test4
ECHO # -----------------------------
ECHO #   Test 4
ECHO # -----------------------------
%1 -h -f pre05 pre05
if errorlevel 1 goto err4
    echo # Test 4 successful
    goto test5
:err4
    echo ## PRETEST ## >> %2
    echo Error: Test 4 unsuccessful!!! | tee -a %2


:test5
ECHO # -----------------------------
ECHO #   Test 5
ECHO # -----------------------------
%1 -h -f pre06 pre06
if errorlevel 1 goto err5
    echo # Test 5 successful
    goto test6
:err5
    echo ## PRETEST ## >> %2
    echo Error: Test 5 unsuccessful!!! | tee -a %2


:test6
ECHO # -----------------------------
ECHO #   Test 6
ECHO # -----------------------------
if exist pre07.out del pre07.out
%1 -h -f pre07 > pre07.out 2>&1
diff -i pre07.chk pre07.out
if errorlevel 1 goto err6
    echo # Test 6 successful
    del pre07.out
    goto test7
:err6
    echo ## PRETEST ## >> %2
    echo Error: Test 6 unsuccessful!!! | tee -a %2

:test7
ECHO # -----------------------------
ECHO #   Test 7
ECHO # -----------------------------
if exist pre08.out del pre08.out
%1 -h -f pre08 > pre08.out 2>&1
if errorlevel 1 goto err7
diff pre08.chk pre08.out
if errorlevel 1 goto err7
    echo # Test 7 successful
    del pre08.out
    goto test8
:err7
    echo ## PRETEST ## >> %2
    echo Error: Test 7 unsuccessful!!! | tee -a %2
:test8
goto done
:usage
echo usage: %0 prgname errorfile
:done
