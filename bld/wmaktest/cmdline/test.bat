@rem *************************************************************
@rem A few notes: when running some of these tests, wmake may
@rem exit before the command line is fully processed. Hence the
@rem -l switch may not work and we should use stdout/stderr
@rem redirection to capture output. This is a sensible thing to
@rem do anyway because that way we know processing of the -l
@rem switch will not interfere with the tests in any way.
@rem Also note that -l only logs errors (stderr), not normal
@rem output (stdout). If a test needs to capture both, it has to
@rem use redirection.
@rem *************************************************************

@echo %verbose% off
set ERRLOG=..\error.out
set ERRORS=0
set VER=

if .%1 == . goto usage

echo # ==================================
echo # Command Line Error Tests (cmdline)
echo # ==================================

set TEST=01
set VER= a
head t%TEST% -3
%1 "-." > x%TEST%a.lst 2>&1
egrep Error x%TEST%a.lst > t%TEST%a.lst
diff t%TEST%a.chk t%TEST%a.lst
call :result

set VER= b
%1 "- " > x%TEST%b.lst 2>&1
egrep Error x%TEST%b.lst > t%TEST%b.lst
diff -b t%TEST%b.chk t%TEST%b.lst
call :result
set VER=

set TEST=02
head t%TEST% -3
%1 -h -f > x%TEST%.lst 2>&1
egrep Error x%TEST%.lst > t%TEST%.lst
diff t%TEST%.chk t%TEST%.lst
call :result

set TEST=03
head t%TEST% -3
%1 -h "-" 2> t%TEST%.lst
%1 -h - 2>> t%TEST%.lst
%1 -h "-\" 2>> t%TEST%.lst
diff t%TEST%.chk t%TEST%.lst
call :result

rem if exist *.obj del *.obj
if %ERRORS% == 0 del *.lst
goto end

:usage
    echo usage: %0 prgname errorfile
goto end

:result
    if errorlevel 1 goto resulterr
    @echo #        Test%VER% successful
    goto end
:resulterr
    @echo ## Command Line Error Tests (cmdline) %TEST% ## >> %ERRLOG%
    @echo # Error: Test%VER% unsuccessful!!! | tee -a %ERRLOG%
    set ERRORS=1
:end
