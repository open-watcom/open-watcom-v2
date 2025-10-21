@echo off
SET TRMEM_CODE=1
set from=%cd%

del error.out
if .%1 == . goto usage

REM ===========================
REM Build utilities first
REM ===========================
cd cmds
wmake -h
cd %from%

REM ===========================
REM -- test1 - Multiple Dependents Test
REM ===========================
echo *****************************************************************
cd TEST1
call test %1 ..\error.out
cd %from%

:tst2
REM ===========================
REM -- TEST2 - Implicit Rules Test
REM ===========================
echo *****************************************************************
cd IMPLICIT
call test %1 ..\error.out
cd %from%

:tst3
REM ===========================
REM -- FORTEST - FOR LOOP TEST
REM ===========================
echo *****************************************************************
cd FORTEST
call test %1 ..\error.out
cd %from%

:tst4
REM ===========================
REM -- PRETEST - PRE COMPILER TEST
REM ===========================
echo *****************************************************************
cd PRETEST
call test %1 ..\error.out
cd %from%

:tst5
REM ===========================
REM -- UPDTEST - UPDATE TEST
REM ===========================
echo *****************************************************************
cd UPDTEST
call test %1 ..\error.out
cd %from%

:tst6
set TRMEM_CODE=3
REM ===========================
REM -- ERRTEST - ERROR TEST
REM ===========================
echo *****************************************************************
cd ERRTEST
call test %1 ..\error.out
cd %from%

:tst7
set TRMEM_CODE=1
REM ===========================
REM -- INLINE TEST -
REM ===========================
echo *****************************************************************
cd INLINE
call test %1 ..\error.out
cd %from%

:tst8
REM ===========================
REM -- PREPROCESS TEST -
REM ===========================
echo *****************************************************************
cd PREPROC
call test %1 ..\error.out
cd %from%

:tst9
REM ===========================
REM -- MACROS TEST -
REM ===========================
echo *****************************************************************
cd MACROS
call test %1 ..\error.out
cd %from%

:tst10
REM ===========================
REM -- MISC TEST -;
REM ===========================
echo *****************************************************************
cd MISC
call test %1 ..\error.out
cd %from%

:tst11
REM ===========================
REM -- LONG FILENAME TEST -
REM ===========================
echo *****************************************************************
cd LONGFILE
call test %1 ..\error.out
cd %from%


REM ===========================
REM -- End of Test
REM ===========================
echo ************* DONE DONE DONE ********************

if exist error.out echo !!!!!!!! ERRORS FOUND !!!!!!!!!!
if exist error.out echo look at error.out for listing
goto done
:usage
echo "%0 <prgname>"

:done
cd %from%
