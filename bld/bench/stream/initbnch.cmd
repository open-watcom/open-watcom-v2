@echo off
if [%cg_bench%] == [] goto error
cd %cg_bench%
pmake support %1 -h
cd %cg_bench%\stream
echo Support Library Built
goto done
:error
echo Error:  CG_BENCH environment variable not set
:done
