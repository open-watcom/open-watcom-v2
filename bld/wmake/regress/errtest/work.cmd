@echo off
if .%3 == . goto usage
head ERR%2 -3
%1 -f ERR%2 -h > tmp.out 2>&1
diff tmp.out ERR%2.CMP
if errorlevel 1 goto err
    echo # Test %2 successful
    goto done
:err
    echo ## ERR ## >> %3
    echo # !!! Test %2 unsuccessful !!! | tee -a %3
    goto done
:usage
echo usage: %0 <argument> <argument2>
:done

