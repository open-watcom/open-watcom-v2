@echo off
SET TRMEM_CODE=1

if not exist ERROR.OUT  goto not_exist 
    del ERROR.OUT
:not_exist
if .%1 == . goto usage

REM ===========================
REM -- test1 - Multiple Dependents Test
REM ===========================
echo *****************************************************************
cd TEST1
call TEST %1 ..\ERROR.OUT
cd ..
    
:tst2
REM ===========================
REM -- TEST2 - Implicit Rules Test
REM ===========================
echo *****************************************************************
cd IMPLICIT
call TEST %1 ..\ERROR.OUT
cd ..
    
:tst3
REM ===========================
REM -- FORTEST - FOR LOOP TEST
REM ===========================
echo *****************************************************************
cd FORTEST
call TEST %1 ..\ERROR.OUT
cd ..

:tst4
REM ===========================
REM -- PRETEST - PRE COMPILER TEST 
REM ===========================
echo *****************************************************************
cd PRETEST
call DOPRE %1 ..\ERROR.OUT
cd ..

:tst5
REM ===========================
REM -- UPDTEST - UPDATE TEST 
REM ===========================
echo *****************************************************************
cd UPDTEST
call DOUPD %1 ..\ERROR.OUT
cd ..

:tst6
set TRMEM_CODE=3
REM ===========================
REM -- ERRTEST - ERROR TEST 
REM ===========================
echo *****************************************************************
cd ERRTEST
call ERROR %1 ..\ERROR.OUT
cd ..

:tst7
set TRMEM_CODE=1
REM ===========================
REM -- INLINE TEST - 
REM ===========================
echo *****************************************************************
cd INLINE
call TEST %1 ..\ERROR.OUT
cd ..

:tst8
REM ===========================
REM -- PREPROCESS TEST - 
REM ===========================
echo *****************************************************************
cd PREPROC
call TEST %1 ..\ERROR.OUT
cd ..

:tst9
REM ===========================
REM -- MACROS TEST - 
REM ===========================
echo *****************************************************************
cd MACROS
call TEST %1 ..\ERROR.OUT
cd ..

:tst10
REM ===========================
REM -- MISC TEST - 
REM ===========================
echo *****************************************************************
cd MISC
call TEST %1 ..\ERROR.OUT
cd ..

:tst11
REM ===========================
REM -- LONG FILENAME TEST - 
REM ===========================
echo *****************************************************************
cd LONGFILE
call TEST %1 ..\ERROR.OUT
cd ..


REM ===========================
REM -- End of Test 
REM ===========================
echo ************* DONE DONE DONE ********************

if exist ERROR.OUT echo !!!!!!!! ERRORS FOUND !!!!!!!!!!
if exist ERROR.OUT echo look at ERROR.OUT for listing 
goto done
:usage
echo %0 <prgname>

:done
