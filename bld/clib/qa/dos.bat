@echo off
dos_s
dos_m
dos_c
dos_l
dos_h
if [%NORUN386%]==[NORUN386] goto NoRUN386
pls_3r
pls_3s
goto DoneRUN386
:NoRUN386
echo ***NOTE: skipping run386 tests...
:DoneRUN386
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
echo ***NOTE: skipping run286 tests...
:DoneRUN286
if [%NODOS4GW%]==[NODOS4GW] goto NoDOS4GW
set odos4g=%dos4g%
set dos4g=quiet
rsi_3r
rsi_3s
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
echo ***NOTE: skipping tnt tests...
:DoneTNT
x32_3r
x32_3s
