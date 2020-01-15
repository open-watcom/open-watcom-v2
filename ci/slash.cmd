@echo off
setlocal ENABLEDELAYEDEXPANSION
for /f "tokens=6" %%f in ('7z l -ba %1') do (
  set X=%%f
  echo %%f !X:\=/!
)
setlocal DISABLEDELAYEDEXPANSION
