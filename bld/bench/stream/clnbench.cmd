@echo off
if [%cg_bench%] == [] goto error
cd %cg_bench%
pmake %1 -h clean
cd %cg_bench%\stream
echo %1 Benchmarks cleaned
goto done
:error
echo Error:  CG_BENCH environment variable not set
:done
