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
pls_3r
pls_3s
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
set odos4g=%dos4g%
set dos4g=quiet
rsi_3r
rsi_3s
set dos4g=%odos4g%
set odos4g=
which tnt.exe > nul
if errorlevel 1 goto NoTNT
if [%NOTNT%]==[NOTNT] goto NoTNT
tnt wnt_3r
tnt wnt_3s
goto DONE_TNT
:NoTNT
echo ***NOTE: tnt.exe not found, skipping...
:DONE_TNT
x32_3r
x32_3s
win_s
win_m
win_c
win_l
win32_3r
win32_3s
wntw_3r
wntw_3s
cat tmp.log
erase tmp.log>nul
wnt_3r
wnt_3s
goto DONE_WNT
:Usage
echo This file should be run from the directory that contains the test programs.
echo To invoke, type "..\wnt.bat".
:DONE_WNT
