@echo %verbose% off
echo # ===========================
echo # Implicit Rules Test
echo # ===========================
if .%2 == . goto usage
wtouch hello.obj
wtouch hello.cpp

echo # ---------------------------
echo # IMPLICIT RULES TEST 1
echo # ---------------------------
%1 -h -l err1.out > tst1.out
if errorlevel 1 goto tst2err
    echo # Implicit Rules Test successful
    goto test2
:tst2err
    echo ## IMPLICIT RULES TEST 1 ## >> %2
    echo !!! Error: Test Not Successful !!!  | tee -a %2

:test2

echo # ---------------------------
echo # IMPLICIT RULES TEST 2A
echo # ---------------------------
sleep 2
wtouch hello.h
%1 -h -c -f IMP02A > tst1.out
diff IMP02.CMP tst1.out
if errorlevel 1 goto err2
    echo # Implicit Rules Test successful
    goto test2b
:err2
    echo ## IMPLICIT RULES TEST 2A ## >> %2
    echo !!! Error: Test Not Successful !!!  | tee -a %2

:test2b

echo # ---------------------------
echo # IMPLICIT RULES TEST 2B
echo # ---------------------------
sleep 2
wtouch hello.h
%1 -c -h -f IMP02B > tst1.out
diff IMP02.CMP tst1.out
if errorlevel 1 goto err2b
    echo # Implicit Rules Test successful
    goto test2c
:err2b
    echo ## IMPLICIT RULES TEST 2B ## >> %2
    echo !!! Error: Test Not Successful !!!  | tee -a %2

:test2c

echo # ---------------------------
echo # IMPLICIT RULES TEST 2C
echo # ---------------------------
sleep 2
wtouch hello.h
%1 -h -c -f IMP02C /ms /m > tst1.out
diff IMP02.CMP tst1.out
if errorlevel 1 goto err2c
    echo # Implicit Rules Test successful
    goto test2d
:err2c
    echo ## IMPLICIT RULES TEST 2C ## >> %2
    echo !!! Error: Test Not Successful !!!  | tee -a %2

:test2d

echo # ---------------------------
echo # IMPLICIT RULES TEST 2D
echo # ---------------------------
rm -f hello.obj
%1 -f IMP02D -h > tst1.out
sed "s:of .*[\\/]:of :" tst1.out | diff IMP02D.CMP -
if errorlevel 1 goto err2d
    echo # Implicit Rules Test successful
    goto test3
:err2d
    echo ## IMPLICIT RULES TEST 2D ## >> %2
    echo !!! Error: Test Not Successful !!!  | tee -a %2

:test3
goto done
:done
    :: hello.* hello?.* uses OW and Linux rm compatible wildcards. hello* no go
    rm -f *.obj err1.out tst1.out tst2.out app.lnk app.exe hello.* hello?.*

goto end
:usage
echo usage: %0 prgname errorfile
:end
