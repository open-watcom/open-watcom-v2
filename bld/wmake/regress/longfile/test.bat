@echo off
echo # ===========================
echo # Start Long FileName Test 
echo # ===========================

if .%2 == . goto usage

echo # ---------------------------
echo #   Long FileName Test 1
echo # ---------------------------
echo LONGFILENAME OK > "HELLO TMP.TMP"
wtouch HELLO.H
del tmp.out
%1 -h -a -f LONG01 -l tmp.out
diff -b LONG01.CMP tmp.out
if errorlevel 1 goto err1
    @echo # LONG01 successful
    goto test2
:err1 
    @echo ## Long FileName ## >> %2
    @echo Error: Long FileName #1 unsuccessful!!! | tee -a %2

:test2
del "HELLO TMP.TMP" 
del HELLO.H

echo # ---------------------------
echo #   Long FileName Test 2
echo # ---------------------------
del tmp.out
%1 -h -ms -a -f LONG02 -l tmp.out
diff -b LONG02.CMP tmp.out
if errorlevel 1 goto err2
    @echo # LONG02 successful
    goto test3
:err2 
    @echo ## Long FileName ## >> %2
    @echo Error: Long FileName #2 unsuccessful!!! | tee -a %2

:test3

goto done
:usage
echo usage: %0 prgname errorfile
:done
if exist tmp.out del tmp.out
