@echo %verbose% off
echo # ===========================
echo # Implicit Rules Test
echo # ===========================
if .%2 == . goto usage
echo. >hello.obj
echo. >hello.cpp

echo # ---------------------------
echo # IMPLICIT RULES TEST 1
echo # ---------------------------
if exist err1.out del err1.out
if exist imp02.out del imp02.out
%1 -h -l err1.out > imp02.out
if errorlevel 1 goto tst2err
    echo # Implicit Rules Test successful
    del err1.out
    del imp02.out
    goto test2
:tst2err
    echo ## IMPLICIT RULES TEST 1 ## >> %2
    echo !!! Error: Test Not Successful !!!  | tee -a %2
:test2

echo # ---------------------------
echo # IMPLICIT RULES TEST 2A
echo # ---------------------------
if exist imp02a.out del imp02a.out
sleep 2
echo. >hello.h
%1 -h -c -f imp02a > imp02a.out
diff imp02.chk imp02a.out
if errorlevel 1 goto err2
    echo # Implicit Rules Test successful
    del imp02a.out
    goto test2b
:err2
    echo ## IMPLICIT RULES TEST 2A ## >> %2
    echo !!! Error: Test Not Successful !!!  | tee -a %2

:test2b

echo # ---------------------------
echo # IMPLICIT RULES TEST 2B
echo # ---------------------------
if exist imp02b.out del imp02b.out
sleep 2
echo. >hello.h
%1 -c -h -f imp02b > imp02b.out
diff imp02.chk imp02b.out
if errorlevel 1 goto err2b
    echo # Implicit Rules Test successful
    del imp02b.out
    goto test2c
:err2b
    echo ## IMPLICIT RULES TEST 2B ## >> %2
    echo !!! Error: Test Not Successful !!!  | tee -a %2

:test2c

echo # ---------------------------
echo # IMPLICIT RULES TEST 2C
echo # ---------------------------
if exist imp02c.out del imp02c.out
sleep 2
echo. >hello.h
%1 -h -c -f imp02c /ms /m > imp02c.out
diff imp02.chk imp02c.out
if errorlevel 1 goto err2c
    echo # Implicit Rules Test successful
    del imp02c.out
    goto test2d
:err2c
    echo ## IMPLICIT RULES TEST 2C ## >> %2
    echo !!! Error: Test Not Successful !!!  | tee -a %2

:test2d

echo # ---------------------------
echo # IMPLICIT RULES TEST 2D
echo # ---------------------------
if exist imp02d1.out del imp02d1.out
if exist imp02d2.out del imp02d2.out
del hello.obj
%1 -f imp02d -h > imp02d1.out
sed "s:of .*[\\/]:of :" imp02d1.out > imp02d2.out
diff imp02d.chk imp02d2.out
if errorlevel 1 goto err2d
    echo # Implicit Rules Test successful
    del imp02d1.out
    del imp02d2.out
    goto test3
:err2d
    echo ## IMPLICIT RULES TEST 2D ## >> %2
    echo !!! Error: Test Not Successful !!!  | tee -a %2

:test3
goto done
:done
    :: hello.* hello?.* uses OW and Linux rm compatible wildcards. hello* no go
    del *.obj app.lnk app.exe hello.* hello?.*

goto end
:usage
echo usage: %0 prgname errorfile
:end
