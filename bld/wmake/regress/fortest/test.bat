@echo %verbose% off
echo # ===========================
echo # Start FORTEST
echo # ===========================
if .%1 == . goto usage

echo # ---------------------------
echo # Test A
echo # ---------------------------
rm -f tst2.out
%1 -h -f for01 > tst2.out 2>&1
diff tst2.out for01.chk
if errorlevel 1 goto tst2aerr
    echo # For Loop Test A successful
    goto tst2b
:tst2aerr
    echo ## FORTEST ## >> %2
    echo # Error: For Loop Test A did not work | tee -a %2
    goto err

:tst2b
echo # ---------------------------
echo # Test B
echo # ---------------------------
rm -f tst2.out
%1 -h -f for02 > tst2.out 2>&1
diff tst2.out for02.chk
if errorlevel 1 goto tst2berr
    echo # For Loop Test B successful
    goto tst2c
:tst2berr
    echo ## FORTEST ## >> %2
    echo # Error: For Loop Test B did not work | tee -a %2
    goto err

:tst2c
echo # ---------------------------
echo # Test C
echo # ---------------------------
rm -f tst2.out
%1 -h -f for03 > tst2.out 2>&1
diff tst2.out for03.chk
if errorlevel 1 goto tst2cerr
    echo # For Loop Test C successful
    goto tst2d
:tst2cerr
    echo ## FORTEST ## >> %2
    echo # Error: For Loop Test C did not work | tee -a %2
    goto err

:tst2d
echo # ---------------------------
echo # Test D
echo # ---------------------------
type for04a.chk   > tmpfile.tmp
dir /b /-o       >> tmpfile.tmp
type for04b.chk  >> tmpfile.tmp
dir /b /-o for?? >> tmpfile.tmp
type for04c.chk  >> tmpfile.tmp
%1 -h -f for04 > tst2.out 2>&1
diff tst2.out tmpfile.tmp
if errorlevel 1 goto tst2derr
    echo # For Loop Test D successful
    goto tst2e
:tst2derr
    echo ## FORTEST ## >> %2
    echo # Error: For Loop Test D did not work | tee -a %2
    goto err

:tst2e
echo # ---------------------------
echo # Test E
echo # ---------------------------
rm -f tst2.out
%1 -h -f for05 > tst2.out 2>&1
diff tst2.out for05.chk
if errorlevel 1 goto tst2eerr
    echo # For Loop Test E successful
    goto tst2f
:tst2eerr
    echo ## FORTEST ## >> %2
    echo # Error: For Loop Test E did not work | tee -a %2
    goto err

:tst2f
echo # ---------------------------
echo # Test F
echo # ---------------------------
rm -f tst2.out
%1 -h -f for06 > tst2.out 2>&1
diff tst2.out for06.chk
if errorlevel 1 goto tst2ferr
    echo # For Loop Test F successful
    goto tst2g
:tst2ferr
    echo ## FORTEST ## >> %2
    echo # Error: For Loop Test F did not work | tee -a %2
    goto err

:tst2g
echo # ---------------------------
echo # Test G
echo # ---------------------------
rm -f tst2.out
%1 -h -f for07 > tst2.out 2>&1
diff tst2.out for07.chk
if errorlevel 1 goto tst2gerr
    echo # For Loop Test G successful
    goto tst2h
:tst2gerr
    echo ## FORTEST ## >> %2
    echo # Error: For Loop Test G did not work | tee -a %2
    goto err

:tst2h
echo # ---------------------------
echo # Test H
echo # ---------------------------
rem Need to set prompt, otherwise the test fails...
prompt $p$g
type for08.chk > tmpfile.tmp
..\cmds\prntdir "echo a" >> tmpfile.tmp
echo a >> tmpfile.tmp
..\cmds\prntdir "echo b" >> tmpfile.tmp
echo b >> tmpfile.tmp
..\cmds\prntdir "echo c" >> tmpfile.tmp
echo c >> tmpfile.tmp
rm -f tst2.out
%1 -h -f for08 > tst2.out 2>&1
diff -b tst2.out tmpfile.tmp
if errorlevel 1 goto tst2herr
    echo # For Loop Test H successful
    goto done
:tst2herr
    echo # Error: For Loop Test H did not work
:err
:done
    if not .%verbose% == . goto end
    rm -f tmpfile.tmp
    rm -f tst2.out
    rm -f *.obj

goto end
:usage
echo usage: %0 prgname errorfile
:end
