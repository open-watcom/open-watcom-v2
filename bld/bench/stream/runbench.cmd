@echo off
if [%cg_bench%] == [] goto error
if [%1] == [] goto error3
if [%2] == [watcom] goto continue
if [%2] == [msvc] goto continue
goto error2

:continue
cd %cg_bench%
if [%3] == [] goto continue2
set BENCH_REPORT_FILE=%3
:continue2
pmake %1 %2 support .not -h run
cd %cg_bench%\stream
echo %1 Bench mark(s) executed
set BENCH_REPORT_FILE=
goto done

:error
echo Error:  CG_BENCH environment variable not set
goto done

:error2
echo Error:  Unknown compiler type: %2
goto done

:error3
echo Error:  No bench mark specified, please specify one of the following
type bench.dat
goto done

:done
