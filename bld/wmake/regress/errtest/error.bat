@echo off
echo # =============================
echo # Start ERROR
echo # =============================
if .%2 == . goto usage

:test03
echo # -----------------------------
echo # ERROR 03:  Invalid Option 
echo # -----------------------------
del tmp.out
%1 "-." -l tmp.out
egrep Error tmp.out > tmpfile.out
diff tmpfile.out err03a.cmp
if errorlevel 1 goto err03
    echo # Test 03a successful
    
del tmp.out
%1 "\"- \"" -l tmp.out
egrep Error tmp.out > tmpfile.out
diff -b tmpfile.out err03b.cmp
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
del tmp.out
%1 -f -l tmp.out
egrep Error tmp.out > tmpfile.out
diff tmpfile.out err04.cmp
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

del tmp.out
%1 -h "-" -l tmp.out
%1 -h - -l tmp.out
%1 -h "-\" -l tmp.out
diff tmp.out err13.cmp
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
del tmp.out
%1 -h -f ERR17a -l tmp.out
%1 -h -f ERR17b -l tmp.out
%1 -h -f ERR17c -l tmp.out
%1 -h -f ERR17d -l tmp.out
%1 -h -f ERR17e -l tmp.out
%1 -h -f ERR17f -l tmp.out
diff tmp.out err17.cmp
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
del tmp.out
%1 -h -f ERR36 .c.obj -l tmp.out
diff tmp.out err36.cmp
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
wtouch ditty.c
del tmp.out
%1 -h -f ERR39 ditty.obj -l tmp.out
diff tmp.out err39.cmp
if errorlevel 1 goto err39
    echo # Test 39 successful
    goto test40
:err39
    echo ## ERR ## >> %2
    echo # !!! Test 39 unsuccessful !!! |tee -a %2

:test40
@del ditty.*
echo # ------------------------------
echo # ERROR 40: Could not touch target 
echo # ------------------------------ 
del tmp.out
wtouch err40.tst >tmp.out
errout attrib +r err40.tst >>tmp.out
%1 -h -a -t -f ERR40 -l tmp.out
attrib -r err40.tst
del err40.tst
diff tmp.out ERR40.CMP
if errorlevel 1 goto err40
    echo # Test 40 successful
    goto test41
:err40
    echo ## ERR ## >> %2
    echo # !!! Test 40 unsuccessful !!! |tee -a %2

:test41

for %%i in (05 07 10 11 12 15 16 18 19 20 21 22 23 24 25 26 28 29A 29B 29C 30 31A 31B 31C 31D 31E 31F 32 33 34A 34B 37A 37B 38 41 42 43 44A 44B 44C) DO @call work %1 %%i %2 
for %%j in (27 35) DO @call debug %1 %%j %2 

goto done
:usage
echo usage: %0 prgname errorfile
:done
del *.out
del *.tmp
