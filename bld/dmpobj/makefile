template = &
dmpobj.$(ext) &
process.$(ext) &
objread.$(ext) &
output.$(ext) &
wildargv.$(ext)

!include cproj.mif
!include deftarg.mif

.EXTENSIONS:
.EXTENSIONS: .obj .nbj .abj .c .mif

ext = obj
objs = $+$(template)$-

ext = nbj
ntobjs = $+$(template)$-

ext = abj
axpobjs = $+$(template)$-

host_CPU = 386

.c: .;$(lang_root)\src\startup
.c.obj:
    $(cc_i86) $[@ /oaxt/d1/zq/w4/bt=dos /i=$(watcom_dir)\h

.c.nbj:
    $(cc_386) $[@ /oaxt/d1/zq/w4/bt=nt/fo=.nbj /i=$(watcom_dir)\h

.c.abj:
    $(cc_axp) $[@ /oaxt/d1/zq/w4/bt=nt/fo=.abj /i=$(watcom_dir)\h

!ifndef linker
linker = wlink
!endif

all : dmpobj.exe dmpobjnt.exe .SYMBOLIC
        @%null

dmpobj.exe : $(objs)
    *$(linker) name dmpobjst.exe system dos opt quiet file {$(objs)}
    *$(linker) debug all system os2 opt quiet, heap=20k, stub=dmpobjst.exe file {$(objs)}
    #$(binder)\bind dmpobj.exe $(apilib) $(doscalls)

dmpobjnt.exe : $(ntobjs)
    *$(linker) name dmpobjnt.exe system nt opt quiet file {$(ntobjs)}

dmpobjax.exe : $(axpobjs)
    *$(linker) name dmpobjax.exe system ntaxp opt quiet file {$(axpobjs)}

install : dmpobj.exe .symbolic
    wstrip dmpobj.exe r:\cmds\dmpobj.exe >nul

