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

echo # =============================
echo # Command Line Error Tests
echo # =============================

set TT=t01
set VER= a
head -n 3 $(%TT)
%1 "-." > $(%TT)a.tmp 2>&1
egrep Error $(%TT)a.tmp > $(%TT)a.lst
diff $(%TT)a.chk $(%TT)a.lst
call :result

set VER= b
%1 "- " > $(%TT)b.tmp 2>&1
egrep Error $(%TT)b.tmp > $(%TT)b.lst
diff -b $(%TT)b.chk $(%TT)b.lst
call :result
set VER=

set TT=t02
head -n 3 $(%TT)
%1 -h -f > $(%TT).tmp 2>&1
egrep Error $(%TT).tmp > $(%TT).lst
diff $(%TT).chk $(%TT).lst
call :result

set TT=t03
head -n 3 $(%TT)
%1 -h "-" 2> $(%TT).lst
%1 -h - 2>> $(%TT).lst
%1 -h "-\" 2>> $(%TT).lst
diff $(%TT).chk $(%TT).lst
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
    @echo ## Test $(%TT:t=) ## >> %ERRLOG%
    @echo # Error: Test%VER% unsuccessful!!! | tee -a %ERRLOG%
    set ERRORS=1
:end
