@echo off
wcl386	-zq -l=stub32x -s -4r vesa.c
sc	-bs -q vesa
del *.obj
