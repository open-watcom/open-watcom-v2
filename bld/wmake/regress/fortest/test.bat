@echo %verbose% off
echo # ===========================
echo # Start FORTEST
echo # ===========================
if .%1 == . goto usage

echo # ---------------------------
echo # Test A
echo # ---------------------------
if exist for01.out del for01.out
%1 -h -f for01 > for01.out 2>&1
diff for01.out for01.chk
if errorlevel 1 goto tst2aerr
    echo # For Loop Test A successful
    del for01.out
    goto tst2b
:tst2aerr
    echo ## FORTEST ## >> %2
    echo # Error: For Loop Test A did not work | tee -a %2
    goto err

:tst2b
echo # ---------------------------
echo # Test B
echo # ---------------------------
if exist for02.out del for02.out
%1 -h -f for02 > for02.out 2>&1
diff for02.out for02.chk
if errorlevel 1 goto tst2berr
    echo # For Loop Test B successful
    del for02.out
    goto tst2c
:tst2berr
    echo ## FORTEST ## >> %2
    echo # Error: For Loop Test B did not work | tee -a %2
    goto err

:tst2c
echo # ---------------------------
echo # Test C
echo # ---------------------------
if exist for03.out del for03.out
%1 -h -f for03 > for03.out 2>&1
diff for03.out for03.chk
if errorlevel 1 goto tst2cerr
    echo # For Loop Test C successful
    del for03.out
    goto tst2d
:tst2cerr
    echo ## FORTEST ## >> %2
    echo # Error: For Loop Test C did not work | tee -a %2
    goto err

:tst2d
echo # ---------------------------
echo # Test D
echo # ---------------------------
if exist tmp04.chk del tmp04.chk
echo. > for04.out
type for04a.chk   > tmp04.chk
dir /b /-o       >> tmp04.chk
type for04b.chk  >> tmp04.chk
dir /b /-o for?? >> tmp04.chk
type for04c.chk  >> tmp04.chk
%1 -h -f for04 > for04.out 2>&1
diff for04.out tmp04.chk
if errorlevel 1 goto tst2derr
    echo # For Loop Test D successful
    del for04.out
    del tmp04.chk
    goto tst2e
:tst2derr
    echo ## FORTEST ## >> %2
    echo # Error: For Loop Test D did not work | tee -a %2
    goto err

:tst2e
echo # ---------------------------
echo # Test E
echo # ---------------------------
if exist for05.out del for05.out
%1 -h -f for05 > for05.out 2>&1
diff for05.out for05.chk
if errorlevel 1 goto tst2eerr
    echo # For Loop Test E successful
    del for05.out
    goto tst2f
:tst2eerr
    echo ## FORTEST ## >> %2
    echo # Error: For Loop Test E did not work | tee -a %2
    goto err

:tst2f
echo # ---------------------------
echo # Test F
echo # ---------------------------
if exist for06.out del for06.out
%1 -h -f for06 > for06.out 2>&1
diff for06.out for06.chk
if errorlevel 1 goto tst2ferr
    echo # For Loop Test F successful
    del for06.out
    goto tst2g
:tst2ferr
    echo ## FORTEST ## >> %2
    echo # Error: For Loop Test F did not work | tee -a %2
    goto err

:tst2g
echo # ---------------------------
echo # Test G
echo # ---------------------------
if exist for07.out del for07.out
%1 -h -f for07 > for07.out 2>&1
diff for07.out for07.chk
if errorlevel 1 goto tst2gerr
    echo # For Loop Test G successful
    del for07.out
    goto tst2h
:tst2gerr
    echo ## FORTEST ## >> %2
    echo # Error: For Loop Test G did not work | tee -a %2
    goto err

:tst2h
echo # ---------------------------
echo # Test H
echo # ---------------------------
if exist for08.out del for08.out
echo. > for08.out
rem Need to set prompt, otherwise the test fails...
prompt $p$g
type for08.chk > tmp08.chk
..\cmds\prntdir "echo a" >> tmp08.chk
echo a >> tmp08.chk
..\cmds\prntdir "echo b" >> tmp08.chk
echo b >> tmp08.chk
..\cmds\prntdir "echo c" >> tmp08.chk
echo c >> tmp08.chk
%1 -h -f for08 > for08.out 2>&1
diff -b for08.out tmp08.chk
if errorlevel 1 goto tst2herr
    echo # For Loop Test H successful
    del for08.out
    del tmp08.chk
    goto done
:tst2herr
    echo # Error: For Loop Test H did not work
:err
:done
    if not .%verbose% == . goto end
    if exist *.obj del *.obj

goto end
:usage
echo usage: %0 prgname errorfile
:end
