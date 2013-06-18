@echo off
echo # ===========================
echo # Start Long FileName Test
echo # ===========================

if .%2 == . goto usage

echo # ---------------------------
echo #   Long FileName Test 1
echo # ---------------------------
echo LONGFILENAME OK > "HELLO TMP.TMP"
wtouch hello.h
rm -f tmp.out
%1 -h -a -f long01 > tmp.out 2>&1
diff -b long01.chk tmp.out
if errorlevel 1 goto err1
    @echo # LONG01 successful
    goto test2
:err1
    @echo ## Long FileName ## >> %2
    @echo Error: Long FileName #1 unsuccessful!!! | tee -a %2

:test2
rm -f "HELLO TMP.TMP"
rm -f hello.h

echo # ---------------------------
echo #   Long FileName Test 2
echo # ---------------------------
rm -f tmp.out
%1 -h -ms -a -f long02 > tmp.out 2>&1
diff -b long02.chk tmp.out
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
rm -f tmp.out
rem This one MUST NOT use -a switch!
%1 -h -ms -f long03 > tmp.out 2>&1
diff -b long03.chk tmp.out
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
rm -f tmp.out
%1 -h -m -f long04 > tmp.out 2>&1
diff -b long04.chk tmp.out
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
rm -f tmp.out
%1 -h -m -f long05 > tmp.out 2>&1
diff -b long05.chk tmp.out
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
rm -f tmp.out
%1 -h -m -f long06 > tmp.out 2>&1
diff -b long06.chk tmp.out
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
rm -f tmp.out
%1 -h -m -f long07 > tmp.out 2>&1
diff -b long07.chk tmp.out
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
rm -f tmp.out
