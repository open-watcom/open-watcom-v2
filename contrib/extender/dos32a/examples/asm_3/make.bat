@echo off
wcl386	-zq -l=stub32x int1ch.asm
sc	-bs -q int1ch
del *.obj
