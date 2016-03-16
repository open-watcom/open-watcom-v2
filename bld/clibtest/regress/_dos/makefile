clibtest_name = _dos

!ifndef __UNIX__

.BEFORE
    @if exist tmpfile rm -f tmpfile
    @if exist _tmpfile.* rm -f _tmpfile.*

srcfile = $(clib_dir)/_dos/c/_dostest.c

!include ../mif/master.mif

!endif
