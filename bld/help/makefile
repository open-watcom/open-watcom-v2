name = help
objdir = obj
help_objext = obj

DO_PHARLAP= 1

lnk = $(name).lnk
ccomp = $(comp386)

!ifdef DO_NT
cflags = -bt=nt-mf-zq-w4-d2 -i=$(uidir)\h;$(trmem_dir) -dTRMEM -fo=$(objdir)\$[& /of+
libs =  $(uidir)\nt\uilibf.lib
!endif
!ifdef DO_RSI
cflags = -mf-fpc-zq-w4-d2 -i=$(uidir)\h;$(trmem_dir) -dTRMEM -fo=$(objdir)\$[& /of+
libs = $(uidir)\dos\uilibf.lib
!endif

!ifdef DO_PHARLAP
cflags = -bt=DOS -mf-fpc-oxt-zq-w4-d2 -i=$(uidir)\h;$(trmem_dir) -dTRMEM -fo=$(objdir)\$[& /of+
libs =  $(uidir)\dos\uilibf.lib
!endif

!ifdef DO_DOS
ccomp = $(comp286)
cflags= -zq-ml-fpc-w4-i=$(uidir)\h;$(trmem_dir) -dTRMEM -fo=$(objdir)\$[& /of+
libs =  $(uidir)\dos\uilibl.lib
!endif

!ifndef on_build_machine
cflags += -d2
!endif

.EXTENSIONS:
.EXTENSIONS: .lib
.EXTENSIONS: .obj
.EXTENSIONS: .c .h

!include help.mif

objs =  main.obj trmemcvr.obj trmem.obj filelist.obj
objs += $(help_objs)

$(name).exe : $(objs) $(libs) $(lnk)
        wlink @$(lnk)

!ifdef DO_PHARLAP
        $(pharlap)bind386 $(pharlap)run386b.exe $^*.exp -exe $^@
!endif

$(lnk) : makefile
        %create $(lnk)
        @%append $(lnk) DEBUG ALL
!ifdef DO_PHARLAP
        @%append $(lnk) system pharlap
        @%append $(lnk) runtime maxreal=640k
        @%append $(lnk) op maxdata=1024K
        @%append $(lnk) OP OFFSET=64K
        @%append $(lnk) NAME $(name).exp
!endif
!ifdef DO_NT
        @%append $(lnk) NAME $(name).exe
        @%append $(lnk) SYS nt
!endif
!ifdef DO_DOS
        @%append $(lnk) NAME $(name).exe
!endif
!ifdef DO_RSI
        @%append $(lnk) system dos4g
        @%append $(lnk) NAME $(name).exe
!endif
        @%append $(lnk) OP MAP
        @for %i in ($(libs)) do @%append $(lnk) lib %i
        @for %i in ($(objs)) do @%append $(lnk) file $(objdir)\%i


.obj : $(objdir)
.c : .;..\trmem

.c.obj :
        $(ccomp) $[* $(cflags) $(cflagswin)
