@echo off
if exist ..\ntaxp\makefile. cd ..\ntaxp
if exist ..\ntaxp\makefile. cd%2 .
if exist ..\ntaxp\makefile. wmake -h %1 %3 %4 %5 %6 %7 %8 %9
