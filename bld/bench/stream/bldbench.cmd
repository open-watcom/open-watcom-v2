@echo off
if [%cg_bench%] == [] goto error
if [%1] == [] goto error3
if [%2] == [watcom] goto continue
if [%2] == [msvc] goto continue
if [%2] == [] goto continue
goto error2

:continue
cd %cg_bench%
pmake %1 %2 support .not -h build
cd %cg_bench%\stream
echo %1 Bench mark(s) built
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
