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
rm tmp.out
%1 -h -a -f LONG01 > tmp.out 2>&1
diff -b LONG01.CMP tmp.out
if errorlevel 1 goto err1
    @echo # LONG01 successful
    goto test2
:err1
    @echo ## Long FileName ## >> %2
    @echo Error: Long FileName #1 unsuccessful!!! | tee -a %2

:test2
rm "HELLO TMP.TMP"
rm HELLO.H

echo # ---------------------------
echo #   Long FileName Test 2
echo # ---------------------------
rm tmp.out
%1 -h -ms -a -f LONG02 > tmp.out 2>&1
diff -b LONG02.CMP tmp.out
if errorlevel 1 goto err2
    @echo # LONG02 successful
    goto test3
:err2
    @echo ## Long FileName ## >> %2
    @echo Error: Long FileName #2 unsuccessful!!! | tee -a %2

:test3
echo # ---------------------------
echo #   Long FileName Test 3
echo # ---------------------------
rm tmp.out
rem This one MUST NOT use -a switch!
%1 -h -ms -f LONG03 > tmp.out 2>&1
diff -b LONG03.CMP tmp.out
if errorlevel 1 goto err2
    @echo # LONG03 successful
    goto test4
:err2
    @echo ## Long FileName ## >> %2
    @echo Error: Long FileName #3 unsuccessful!!! | tee -a %2

:test4
echo # ---------------------------
echo #   Long FileName Test 4
echo # ---------------------------
rm tmp.out
%1 -h -m -f LONG04 > tmp.out 2>&1
diff -b LONG04.CMP tmp.out
if errorlevel 1 goto err2
    @echo # LONG04 successful
    goto test5
:err2
    @echo ## Long FileName ## >> %2
    @echo Error: Long FileName #4 unsuccessful!!! | tee -a %2

:test5
echo # ---------------------------
echo #   Long FileName Test 5
echo # ---------------------------
rm tmp.out
%1 -h -m -f LONG05 > tmp.out 2>&1
diff -b LONG05.CMP tmp.out
if errorlevel 1 goto err2
    @echo # LONG05 successful
    goto test6
:err2
    @echo ## Long FileName ## >> %2
    @echo Error: Long FileName #5 unsuccessful!!! | tee -a %2

:test6
echo # ---------------------------
echo #   Long FileName Test 6
echo # ---------------------------
rm tmp.out
%1 -h -m -f LONG06 > tmp.out 2>&1
diff -b LONG06.CMP tmp.out
if errorlevel 1 goto err2
    @echo # LONG06 successful
    goto test7
:err2
    @echo ## Long FileName ## >> %2
    @echo Error: Long FileName #6 unsuccessful!!! | tee -a %2

:test7
echo # ---------------------------
echo #   Long FileName Test 7
echo # ---------------------------
rm tmp.out
%1 -h -m -f LONG07 > tmp.out 2>&1
diff -b LONG07.CMP tmp.out
if errorlevel 1 goto err2
    @echo # LONG07 successful
    goto test8
:err2
    @echo ## Long FileName ## >> %2
    @echo Error: Long FileName #7 unsuccessful!!! | tee -a %2

:test8

goto done
:usage
echo usage: %0 prgname errorfile
:done
rm tmp.out
