@echo off
call clnbench watcom
call initbnch watcom
call bldbench all watcom
call runbench all watcom %CG_BENCH%\stream\watcom.res
