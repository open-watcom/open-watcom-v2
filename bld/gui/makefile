.EXTENSIONS:
.EXTENSIONS: .pp .exe .lib .obj .res .asm .c .h .msg

!ifndef host_CPU
!ifeqi model f
host_CPU = 386
!else
host_CPU = i86
!endif
!endif

!ifndef host_OS
host_OS=$(os)
!endif

compiler_i86=$(comp286)
compiler_386=$(comp286)
compiler_axp=wccaxp

!ifndef compiler
compiler=compiler_$(host_CPU)
!endif
source_dir = $(guidir)\c
os_source_dir =  $(guidir)\$(type)\c;$(sys_dep_dir)
gui_msg_file = ..\..\h\_guimsgs.h

inc_dirs_lang_qnx = $(lang_root)\qh
inc_dirs_lang_netware = $(lang_root)\novh

!ifdef inc_dirs_lang_$(host_OS)
inc_dirs_lang   = $(inc_dirs_lang_$(host_OS))
!else
inc_dirs_lang   = $(lang_root)\h
!endif

.before
    set $(build_target)_include=$(sysinclude);$(sys_dep_dir);$(watcom_dir)\h;$(guidir)\h;$(guidir)\$(type)\h;$(trmem_dir);$(help_dir);$(plat_inc);$(inc_dirs_lang)

# database team uses fpi87 as default in 6.0
!ifdef wsqlver
!  ifeqi wsqlver 6.0
!    define deffp /fpi87
!  else
!    define deffp /fpc
!  endif
!else
!    define deffp /fpc
!endif

nodebugflags = $(xflags)-zq-w3 -bt=$(build_target)
!ifndef axp
nodebugflags += $(deffp)
!endif
!ifdef model
nodebugflags += -m$model
!endif

!ifdef on_build_machine
cflags = $(nodebugflags) -oxt-d1
track =
!else
!ifdef nodebug
cflags = $(nodebugflags) -oxt-d1
!else
!ifdef codeview
cflags += $(nodebugflags) -d1 -hc
!else
cflags = $(nodebugflags) -d2    # need a space: -bt=foo-d2 eats the -d2. Ugh!
!endif
!endif
!endif

!ifdef wpi_lib
wpi_library=$(wpi_dir)\lib\$(wpi_lib).lib
!endif

librarian_i86=wlib
librarian_386=wlib
librarian_axp=wlib

!ifeq host_CPU axp
cflags += -ai -aq
!endif

compile  = $($(compiler)) $(cflags) $[*

!ifndef track
track=/dTRMEM
!ifndef axp
track += /of+
!endif
!endif

wv_objs = &
       guicreat.obj   &
       guihint.obj    &
       guihook.obj    &
       guimdi.obj     &
       guimdime.obj   &
       guiimdi.obj    &
       guiimmdi.obj   &
       guihflt.obj    &
       guihot.obj     &
       guidraw.obj    &
       guiutil.obj    &
       guipixel.obj   &
       guipick.obj    &
       guiscale.obj   &
       guirscal.obj   &
       guigcolr.obj   &
       guimkey.obj    &
       guizlist.obj   &
       guiextra.obj   &
       guimin.obj     &
       guisetup.obj   &
       guidlg.obj     &
       guindlg.obj    &
       guistyle.obj   &
       guiwnclr.obj   &
       guistr.obj     &
       guiisgui.obj   &
       guideath.obj   &
       guidead.obj    &
       guihtool.obj   &
       guiclrst.obj   &
       guiev.obj      &
       guisdef.obj    &
       guiextnm.obj   &
       guifcrck.obj   &


mem_objs = &
       guimem.obj     &

!ifndef on_build_machine
mem_objs += &
       trmemcvr.obj   &
       trmem.obj
!endif

objs = $(wv_objs) $(mem_objs)

lib_cmdfile = $(guidir)\gui$(os)$(model).clb
here = $(%cdrive):$(%cwd)

tail_i86 = $(model)
tail_386 = $(model)
tail_axp = axp

..\gui$(host_OS)$(tail_$(host_CPU)).lib : eraseall.obj $(gui_msg_file) $(objs) $(extra_objs) $(sysobjs) $(__makefiles__) $(wpi_library) $(ctl3d_lib)
        @%create $(lib_cmdfile)
        @for %i in ($(objs)) do @%append $(lib_cmdfile) +%i
        @for %i in ($(sysobjs)) do @%append $(lib_cmdfile) +%i
!ifdef wpi_lib
        @%append $(lib_cmdfile) +$(wpi_dir)\lib\$(wpi_lib).lib
        @%append $(lib_cmdfile) +wpimem
!endif
!ifdef ctl3d_lib
        @%append $(lib_cmdfile) +$(ctl3d_lib)
!endif
        $(librarian_$(host_CPU)) $(wlib_opt) /n/b/q $^@ @$(lib_cmdfile)
        del $(lib_cmdfile)

eraseall.obj : $(guidir)\$(type)\compile.gbl
        if exist *.obj erase *.obj
        wtouch eraseall.obj


.CONTINUE
.ERASE


.c:     $(source_dir);$(os_source_dir);$(os_extra_dir);$(trmem_dir);$(help_dir)
.h:     $(guidir)\h;$(guidir)\$(type)\h;$(wpi_dir)\h
.msg:   $(guidir)\h

.c.obj:
!ifdef on_build_machine
        $(compile)
!else
        $(compile) -fh
!endif

.c.pp:
        $(compile) -plc -fo=$^&.pp

guisetup.obj    : guisetup.c guiwind.h
guimin.obj      : guimin.c guimin.h
guistr.obj      : guistr.c guistr.h gui.msg


!ifeq model f
wasmopt=-3
!endif
!ifeq model s
wasmopt=-3
!endif

guimem.obj :    guimem.c
           $($(compiler)) $(guidir)\c\guimem.c $(cflags) $(track)

trmemcvr.obj :  trmemcvr.c
           $($(compiler)) $(trmem_dir)\trmemcvr.c $(cflags) $(track)

trmem.obj :     trmem.c
           $($(compiler)) $(trmem_dir)\trmem.c $(cflags) $(track)

$(gui_msg_file): gui.msg mkstr.c
        wcl386 -zq $(guidir)\c\mkstr.c /i$(guidir)\h /lr
        mkstr $(gui_msg_file) $(gui_msg_id_modifier)

clean : .SYMBOLIC
        @-if exist *.?bj @rm *.?bj
        @-if exist *.res @rm *.res
        @-if exist *.err @rm *.err
        @-if exist *.sym @rm *.sym
        @-if exist *.map @rm *.map
        @-if exist *.lnk @rm *.lnk
        @-if exist *.pp  @rm *.pp

