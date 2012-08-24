.EXTENSIONS:
.EXTENSIONS: .pp .exe .lib .obj .res .asm .c .h

LNK=sample.lnk
source_dir = $(gui_dir)\sample\c
name = sample
!ifeq type win
rcopt += -i=$(gui_dir)\$(type);$(sdk_misc_dir)
!else
rcopt += -i=$(gui_dir)\$(type);$(sdk_misc_dir);$(gui_dir)\win
!endif

.before
    @set include=$(sysinclude);$(gui_dir)\h;$(gui_dir)\$(type)\h;$(%include);$(lang_root)\h

# database team uses fpi87 as default in 6.0
!ifdef wsqlver
!  ifeqi wsqlver 6.0
deffp = /fpi87
!  else
deffp = /fpc
!  endif
!else
deffp = /fpc
!endif

commonflags = -w3-zq$(xflags) -bt=$(build_target)
!ifndef axp
commonflags += $(deffp)-zc
!endif
!ifdef model
commonflags += -m$(model)
!endif
!ifdef nodebug
cflags = $(commonflags) -oail -s
!else
cflags = $(commonflags) -d2
!endif

!ifeq sys nlm
cflags += /4s/zq/s$(deffp)
!endif

compile = $($(compiler)) $(cflags) $[*
ppcompile = $($(compiler)) $(cflags) -pl -pc $[* > $^&.pp

objs =                  &
        dlgstat.obj     &
        dlgdynam.obj    &
        dlgtest.obj     &
        sampdef.obj     &
        sample.obj

!ifndef ext
ext = exe
!endif

!ifeq type ui
uilib = $(ui_dir)\$(os)\uilib$(model).lib
guilib = $(gui_dir)\$(type)\gui$(os)$(model).lib
!else
uilib =
guilib = $(gui_dir)\$(type)\gui$(os)$(model).lib
!endif


!ifeq type win
!ifneq os pm
rcname=$(name)
!else
rcname=$(name)pm
!endif
!else
rcname=$(name)
!endif
sysdep = $(rcname).res

!ifeq sys nlm

$(LNK): $(__makefiles__)
        @%append $(LNK) debug novell
        @%append $(LNK) name sample
        @%append $(LNK) module clib
        @%append $(LNK) format novell 'sample nlm'
        @%append $(LNK) libpath %WATCOM%\lib386
        @%append $(LNK) libpath %WATCOM%\lib386\netware
        @%append $(LNK) import @%WATCOM%\novi\clib.imp
        @%append $(LNK) import @$(ui_dir)\$(os)\uinlm.imp
        set lib=$(watcomlib);$(%lib)

!else

$(LNK): $(__makefiles__)
        @%write $(LNK) sys $(sys)
!ifndef axp
        @%append $(LNK) debug all
!else
        @%append $(LNK) debug codeview
!endif
        @%append $(LNK) option map $(extra_option)
        @%append $(LNK) name sample
!ifneq os pm
        @%append $(LNK) option stack=16k
!else
        @%append $(LNK) option stack=16k
        @%append $(LNK) option heapsize=4k
!endif
!ifeq type win
        @%append $(LNK) option symfile=$(name).sym
!endif
!ifdef watcomlib
        @%append $(LNK) libpath $(watcomlib)
!endif
        @%append $(LNK) libpath $(sdk_misc_dir)
        @%append $(LNK) library $(guilib)
!ifeq type ui
        @%append $(LNK) library $(uilib)
!endif
!ifdef extra_lib
        @%append $(LNK) library $(extra_lib)
!endif
!ifdef wreslib
        @%append $(LNK) library $(wreslib)
!endif
        @for %i in ($(objs)) do @%append $(LNK) file %i
!ifeq type ui
        @%append $(LNK) file $(guilib)(uialloc)
        @%append $(LNK) file $(guilib)(uibarf)
        @%append $(LNK) file $(guilib)(uidchfld)
!ifeq sys windows
        @%append $(LNK) import DEATH GDI.121 import RESURRECTION GDI.122
!endif
!endif

!endif

###############
sample.$(ext) : $(objs) $(guilib) $(uilib) $(sysdep) $(LNK)
        $(linker) @$(LNK)
!ifeq type win
!ifeq os w386
        wbind $* -s $(%watcom)\binw\win386.ext -R -k $(rcopt) $(sysdep)
!else ifeq os pm
        $(lang_root)\binp\rc -p -x $(rcname).res $(name).exe
!else
        $(rc) $(rcopt) -k $(sysdep)
!endif
!ifneq os w386
        copy /b $(name).exe+$(name).sym
!endif
!else
!ifeq sys pharlap
        !$(pharlap)bind386 $(pharlap)run386b sample
!endif
!ifeq ext qnx
        $(wstrip) -q -a -r sample.qnx . $(rcname).res
!else
        $(wstrip) -q -a -r sample.exe . $(rcname).res
!endif
!endif

################
pmsample.$(ext) : pmsample.obj $(guilib) $(uilib) $(LNK)
        @%append $(LNK) file pmsample.obj
        $(linker) @$(LNK)

################
samp2.$(ext) : samp2.obj $(guilib) $(uilib) $(LNK)
        @%append $(LNK) file samp2.obj
        $(linker) @$(LNK)

.CONTINUE
.ERASE

.c:     $(source_dir);$(os_source_dir);$(os_extra_dir)
.h:     ..\h

.c.obj:
        $(compile)

.c.pp:
        $(ppcompile)

$(rcname).res : ../$(rcname).rc ../$(rcname).dlg
!ifeq os pm
        @set oinc=$(%include)
        #set include=$(watcom_dir)\os220_h;$(%include)
        set include=$(watcom_os220_h);$(%include)
        copy $(sdk_misc_dir)\closebmp.bmp
        wcc -p -i..;..\h;$(gui_dir)\$(type) ..\$(rcname).rc -fo=tmp.rc
        @%write $(rcname).rc $#include <os2.h>
        copy $(rcname).rc+tmp.rc
        del tmp.rc
        set opath=$(%path)
        set path=$(lang_root)\binp
        #@$(lang_root)\binp\rc -r $(rcname).rc
        rc -r $(rcname).rc
        del $(rcname).rc
        set path=$(%opath)
        @set include=$(%oinc)
        del closebmp.bmp
!else
        $(rc) $(rcopt) -r $[@ -fo=$^@
#        copy ..\$(rcname).res
!endif

sample.obj: sample.c sample.h dlgstat.h dlgdynam.h
sampdef.obj: sampdef.c sample.h
dlgtest.obj: dlgtest.c dlgtest.h dlgstat.h sample.h
dlgdynam.obj: dlgdynam.c dlgdynam.h dlgstat.h sample.h
dlgstat.obj: dlgstat.c dlgstat.h sample.h

clean : .SYMBOLIC
        @-if exist *.?bj @rm *.?bj
        @-if exist *.res @rm *.res
        @-if exist *.err @rm *.err
        @-if exist *.sym @rm *.sym
        @-if exist *.map @rm *.map
        @-if exist *.lnk @rm *.lnk
        @-if exist *.pp  @rm *.pp

