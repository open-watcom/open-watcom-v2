@echo off
if .%3 == . goto usage
head err%2 -3
%1 -f err%2 -h -d > tmp.out 2>&1
egrep W%2 tmp.out > tmpfile.tmp
diff -i tmpfile.tmp err%2.chk
if errorlevel 1 goto err
    echo # Test %2 successful
    goto done
:err
    echo ## ERR ## >> %3
    echo # !!! Test %2 unsuccessful !!! |tee -a %3
    goto done
:usage
echo usage: %0 <argument> <argument2>
:done

