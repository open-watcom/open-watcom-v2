@echo off

if [%1] == [] goto usage

strings - %1 | fgrep -i 'Version Verification'
goto end

:usage
echo usage: %0 filename
echo     Checks 'filename' for occurences of the Version Verification strings
echo     and prints them.

:end
