@echo off
wcl386	-zq -l=dos32x -fm=except86 except86.c
sc	-bc -q except86
ss	except86 except86 -q
del *.obj
