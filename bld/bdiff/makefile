.ERASE

wres16=$(watcom_dir)\lib\wresl
wres32=$(watcom_dir)\lib\wresf

targs = bpatch.exe bpatch.qnx bdiff.exe bdiff2.exe bpcmt.exe bplevel.exe wpatch.exe

all : .symbolic $(targs)
    @%null

.EXTENSIONS:
.EXTENSIONS: .exe .exp .qnx .xbj .obj .dbj .qbj .wbj .pbj .c .h

.BEFORE
!ifdef on_build_machine
    @set debug=
        @set d1=d1
!else
        @set debug=debug all
        @set d1=d2
!endif

.c: $(build_root)\bdiff;$(watcom_dir)\c

.c.obj:
        $(comp286) $[* /ml /ox /$(%d1) /w4 /zq

.c.xbj:
        $(comp286) $[* /ml /ox /$(%d1) /w4 /dBDUMP /fo=.xbj /zq

.c.dbj:
        $(comp386) $[* /ox /$(%d1) /dBDIFF /fo=.dbj /w4 /zq

#.c.pbj:
#        $(comp286) $[* /ml /ox /$(%d1) /d_WPATCH /fo=.pbj /w4 /zq

.c.pbj:
        $(comp386) $[* /ox /$(%d1) /d_WPATCH /fo=.pbj /w4 /zq

.c.wbj:
        $(comp386) $[* /ox /$(%d1) /dBDIFF /d_WPATCH /fo=.wbj /w4 /zq

.c.qbj:
        $(comp286) $[* /ml /ox /$(%d1) /fo=.qbj /w4 /zq /d__QNX__ /i$(lang_root)\qh

#
# WPATCH/WCPATCH
#
WBJS = dopatch.wbj msg.wbj myio.wbj wcptchio.wbj wcpatch.wbj &
bdiffmem.wbj wbdiff.wbj
wcpatch.exe : $(WBJS) makefile
        if exist tmp.lnk erase tmp.lnk
        @for %i in ($(WBJS)) do @%append tmp.lnk file %i
        wlink name wcpatch $(%debug) lib $(wres32) @tmp
        erase tmp.lnk

PBJS = dopatch.pbj msg.pbj myio.pbj oldfile.pbj wpatchio.pbj wpatch.pbj &
bdiffmem.pbj bpatch.pbj
wpatch.exe : $(PBJS) makefile
        if exist tmp.lnk erase tmp.lnk
        @for %i in ($(PBJS)) do @%append tmp.lnk file %i
        wlink name wpatch $(%debug) lib $(wres32) @tmp
        erase tmp.lnk
#        wlink name wpatch sys dos $(%debug) lib $(wres16) @tmp
#
# BPATCH
#
OBJ = bpatch.obj dopatch.obj msg.obj myio.obj oldfile.obj patchio.obj patmain.obj bdiffmem.obj
QNX_OBJ = bpatch.qbj dopatch.qbj msg.qbj myio.qbj oldfile.qbj patchio.qbj patmain.qbj bdiffmem.obj

!ifdef on_build_machine
bpatch.osz :
        wlink @tmp
        objsize @map bpatch.map >bpatch.osz
        objpad bpatch.osz
!endif

bpatch.exe : obpatch.res $(OBJ) makefile
        @%create tmp.lnk
    @%append tmp.lnk system os2 pmc
        @%append tmp.lnk $(%debug) option map,newfile,heap=8k,stack=8k
        @%append tmp.lnk lib $(wres16)
        @for %i in ($(OBJ)) do @%append tmp.lnk file %i
!ifdef on_build_machine
        @%make bpatch.osz
        objsetsz bpatch.osz
!endif
        wlink @tmp
        erase tmp.lnk
        @set path=$(%path);$(binder)
        $(binder)\bind bpatch.exe $(doscalls) $(apilib) -o bpatch.exe -n DosLockSeg DosUnlockSeg
        wstrip /q /a /r bpatch.exe . obpatch.res

bpatch.qnx : qbpatch.res $(QNX_OBJ) makefile
    @%make usage.u
        @%create tmp.lnk
        @%append tmp.lnk $(%debug) system qnx option map=bpatchq,res=usage.u
        @%append tmp.lnk lib $(wres16)
        @for %i in ($(QNX_OBJ)) do @%append tmp.lnk file %i
    @%append tmp.lnk name bpatch
!ifdef on_build_machine
        @%make bpatch.osz
        objsetsz bpatch.osz
!endif
        wlink @tmp
        erase tmp.lnk
        wstrip /q /a /r bpatch.qnx . qbpatch.res

#
# BDUMP
#
XBJ = bdump.obj dopatch.xbj msg.obj myio.obj oldfile.obj patchio.obj patmain.obj bdiffmem.obj

bdump.exe : obpatch.res $(XBJ) makefile
        if exist tmp.lnk erase tmp.lnk
        @for %i in ($(XBJ)) do @%append tmp.lnk file %i
        wlink $(%debug) lib $(wres16) @tmp
        erase tmp.lnk
        wstrip /q /a /r bdump.exe . obpatch.res

#
# BDIFF
#
dbj = bdiff.dbj bdiffmem.dbj dopatch.dbj msg.dbj myio.dbj  symtab.dbj

bdiff.exe bdiff2.exe : $(dbj) makefile bdiff.res
        if exist tmp.lnk erase tmp.lnk
        @for %i in ($(dbj)) do @%append tmp.lnk file %i
    @%append tmp.lnk option caseexact lib $(wres32)
        wlink name bdiff2.exe $(%debug) system os2v2 @tmp
        wstrip /q /a /r bdiff2.exe . bdiff.res
        wlink $(%debug) system pharlap @tmp
        erase tmp.lnk
        wstrip /q /a /r bdiff.exp . bdiff.res
        !$(pharlap)bind386 $(pharlap)run386b bdiff.exp

#
# BPCMT
#
bpcmt.exe : bpcmt.c bdiffmem.c
        $(comp286) bpcmt /oaxt /$(%d1) /ml
        wlink $(%debug) system os2 file bpcmt, bdiffmem
        @set path=$(%path);$(binder)
        bind bpcmt.exe $(apilib) $(doscalls)

#
# BPLEVEL
#
bplevel.exe : bplevel.c
        $(comp286) bplevel /oaxt /$(%d1)
        wlink $(%debug) system os2 file bplevel
        @set path=$(%path);$(binder)
        bind bplevel.exe $(apilib) $(doscalls)


obpatch.res : bpatch.rc bpatch.h opusage.rc opusagej.rc
        wrc /r /dBPATCH /fo=$^. bpatch.rc

qbpatch.res : bpatch.rc bpatch.h qpusage.rc
        wrc /r /dBPATCH /dQNX /fo=$^. bpatch.rc

opusage.rc : usage.sp
    wsplice -kBPATCH -kENGLISH -t8 -f "%+MSG_USE_E_BASE-1+%$#, \"%s\""usage.sp opusage.rc

opusagej.rc : usage.sp
    wsplice -kBPATCH -kJAPANESE -t8 -f "%+MSG_USE_J_BASE-1+%$#, \"%s\""usage.sp opusagej.rc

qpusage.rc : usage.sp
    wsplice -kBPATCH -kENGLISH -kQNX -t8 -f "%+MSG_USE_E_BASE-1+%$#, \"%s\""usage.sp qpusage.rc

dusage.rc : usage.sp
    wsplice -kBDIFF -kENGLISH -k$(_OS) -t8 -f "%+MSG_USAGE_LN_%$#, \"%s\""usage.sp dusage.rc

usage.u : usage.sp
    wsplice -kBPATCH -kENGLISH -kQNX -t8 -u usage.sp usage.u

bdiff.res : bpatch.rc dusage.rc bpatch.h
        wrc /r /dBDIFF /fo=$^. bpatch.rc

clean : .symbolic
    @rm -f *.?bj *.lnk *.map *.err *.exe *.exp *.qnx *.qbj *.res *.rc
    @rm -f usage.rc usagej.rc usage.u usagej.u msg.h usage.h tmp.msg
    @rm -f pusage.rc pusagej.rc qpusage.rc qpusagej.rc dusage.rc bpatch.osz
