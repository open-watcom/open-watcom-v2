@echo off
wcl386	-zq -l=dos32x -s -fm=nullptr nullptr.c
sc	-bc -q nullptr
ss	nullptr.exe nullptr.d32 -q

del *.obj
