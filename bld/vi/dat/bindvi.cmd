@echo off
if [%1] == [] goto usage
set __exe__=%WATCOM%\binp\vi.exe
if [%2] == [] goto skipexe
set __exe__=%2
:skipexe
if not exist %1 goto usage
if not exist %__exe__% goto usage
echo %1 > edbind.dat
type skel.dat >> edbind.dat
edbind %__exe__%
goto done
:usage
@echo Usage: bindvi [config file] [exe]
:done
