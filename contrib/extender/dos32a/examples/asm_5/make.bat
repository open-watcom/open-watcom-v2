@echo off
wcl386	-zq -l=stub32x memerr.asm
sc	-bs -q memerr
del *.obj
