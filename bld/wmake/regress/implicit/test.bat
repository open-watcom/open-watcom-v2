@echo off
wtouch hello.obj
wtouch hello.cpp
echo # ===========================
echo # Implicit Rules Test
echo # ===========================
if .%2 == . goto usage

echo # ---------------------------
echo # IMPLICIT RULES TEST 1 
echo # ---------------------------
del err1.out
%1 -h -l err1.out >tst1.out
diff tst1.out tst1.chk
if errorlevel 1 goto tst2err
if exist err1.out goto tst2err
    echo # Implicit Rules Test successful
    goto test2
:tst2err
    echo ## IMPLICIT RULES TEST 1 ## >> %2
    echo !!! Error: Test Not Successful !!!  | tee -a %2
 
:test2
    
echo # ---------------------------
echo # IMPLICIT RULES TEST 2A
echo # ---------------------------
wtouch hello.h
del tst1.out
%1 -h -c -f IMP02A -l tst1.out
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
wtouch hello.h
del tst1.out
%1 -c -h -f IMP02B -l tst1.out
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
wtouch hello.h
del tst1.out
%1 -h -c -f IMP02C /ms -l tst1.out
diff IMP02.CMP tst1.out
if errorlevel 1 goto err2c
    echo # Implicit Rules Test successful
    goto test3
:err2c
    echo ## IMPLICIT RULES TEST 2C ## >> %2
    echo !!! Error: Test Not Successful !!!  | tee -a %2
   
:test3 
goto done
:done
    del *.obj
    del tst1.out
    del tst2.out
    del app.lnk
    del app.exe
    del hello.cpp
    del hello.h
    
goto end    
:usage
echo usage: %0 prgname errorfile
:end
