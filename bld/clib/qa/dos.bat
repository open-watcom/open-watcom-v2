@echo off
dos_s
dos_m
dos_c
dos_l
dos_h
if [%TEST_PLS%] == [] goto NoPLS
pls_3r
pls_3s
goto DONE_PLS
:NoPLS
echo ***NOTE: TEST_PLS not set, skipping...
:DONE_PLS
which run286.exe > nul
if errorlevel 1 goto NoRUN286
if [%NORUN286%]==[NORUN286] goto NoRUN286
run286 os2_s
run286 os2_m
run286 os2_c
run286 os2_l
run286 os2_h
run286 os2_mt
goto DoneRUN286
:NoRUN286
echo ***NOTE: run286.exe not found, skipping...
:DoneRUN286
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
tnt.exe -NI 12 wnt_3r
tnt.exe -NI 12 wnt_3s
goto DoneTNT
:NoTNT
echo ***NOTE: tnt.exe not found, skipping...
:DoneTNT
if [%TEST_X32%] == [] goto NoX32
x32_3r
x32_3s
goto DONE_X32
:NoX32
echo ***NOTE: TEST_X32 not set, skipping...
:DONE_X32
