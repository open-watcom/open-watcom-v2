@echo off
call clnbench msvc
call initbnch msvc
call bldbench all msvc
call runbench all msvc %CG_BENCH%\stream\msvc.res
