.mif : ..;..\..

TARG=qnx
EXE = qnx
!ifndef FORM
FORM = qnx
!endif
INCLUDES=;$(%WATCOM)\qh;$(watcom_dir)\h

!include ui.mif

conesc.gh : ..\c\buildesc.c ..\tix\qnx.tix
        wcl386 /zq ..\c\buildesc/i$(lang_root)\qh
        buildesc ..\tix\qnx.tix conesc.gh
