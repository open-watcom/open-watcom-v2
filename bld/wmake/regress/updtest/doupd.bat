@echo off
echo # ===========================
echo # Start UPDTEST 
echo # ===========================

if .%2 == . goto usage

echo # ---------------------------
echo #   Test 1
echo # ---------------------------

%1 -h -f upd01
if errorlevel 1 goto err1
    @echo # UPD01 successful
    goto test2
:err1 
    @echo ## UPDTEST ## >> %2
    @echo Error: UPD01 unsuccessful!!! | tee -a %2

:test2
echo # ---------------------------
echo #   Test 2
echo # ---------------------------
%1 -h -f upd02 /c
if errorlevel 1 goto err2
    echo # UPD02 successful
    goto test3
:err2 
    @echo ## UPDTEST ## >> %2
    @echo # Error: UPD02 unsuccessful!!! | tee -a %2

:test3
echo # ---------------------------
echo #   Test 3
echo # ---------------------------
%1 -h -f upd03
if errorlevel 1 goto err3
    echo # UPD03 successful
    goto test4
:err3 
    @echo ## UPDTEST ## >> %2
    @echo # Error: UPD03 unsuccessful!!! | tee -a %2

:test4
echo # ---------------------------
echo #   Test 4
echo # ---------------------------
set TRMEM_CODE=3
del test4.out
%1 -h -f upd04 -l test4.out
diff -b upd04.out test4.out
if errorlevel 1 goto err4
    echo # UPD04 successful
    goto test5
:err4 
    @echo ## UPDTEST ## >> %2
    @echo # Error: UPD04 unsuccessful!!! | tee -a %2
    
:test5
set TRMEM_CODE=1
echo # ---------------------------
echo #   Test 5
echo # ---------------------------
del test5.out
%1 -h -s -f upd05 -l test5.out
diff -b upd05.out test5.out
if errorlevel 1 goto err5
    echo # UPD05 successful
    goto test6
:err5 
    @echo ## UPDTEST ## >> %2
    @echo # Error: UPD05 unsuccessful!!! | tee -a %2

:test6
echo # ---------------------------
echo #   Test 6
echo # ---------------------------
del test6.out
%1 -h -f upd06 -l test6.out
diff upd06.out test6.out
if errorlevel 1 goto err6
    echo # UPD06 successful
    goto test7
:err6 
    @echo ## UPDTEST ## >> %2
    @echo # Error: UPD06 unsuccessful!!! | tee -a %2

:test7
echo # ---------------------------
echo #   Test 7
echo # ---------------------------
del test7.out
%1 -h -f upd07 -l test7.out
diff upd07.out test7.out
if errorlevel 1 goto err7
    echo # UPD07 successful
    goto test8
:err7 
    @echo ## UPDTEST ## >> %2
    @echo # Error: UPD07 unsuccessful!!! | tee -a %2

:test8
echo # ---------------------------
echo #   Test 8  ---> ONLY FOR DOS
echo # ---------------------------
REM %1 -h -f upd08 >tmp.out
REM if errorlevel 1 goto err
REM diff upd08.out tmp.out
REM if errorlevel 1 goto err8
REM echo # UPD08 successful
REM goto test9
REM :err8 
REM @echo ## UPDTEST ## >> %2
REM @echo # Error: UPD08 unsuccessful!!! | tee -a %2

:test9
echo # ---------------------------
echo #   Test 9
echo # ---------------------------
%1 -h -f upd09
if errorlevel 1 goto err9
    echo # UPD09 successful
    goto test10
:err9 
    @echo ## UPDTEST ## >> %2
    @echo # Error: UPD09 unsuccessful!!! | tee -a %2

:test10

goto done
:usage
echo usage: %0 prgname errorfile
:done
if exist tmp.out del tmp.out


