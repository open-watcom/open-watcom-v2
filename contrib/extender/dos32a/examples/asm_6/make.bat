@echo off
wcl386	-zq -l=stub32x lfb.asm
sc	-bs -q lfb
del *.obj
