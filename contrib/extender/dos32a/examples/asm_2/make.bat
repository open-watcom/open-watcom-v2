@echo off
wcl386	-zq -l=stub32x detect.asm
sc	-bs -q detect
del *.obj
