@echo %verbose% off
if .%MSVCDir% == . call vcvars32.bat
if .%MSVCDir% == .               for %%C in (echo false goto:done) do %%C %0: MSVCDir environment variable neither set nor settable
if not .%BENCH_REPORT_FILE% == . for %%C in (echo false goto:done) do %%C %0: BENCH_REPORT_FILE environment variable already set

:: I have slashes rather than backslashes in %MSVCDir% so I can use grep W.Briscoe 2005-03-16
if exist %temp%.\reslash.cmd     for %%C in (echo false goto:done) do %%C %0: %temp%.\reslash.cmd file already exists
echo set MSVCDir=%MSVCDir%| sed -e s:/:\\:g -e "s/ *$//" > %temp%.\reslash.cmd 
for %%C in (call del) do %%C %temp%.\reslash.cmd

set BENCH_REPORT_FILE=%owroot%\bld\bench\msvc.results
pmake -d msvc -h clean
cd support\msvc
wmake -h
cd ..\..
pmake -d msvc -h extra_m_opts=/G5
set BENCH_REPORT_FILE=
:done
