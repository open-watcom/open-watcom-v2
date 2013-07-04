@echo %verbose% off

rem *************************************************************
rem A few notes: when running some of these tests, wmake may
rem exit before the command line is fully processed. Hence the
rem -l switch may not work and we should use stdout/stderr
rem redirection to capture output. This is a sensible thing to
rem do anyway because that way we know processing of the -l
rem switch will not interfere with the tests in any way.
rem Also note that -l only logs errors (stderr), not normal
rem output (stdout). If a test needs to capture both, it has to
rem use redirection.
rem *************************************************************

echo # =============================
echo # Start ERROR
echo # =============================
if .%2 == . goto usage

:test03
echo # -----------------------------
echo # ERROR 03:  Invalid Option
echo # -----------------------------
%1 "-." > tmp.out 2>&1
egrep Error tmp.out > tmpfile.out
diff tmpfile.out err03a.chk
if errorlevel 1 goto err03
    echo # Test 03a successful

%1 "- " > tmp.out 2>&1
egrep Error tmp.out > tmpfile.out
diff -b tmpfile.out err03b.chk
if errorlevel 1 goto err03
    echo # Test 03b successful
    goto test04

:err03
    echo ## ERR ## >> %2
    echo # !!! Test 03 unsuccessful !!! |tee -a %2

:test04
echo # -----------------------------------------
echo # ERROR 04: -f must be followed by a filename
echo # -----------------------------------------
%1 -f > tmp.out 2>&1
egrep Error tmp.out > tmpfile.out
diff tmpfile.out err04.chk
if errorlevel 1 goto err04
    echo # Test 04 successful
    goto test05
:err04
    echo ## ERR ##  >> %2
    echo # !!! Test 04 unsuccessful !!! |tee -a %2

:test05

echo # ------------------------------------------------
echo # ERROR 13: No Control Characeters valid in Option
echo # ------------------------------------------------

rm tmp.out
%1 -h "-" 2> tmp.out
%1 -h - 2>> tmp.out
%1 -h "-\" 2>> tmp.out
diff tmp.out err13.chk
if errorlevel 1 goto err13
    echo # Test 13 successful
    goto test15
:err13
    echo ## ERR ## >> %2
    echo # !!! Test 13 unsuccessful !!!  | tee -a %2

:test15

:test17
echo # ------------------------------
echo # ERROR 17: Token Too Long
echo # ------------------------------
%1 -h -f err17a >  tmp.out 2>&1
%1 -h -f err17b >> tmp.out 2>&1
%1 -h -f err17c >> tmp.out 2>&1
%1 -h -f err17d >> tmp.out 2>&1
%1 -h -f err17e >> tmp.out 2>&1
%1 -h -f err17f >> tmp.out 2>&1
diff -i tmp.out err17.chk
if errorlevel 1 goto err17
    echo # Test 17 successful
    goto test18
:err17
    echo ## ERR ## >> %2
    echo # !!! Test 17 unsuccessful !!! | tee -a %2

:test18
echo # ------------------------------
echo # ERROR 36: Illegal attempt to update special target
echo # ------------------------------
%1 -h -f err36 .c.obj > tmp.out 2>&1
diff tmp.out err36.chk
if errorlevel 1 goto err36
    echo # Test 36 successful
    goto test37
:err36
    echo ## ERR ## >> %2
    echo # !!! Test 36 unsuccessful !!! | tee -a %2

:test37
echo # ------------------------------
echo # ERROR 39: Target not mentioned in any makefile
echo # ------------------------------
echo. >ditty.c
%1 -h -f err39 ditty.obj > tmp.out 2>&1
diff tmp.out err39.chk
if errorlevel 1 goto err39
    echo # Test 39 successful
    goto test40
:err39
    echo ## ERR ## >> %2
    echo # !!! Test 39 unsuccessful !!! |tee -a %2

:test40
rm ditty.*
echo # ------------------------------
echo # ERROR 40: Could not touch target
echo # ------------------------------
echo. >err40.tst
chmod +r err40.tst >tmp.out
%1 -h -a -t -f err40 >> tmp.out 2>&1
chmod -r err40.tst
rm err40.tst
diff tmp.out err40.chk
if errorlevel 1 goto err40
    echo # Test 40 successful
    goto test41
:err40
    echo ## ERR ## >> %2
    echo # !!! Test 40 unsuccessful !!! |tee -a %2

:test41

for %%i in (05 07 10 11 12 15 16 18 19 20 21 22 23 24 25 26 28 29a 29b 29c 30 31a 31b 31c 31d 31e 31f 32 33 34a 34b 37a 37b 38 41 42 43 44a 44b 44c) DO call work %1 %%i %2
for %%j in (27 35) DO call debug %1 %%j %2

goto done
:usage
echo usage: %0 prgname errorfile
:done
if not .%verbose% == . goto end
rm *.out
rm *.tmp
:end
