@echo off
wcl386	-zq -l=stub32x dbgshow.asm
wcl386	-zq -l=stub32x excshow.asm
wcl386	-zq -l=stub32x intshow.asm
wcl386	-zq -l=stub32x simshow.asm
sc	-bs -q dbgshow
sc	-bs -q excshow
sc	-bs -q intshow
sc	-bs -q simshow

del *.obj
