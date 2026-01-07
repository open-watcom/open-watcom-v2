@echo %verbose% off

set ERRORS=0

echo # ===========================
echo # Implicit Rules Tests
echo # ===========================

if .%1 == . goto usage
set WMK=%1
set ERRLOG=..\error.out

echo. >hello.obj
echo. >hello.cpp

set TEST=01
call :header
%WMK% -h -l err%TEST%.lst -f imp%TEST% > test%TEST%.lst
call :result

set TEST=02
call :header
sleep 2
echo. >hello.h
%WMK% -h -c -f imp%TEST% > test%TEST%.lst
diff imp%TEST%.chk test%TEST%.lst
call :result

set TEST=03
call :header
sleep 2
echo. >hello.h
%WMK% -c -h -f imp%TEST% > test%TEST%.lst
diff imp%TEST%.chk test%TEST%.lst
call :result

set TEST=04
call :header
sleep 2
echo. >hello.h
%WMK% -h -c -f imp%TEST% /ms /m > test%TEST%.lst
diff imp%TEST%.chk test%TEST%.lst
call :result

set TEST=05
call :header
del hello.obj
%WMK% -f imp%TEST% -h > tmp%TEST%.lst
sed "s:of .*[\\/]:of :" tmp%TEST%.lst > test%TEST%.lst
diff imp%TEST%.chk test%TEST%.lst
call :result

    :: hello.* hello?.* uses OW and Linux rm compatible wildcards. hello* no go
    del *.obj app.lnk app.exe hello.* hello?.*

rem if exist *.obj del *.obj
if %ERRORS% == 0 del *.lst
goto end

:usage
    echo usage: %0 prgname errorfile
    goto end

:header
    echo # ---------------------------
    echo #  Implicit Rules Test %TEST%
    echo # ---------------------------
    goto end

:result
if errorlevel 1 goto resulterr
    echo #        Test successful
    goto end
:resulterr
    echo ## IMPLICIT RULES TEST %TEST% ## >> %ERRLOG%
    echo # Error: Test unsuccessful!!! | tee -a %ERRLOG%
    set ERRORS=1
:end
