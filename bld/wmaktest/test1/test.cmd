@echo off

set ERRORS=0

echo # ===========================
echo # Multiple Dependents Test
echo # ===========================

if .%1 == . goto usage
set PRG=%1
set ERRLOG=%2

set OWVERBOSE=0
set TEST=01
call :header
%1 -h -f create
echo. >err%TEST%.ref
echo. >err%TEST%.lst
%1 -h -f maketst1 -l err%TEST%.lst > test%TEST%.lst
diff -b err%TEST%.ref err%TEST%.lst
call :result a
diff -b test%TEST%.chk test%TEST%.lst
call :result b

del *.obj
del *.exe
del main.*
del foo*.c

if not %ERRORS% == 0 goto end
    del *.ref
    del *.lst
    del maketst1
goto end

:usage
    echo usage: %0 prgname errorfile
goto end

:header
    echo # ---------------------------
    echo #  For Loop Test %TEST%
    echo # ---------------------------
    goto end

:result
    if errorlevel 1 goto resulterr
    @echo #        Test %1 successful
goto end
:resulterr
    @echo ## FORTEST %TEST% ## >> %ERRLOG%
    @echo # Error: Test %1 unsuccessful!!! | tee -a %ERRLOG%
    set ERRORS=1
:end
