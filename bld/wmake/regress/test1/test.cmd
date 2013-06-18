@echo off
echo # ===========================
echo # Multiple Dependents Test
echo # ===========================
if .%1 == . goto usage

echo # ---------------------------
echo # TEST 1
echo # ---------------------------
wcl386 -l=os2v2 create.c -ox-d1-zq
create 30
rm -f err1.out
wtouch err1.out
%1 -h -f maketst1 -l err1.out > tst1.out
diff -b tst1.out tst1.chk
if errorlevel 1 goto tst1err
diff -b err1.out err1.chk
if errorlevel 1 goto tst1err
    echo # Multiple Dependents Test successful
    goto done
:tst1err
    pause
    echo ## TEST1 ##  >> %2
    echo Error: Multiple Dependents Test Unsuccessful | tee -a %2
:done
    rm *.obj
    rm *.exe
    rm *.out
    rm main.*
    rm foo*.c
    rm maketst1
goto end
:usage
echo usage: %0 prgname errorfile
:end
