@echo off
wcl386	-zq -l=stub32xc tsr.asm
ss tsr tsr -q

del *.obj
