#pmake:all clibqa _dos
.BEFORE
    @if exist tmpfile rm -f tmpfile > nul
    @if exist _tmpfile.* rm -f _tmpfile.* > nul
srcfile = $(clib_dir)\_dos\c\_dostest.c
!include ..\mif\master.mif
