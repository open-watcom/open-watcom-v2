@echo off
wcl386	-zq -l=stub32x -s -4r d32alib.cpp
sc	-bs -q d32alib
del *.obj
