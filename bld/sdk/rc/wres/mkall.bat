@echo off
call mk s %1 %2 %3 %4 %5 %6 %7 %8
if errorlevel 1 goto end
call mk c %1 %2 %3 %4 %5 %6 %7 %8
if errorlevel 1 goto end
call mk m %1 %2 %3 %4 %5 %6 %7 %8
if errorlevel 1 goto end
call mk l %1 %2 %3 %4 %5 %6 %7 %8
if errorlevel 1 goto end
call mk lp %1 %2 %3 %4 %5 %6 %7 %8
if errorlevel 1 goto end
call mk lw %1 %2 %3 %4 %5 %6 %7 %8
if errorlevel 1 goto end
call mk 3r %1 %2 %3 %4 %5 %6 %7 %8
if errorlevel 1 goto end
call mk 3s %1 %2 %3 %4 %5 %6 %7 %8
if errorlevel 1 goto end
call mk nt %1 %2 %3 %4 %5 %6 %7 %8
if errorlevel 1 goto end
call mk qnx32 %1 %2 %3 %4 %5 %6 %7 %8
if errorlevel 1 goto end
:end
