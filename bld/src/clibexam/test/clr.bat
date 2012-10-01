@echo off
copy file.txt file.>nul
clog + cl.log
echo Processing *%1*
wcl /cc++ ..\%1 /zq /w9
if [%1]==[time] goto NORUN
%1.exe %2 %3 %4 %5
:NORUN
clog
del %1.obj>nul
del %1.exe>nul
