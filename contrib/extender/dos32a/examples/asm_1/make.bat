@echo off
wcl386	-zq -l=stub32x hello.asm
sc	-bs -q hello
del *.obj
