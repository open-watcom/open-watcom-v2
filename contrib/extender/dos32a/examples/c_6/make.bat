@echo off
wcl386	-zq -l=stub32x -s -5r spawn.c
wcl386	-zq -l=stub32x -s -5r prog1.c
wcl386	-zq -l=stub32x -s -5r prog2.c
sc	-bs -q spawn
sc	-bs -q prog1
sc	-bs -de -q prog2

del *.obj
