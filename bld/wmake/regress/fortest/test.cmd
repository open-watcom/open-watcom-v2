echo off
echo # ===========================
echo # Start FORTEST
echo # ===========================
if .%1 == . goto usage

echo # ---------------------------
echo # Test A
echo # ---------------------------
rm tst2.out
%1 -h -f FOR01 > tst2.out 2>&1
diff tst2.out FOR01.cmp
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
rm tst2.out
%1 -h -f FOR02 > tst2.out 2>&1
diff tst2.out FOR02.cmp
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
rm tst2.out
%1 -h -f FOR03 > tst2.out 2>&1
diff tst2.out FOR03.cmp
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
type FOR04a.cmp > tmpfile.tmp
dir /b >> tmpfile.tmp
type FOR04b.cmp >> tmpfile.tmp
dir for?? /b >> tmpfile.tmp
type FOR04c.cmp >> tmpfile.tmp
rm tst2.out
%1 -h -f FOR04 > tst2.out 2>&1
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
rm tst2.out
%1 -h -f FOR05 > tst2.out 2>&1
diff tst2.out FOR05.cmp
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
rm tst2.out
%1 -h -f FOR06 > tst2.out 2>&1
diff tst2.out FOR06.cm2
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
rm tst2.out
%1 -h -f FOR07 > tst2.out 2>&1
diff tst2.out FOR07.cmp
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
rm prntdir.exe
wcl386 prntdir.c -ox-d1-zq
type FOR08.cmp > tmpfile.tmp
prntdir "echo a" >> tmpfile.tmp
echo a >> tmpfile.tmp
prntdir "echo b" >> tmpfile.tmp
echo b >> tmpfile.tmp
prntdir "echo c" >> tmpfile.tmp
echo c >> tmpfile.tmp
rm tst2.out
%1 -h -f FOR08 > tst2.out 2>&1
diff -b tst2.out tmpfile.tmp
if errorlevel 1 goto tst2herr
    echo # For Loop Test H successful
    goto done
:tst2herr
    echo # Error: For Loop Test H did not work
:err
:done
    rm tmpfile.tmp
    rm tst2.out
    rm temp.out
    rm *.obj

goto end
:usage
echo usage: %0 prgname errorfile
:end
