@echo off
wcl386	-zq -l=stub32x hello.c
sc	-bs -q hello
del *.obj
