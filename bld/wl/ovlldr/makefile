#pmake: all os_dos cpu_i86 intel

c_opts = /r/d1/zu/oaxt/w3/zq/zl/nc=_OVLCODE/nt=_TEXT/nm=$^&/fo=$^&
m_opts = /s/mx/t/ZD/i$(watcom_h)
lib_opts = /b/q

.extensions:
.extensions: .obj .asm .c

.c: $(wldir)ovlldr
.asm: $(wldir)ovlldr

bothobjs = $(s)ovlret.obj $(s)ovlldr.obj $(s)ovlmain.obj $(s)ovldbg.obj
s=s
sobjs = $+ $(bothobjs) $-
s=l
lobjs = $+ $(bothobjs) $-

regobjs = ovlio.obj ovlboth.obj novlldr.obj novlmain.obj ovldata.obj &
          ovlstd.obj novldbg.obj naddarea.obj ngrablow.obj ovlshare.obj

mtobjs  = ovlio.obj ovlboth.obj movlldr.obj movlmain.obj ovldata.obj &
          movldbg.obj naddarea.obj

allobjs = $(lobjs) $(sobjs) $(regobjs)

debugobjs = dovlboth.obj dovldata.obj

.asm.obj :
    $(asm286) $(m_opts) $[@,$^@;

.c.obj:
    $(comp286) $[*$(c_opts)/ml

wovl : .symbolic wovl.lib wmovl.lib
    @%null

all : .symbolic wovl.lib dovl.lib dovls.lib dovll.lib dovln.lib
    @%null

wovl.lib : $(allobjs)
    %create tmp.lbc
    @for %i in ($?) do %append tmp.lbc -+%i
!ifdef on_build_machine
    wlib $(lib_opts)/s/t wovl @tmp.lbc
!else
    wlib $(lib_opts) wovl @tmp.lbc
!endif

wmovl.lib : $(mtobjs)
    %create tmp.lbc
    @for %i in ($?) do %append tmp.lbc -+%i
!ifdef on_build_machine
    wlib $(lib_opts)/s/t wmovl @tmp.lbc
!else
    wlib $(lib_opts) wmovl @tmp.lbc
!endif

dovl.lib : $(allobjs)
    %create tmp.lbc
    @for %i in ($?) do %append tmp.lbc -+%i
    wlib $(lib_opts) dovl @tmp.lbc

dovls.lib : $(debugobjs) dovlldr.obj sovlmain.obj sovldbg.obj
    %create tmp.lbc
    @for %i in ($?) do %append tmp.lbc -+%i
    wlib $(lib_opts) dovls @tmp.lbc

dovll.lib : $(debugobjs) xovlldr.obj lovlmain.obj lovldbg.obj
    %create tmp.lbc
    @for %i in ($?) do %append tmp.lbc -+%i
    wlib $(lib_opts) dovll @tmp.lbc

dovln.lib : $(debugobjs) qovlldr.obj novlmain.obj novldbg.obj &
         naddarea.obj ngrablow.obj
    %create tmp.lbc
    @for %i in ($?) do %append tmp.lbc -+%i
    wlib $(lib_opts) dovln @tmp.lbc

dovlldr.obj : ovlldr.c
    $(comp286) $[*$(c_opts)/mc/dOVL_DEBUG/dOVL_SMALL

sovlldr.obj : ovlldr.c
    $(comp286) $[*$(c_opts)/mc/dOVL_SMALL

xovlldr.obj : ovlldr.c
    $(comp286) $[*$(c_opts)/ml/dOVL_DEBUG

lovlldr.obj : ovlldr.c
    $(comp286) $[*$(c_opts)/ml

qovlldr.obj : novlldr.c
    $(comp286) $[*$(c_opts)/ml/dOVL_DEBUG

dovlboth.obj : ovlboth.c
    $(comp286) $[*$(c_opts)/ml/dOVL_DEBUG

sovldbg.obj : ovldbg.c
    $(comp286) $[*$(c_opts)/mc/dOVL_SMALL

lovldbg.obj : ovldbg.c
    $(comp286) $[*$(c_opts)/ml

novldbg.obj : ovldbg.c
    $(comp286) $[*$(c_opts)/ml/dOVL_WHOOSH

movldbg.obj : ovldbg.c
    $(comp286) $[*$(c_opts)/ml/dOVL_WHOOSH/dOVL_MULTITHREAD

sovlmain.obj : sovlmain.asm ovlmain.asm
    $(asm286) $(m_opts) /DOVL_SMALL=1 $[&;

lovlmain.obj : lovlmain.asm ovlmain.asm
    $(asm286) $(m_opts) $[&;

sovlret.obj : sovlret.asm ovlret.asm
    $(asm286) $(m_opts) /DOVL_SMALL=1 $[&;

lovlret.obj : lovlret.asm ovlret.asm
    $(asm286) $(m_opts) $[&;

dovldata.obj : ovldata.asm
    $(asm286) $(m_opts) /DOVL_DEBUG=1 $[&,$^&;

clean
    rm -f *.obj tmp.lbc *.lib
