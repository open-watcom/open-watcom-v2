@echo off
if not exist wnt*.exe goto Usage
if exist tmp.log erase tmp.log>nul
dos_s
dos_m
dos_c
dos_l
dos_h
os2_s
os2_m
os2_c
os2_l
os2_h
os2_mt
if [%TEST_PLS%] == [] goto NoPLS
pls_3r
pls_3s
goto DONE_PLS
:NoPLS
echo ***NOTE: TEST_PLS not set, skipping...
:DONE_PLS
which run286.exe > nul
if errorlevel 1 goto NoRUN286
run286 os2_s
run286 os2_m
run286 os2_c
run286 os2_l
run286 os2_h
run286 os2_mt
goto DONE_RUN286
:NoRUN286
echo ***NOTE: run286.exe not found, skipping...
:DONE_RUN286
if [%NOCW%]==[NOCW] goto NoCW
cw_3r
cw_3s
goto DoneCW
:NoCW
echo ***NOTE: skipping Causeway tests...
:DoneCW
if [%NODOS4GW%]==[NODOS4GW] goto NoDOS4GW
set odos4g=%dos4g%
set dos4g=quiet
d4g_3r
d4g_3s
set dos4g=%odos4g%
set odos4g=
goto DoneDOS4GW
:NoDOS4GW
echo ***NOTE: skipping dos4gw tests...
:DoneDOS4GW
which tnt.exe > nul
if errorlevel 1 goto NoTNT
if [%NOTNT%]==[NOTNT] goto NoTNT
tnt wnt_3r
tnt wnt_3s
goto DONE_TNT
:NoTNT
echo ***NOTE: tnt.exe not found, skipping...
:DONE_TNT
if [%TEST_X32%] == [] goto NoX32
x32_3r
x32_3s
goto DONE_X32
:NoX32
echo ***NOTE: TEST_X32 not set, skipping...
:DONE_X32
win_s
win_m
win_c
win_l
w386_3r
w386_3s
rem wntw_3r
rem wntw_3s
cat tmp.log
erase tmp.log>nul
wnt_3r
wnt_3s
goto DONE_WNT
:Usage
echo This file should be run from the directory that contains the test programs.
echo To invoke, type "..\wnt.bat".
:DONE_WNT
