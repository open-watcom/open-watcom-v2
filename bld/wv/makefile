#
# WATCOM Debugger makefile
#

!ifndef bin_root
!ifdef bin_root_wv
bin_root = $(bin_root_wv)
!endif
!endif

#For the debugger we must use dos4g 1.96 because it does not work with
#dos4g 1.97, probably due to some optimizations made in 1.97
dos4g_ver = dos4g196

cdir   = $(wv_dir)\c
hdir   = $(wv_dir)\h

common_h= $(lang_root)\h;$(watcom_dir)\h
os2inc  = $(common_h)
dosinc  = $(common_h)
ntinc   = $(watcom_dir)\nt_h;$(common_h)
wininc  = $(lang_root)\h\win;$(common_h)
qnxinc  = $(lang_root)\qh;$(watcom_dir)\h

!ifeq host_OS nt
sysdir = win
!else
sysdir = $(host_OS)
!endif


!ifdef machine
all_machine=$(machine)
!else
all_machine=at
!endif

bdir_win_i86_at         = $(bin_root)\bin
bdir_win_i86_nec        = $(bin_root)\bin.nec
bdir_dos_386_at         = $(bin_root)\bin
bdir_dos_386_nec        = $(bin_root)\bin.nec
bdir_nt_386_at          = $(bin_root)\binnt
bdir_os2_i86_at         = $(bin_root)\binp
bdir_os2_386_at         = $(bin_root)\binp
bdir_qnx_i86_at         = $(bin_root)\qnx
bdir_qnx_386_at         = $(bin_root)\qnx
bdir_nt_axp_at          = $(bin_root)\axpnt

bdir = $(bdir_$(host_OS)_$(host_CPU)_$(all_machine))

!ifndef syscdir
syscdir = ..\c
!endif
!ifndef syshdir
syshdir = ..\h
!endif
!ifndef sysadir
sysadir = ..\asm
!endif

!ifndef extra_cflags
extra_cflags =
!endif

compiler_i86    =$(comp286) -ml -fpc
compiler_386    =$(comp386) -mf -fpc
compiler_axp    =wccaxp

resname_win_gui         = wvwin
resname_nt_gui          = wvnt
resname_os2_gui         = wvpm

!ifdef resname_$(host_OS)_$(display)
resname = $(resname_$(host_OS)_$(display))
!else
resname = wvui
!endif
!ifdef dll
resname=$+$(resname)$-dll
!endif
resfile = $(resname).res

!ifdef codeview
debug_all=debug codeview
symfile=
!else ifdef wvdbg
debug_all=debug watcom
!else
debug_all=debug all
!endif

exe_qnx = qnx

!ifdef exe_$(host_OS)
EXE = $(exe_$(host_OS))
!else
EXE = exe
!endif

tail_char_win   = c
tail_gui        = w

!ifdef tail_$(display)_$(host_OS)
tail_exe = $(tail_$(display)_$(host_OS))
!else ifdef tail_$(display)
tail_exe = $(tail_$(display))
!else
tail_exe =
!endif

d : $(bdir)\dv$(tail_exe).$(EXE) .SYMBOLIC
    @%null

w : $(bdir)\wv$(tail_exe).$(EXE) .SYMBOLIC
    @%null

lib : wd.lib .SYMBOLIC
    @%null

cflags_at = -D_AT_PC
cflags_nec = -D_NEC_PC
cflags_gui = -D__GUI__
cflags_char = -D__CHAR__

cflags = -bt=$(host_OS)-zq-d1-oxt-s-w3 $(cflags_$(all_machine)) $(cflags_$(display))
!ifndef on_build_machine
!ifndef nodebug
!ifdef codeview
cflags +=-d1 -hc -dAUI_DBG -dDBG_DBG
!else
cflags +=-d2 -dAUI_DBG -dDBG_DBG
!endif
!endif
!ifdef trmem
cflags +=-dTRMEM
!endif
!endif

cflags += $(extra_cflags)

.BEFORE
    @set ipaths=-i$(dig_dir)\h
    @set ipaths=$(%ipaths);$(syshdir)
    @set ipaths=$(%ipaths);$(hdir)
    @set ipaths=$(%ipaths);$(trmem_dir)
!ifndef dll
    @set ipaths=$(%ipaths);$(aui_dir)\h
    @set ipaths=$(%ipaths);$(gui_dir)\h
    @set ipaths=$(%ipaths);$(ui_dir)\h
!endif
    @set ipaths=$(%ipaths);$($(host_OS)inc)
!ifndef dll
    @set ipaths=$(%ipaths);$(posix_dir)\h
    @set ipaths=$(%ipaths);$(gui_dir)\ui\h
    @set ipaths=$(%ipaths);$(help_dir)
!endif

compile  = *$(compiler_$(host_CPU)) $[* $(cflags) @ipaths
assemble = wasm $(extra_aflags) $[@ -fo=$^@

dig_os_dos_386  = dsx

!ifdef dig_os_$(host_OS)_$(host_CPU)
dig_os = $(dig_os_$(host_OS)_$(host_CPU))
!else
dig_os = $(host_OS)
!endif

!include $(dig_dir)/mif/dipcli.mif
!include $(dig_dir)/mif/madcli.mif
!include $(dig_dir)/mif/trpcli.mif

engine_objs = &
        $(dip_objs) &
        $(mad_objs) &
        $(trap_objs) &
        dumb.obj &
        spawn.obj &
        dbgchain.obj &
        dbgvar.obj &
        dbgvset.obj &
        dbgass.obj &
        dbgimg.obj &
        dbglist.obj &
        dbgpend.obj &
        ambigsym.obj &
        addarith.obj &
        dbgbrk.obj &
        dbgcall.obj &
        dbgcall2.obj &
        dbgcmd.obj &
        dbgcmdln.obj &
        dbgdot.obj &
        dbgexec.obj &
        dbgexpr.obj &
        dbgexpr2.obj &
        dbgexpr3.obj &
        dbgexpr4.obj &
        dbgfile.obj &
        dbghook.obj &
        dbginvk.obj &
        dbglkup.obj &
        dbgloc.obj &
        dbglog.obj &
        dbgmain.obj &
        dbgmem.obj &
        memtypes.obj &
        dbgmisc.obj &
        dbgovl.obj &
        dbgparse.obj &
        dbgprint.obj &
        dbgreg.obj &
        dbgscan.obj &
        dbgshow.obj &
        dbgsrc.obj &
        dbgstdio.obj &
        dbgsys.obj &
        dbgutil.obj &
        dbg_dbg.obj &
        dbgprog.obj &
        strutil.obj &
        stub.obj &
        dbgwalk.obj &
        dynmem.obj &
        dbgsem.obj &
        dbgtrace.obj &
        dbgexins.obj &
        dbgexdat.obj &
        dbgmodfy.obj &
        dbgtback.obj &
        dbgerr.obj &
        namelist.obj &
        modlist.obj &
        sortlist.obj &
        remmisc.obj &
        remcore.obj &
        remfinfo.obj &
        remenv.obj &
        remfile.obj &
        remthrd.obj &
        removl.obj &
        ldsupp.obj &
        doserr.obj &
        diginter.obj &
        dipinter.obj &
        madinter.obj &
        dbglit.obj &
        dbginsty.obj &
        dbgdata.obj &
        dbgevent.obj &
        dbgdll.obj &
        dlgscan.obj &
        dbgset.obj &
        srcmgt.obj &
        dbgupdt.obj &
        symcomp.obj &
        litengin.obj

ui_objs = &
        litwd.obj &
        dui.obj &
        dbgwset.obj &
        dbginsp.obj &
        dbgicon.obj &
        dbgdisp.obj &
        dbgpaint.obj &
        dbgcap.obj &
        wndalloc.obj &
        dlgambig.obj &
        dlgscomp.obj &
        modcomp.obj &
        wndsys.obj &
        wnddump.obj &
        dbgintrn.obj &
        dlgabout.obj &
        dlgstk.obj &
        dlgayn.obj &
        dlgsrc.obj &
        dlgdll.obj &
        dlglist.obj &
        dlgfile.obj &
        dlgnewp.obj &
        dlgexpr.obj &
        dlgbreak.obj &
        dlgvarx.obj &
        dlgcmd.obj &
        dlgnewws.obj &
        wndhelp.obj &
        wndmenu.obj &
        dbgwfpu.obj &
        dbgwmac.obj &
        dbgwglob.obj &
        dbgwfunc.obj &
        dbgwcall.obj &
        dbgwmod.obj &
        dbgwsrch.obj &
        dbgwdlg.obj &
        dbgwfil.obj &
        dbgwass.obj &
        dbgwimg.obj &
        dbgwbrk.obj &
        dbgwio.obj &
        dbgwrep.obj &
        dbgwmem.obj &
        dbgwregs.obj &
        dbgwmmx.obj &
        dbgwmadr.obj &
        dbgwtogl.obj &
        dbgwvar.obj &
        dbgwthrd.obj &
        dlgwind.obj &
        dbgwdata.obj &
        dlgoptn.obj &
        dbgtool.obj &
        dbgfing.obj

!ifndef on_build_machine
engine_objs += &
        trmem.obj
!endif


libs_win_i86_char       = $(aui_dir)\win\aui.lib $(gui_dir)\ui\guiwinl.lib $(ui_dir)\$(machine)win\uilibl.lib $(wres_dir)\lib\wreslw.lib
libs_win_i86_gui        = $(aui_dir)\win\aui.lib $(gui_dir)\win\guiwinl.lib
libs_nt_386_char        = $(aui_dir)\nt\aui.lib $(gui_dir)\ui\guintf.lib $(ui_dir)\nt\uilibf.lib $(wres_dir)\lib\wresf.lib
libs_nt_386_gui         = $(aui_dir)\nt\aui.lib $(gui_dir)\win\guintf.lib
libs_os2_i86_char       = $(aui_dir)\ol\aui.lib $(gui_dir)\ui\guios2l.lib $(ui_dir)\os2\uilibl.lib $(wres_dir)\lib\wresl.lib
libs_os2_i86_gui        = $(aui_dir)\pm\aui.lib $(gui_dir)\win\guipml.lib $(watcom_dir)\lib\os2.lib
libs_os2_386_char       = $(aui_dir)\of\aui.lib $(gui_dir)\ui\guios2f.lib $(ui_dir)\os2\uilibf.lib $(wres_dir)\lib\wresf.lib
libs_os2_386_gui        = $(aui_dir)\pmf\aui.lib $(gui_dir)\win\guipmf.lib
libs_dos_386_char       = $(aui_dir)\of\aui.lib $(gui_dir)\ui\guidosf.lib $(ui_dir)\$(machine)dos\uilibf.lib $(wres_dir)\lib\wresf.lib
libs_qnx_i86_char       = $(aui_dir)\ol\aui.lib $(gui_dir)\ui\guiqnxl.lib $(ui_dir)\qnx\uilibl.lib $(wres_dir)\lib\wresl.lib
libs_qnx_386_char       = $(aui_dir)\o32s\aui.lib $(gui_dir)\ui\guiqnxs.lib $(ui_dir)\qnx\uilibs.lib $(wres_dir)\lib\wressqnx.lib
libs_nt_axp_char        = $(aui_dir)\ntaxp\aui.lib $(gui_dir)\ui\guintaxp.lib $(uidir)\nt\uilibaxp.lib $(wres_dir)\lib\wresaxp.lib
libs_nt_axp_gui         = $(aui_dir)\ntaxp\aui.lib $(gui_dir)\win\guintaxp.lib

libs = $(libs_$(host_OS)_$(host_CPU)_$(display))

objs_win_i86_char       = helpio.obj
objs_win_i86_gui        =
objs_nt_386_char        = helpio.obj
objs_nt_386_gui         =
objs_os2_i86_char       = helpio.obj
objs_os2_i86_gui        =
objs_dos_386_char       = helpio.obj
objs_qnx_i86_char       = helpio.obj
objs_qnx_386_char       = helpio.obj
objs_nt_axp_char        = helpio.obj
objs_nt_axp_gui         =

ui_objs += $(objs_$(host_OS)_$(host_CPU)_$(display))

!ifdef ovl_dos
endsect=end
!endif

objs = $(engine_objs) $(ui_objs)

regexp.obj : $(posix_dir)\misc\regexp.c .autodepend
        $(compile) -fi=wndregx.h

lnk = wv.lnk
lnk2 = wv2.lnk

$(bdir)\dv$(tail_exe).$(EXE) : $(resfile) eraseall.obj $(objs) $(sysobjs) $(libs) $(lnk)
        @set DEBUG=$(debug_all)
        @set NAME=$^&
        @set RESFILE=$+ $(resfile) $-
        @%make do_link

wd.lib : eraseall.obj $(engine_objs) $(sysobjs)
        if exist tmp.lbc del tmp.lbc
        @for %i in ($(engine_objs) $(sysobjs)) do @%append tmp.lbc +%i
        wlib /b /n wd.lib @tmp.lbc
        del tmp.lbc

$(bdir)\wv$(tail_exe).$(EXE) : $(resfile) eraseall.obj $(objs) $(sysobjs) $(libs) $(lnk)
!ifndef on_build_machine
        @set DEBUG=debug line
!else
        @set DEBUG=$(debug_all)
!endif
        @set NAME=$^&
        @set RESFILE=$+ $(resfile) $-
        @%make do_link

eraseall.obj : $(wv_dir)\compile.gbl
        if exist *.obj erase *.obj
        wtouch eraseall.obj

do_link : .PROCEDURE
        %make finger_msg
!ifdef on_build_machine
!ifndef host_CPU axp
        @%make all.osz
        !objsetsz all.osz
!endif
!endif
        $(linker) op symfile=$(bdir)\$(%NAME).sym $(%DEBUG) name $(bdir)\$(%NAME).$(EXE) @$(lnk) $(endsect)
        @%make post_process

all.osz :
        $(linker) name $(bdir)\$(%NAME).$(EXE) @$(lnk) $(endsect)
        objsize @map $(%NAME).map > all.osz
        objpad all.osz

$(lnk) : $(__MAKEFILES__)
    %create $(lnk)
    @%make syslnk
    @%append $(lnk) option map,nocache
!ifdef syslibs
    @for %i in ($(syslibs)) do @%append $(lnk) library %i
!endif
    @for %i in ($(libs))    do @%append $(lnk) library %i
    @for %i in ($(sysobjs)) do @%append $(lnk) file %i
    @%append $(lnk) file fingmsg
!ifeq host_OS dos
!ifdef ovl_dos
    @%append $(lnk) begin autosection
!endif
!endif
    @for %i in ($(objs))    do @%append $(lnk) file %i

rcinclude=.;$(hdir);$(dig_dir)\h;$(aui_dir)\h;$(gui_dir);$(gui_dir)\h;$(lang_root)\h

post_process :.PROCEDURE
!ifdef codeview
        !cvpack $(bdir)\$(%NAME).sym
!endif
!ifeq host_OS dos
!ifeq host_CPU 386
!ifdef on_build_machine
        @ren $(bdir)\$(%NAME).$(EXE) x.exe
        @copy $(rsi_root)\bin\$(dos4g_ver).exe d4g.exe
        @$(rsi_root)\bin\expmod d4g.exe pmem=384 xmax=384
        $(rsi_root)\bin\4gbind d4g.exe $(bdir)\x.exe $(bdir)\$(%NAME).$(EXE) -V
        @del d4g.exe
        @del $(bdir)\x.exe
!else ifdef really_ovl_dos
        copy dv.map $(lnk2)
        vi -d -q -s ..\mapfix.vi $(lnk2)
        $(linker) $(symfile) $(%DEBUG) name $(bdir)\$(%NAME).$(EXE) @$(lnk) @$(lnk2) $(endsect)
        @%null
!endif
!endif
!else ifeq host_OS qnx
!ifdef %QNX_KEY
        serialize $(bdir)\$(%NAME).$(EXE) $(%QNX_KEY)
!endif
!endif
!ifeq display char
        wstrip /q /a /r  $(bdir)\$(%NAME).$(EXE) . $(%RESFILE)
!else ifeq host_OS win
        *wrc -k -bt=windows -i=$(rcinclude);$(lang_root)\h\win $(%RESFILE) $(bdir)\$(%NAME).$(EXE)
!else ifeq host_OS nt
        *wrc -k -bt=nt -i=$(rcinclude);$(lang_root)\h\nt $(%RESFILE) $(bdir)\$(%NAME).$(EXE)
!else ifeq host_OS os2
        $(lang_root)\binp\rc -p -x $(%RESFILE) $(bdir)\$(%NAME).$(EXE)
!endif

sortlist.obj : $(watcom_dir)\c\sortlist.c .autodepend
        $(compile)

comp_tracker_i86        = $(compile) -of+
comp_tracker_386        = $(compile) -of+
comp_tracker_axp        = $(compile)

trmem.obj : $(trmem_dir)\trmem.c .autodepend
        $(comp_tracker_$(host_CPU))

srcmgt.obj : $(watcom_dir)\c\srcmgt.c $(hdir)\srcmgt.h  .autodepend
        $(compile)

finger_msg  :.PROCEDURE
       @set DGINFO=
!ifdef on_build_machine
!ifeq host_os dos
!ifeq host_CPU 386
        @copy $(rsi_root)\bin\$(dos4g_ver).exe dos4g.exe
        -@dos4g $(rsi_root)\bin\dginfo >dginfo.gh
        @set DGINFO=-fidginfo.gh
        @del dos4g.exe
!endif
!endif
!endif
        @$(compiler_$(host_CPU)) ..\..\c\fingmsg $(cflags) $(%DGINFO) @ipaths

bitmaps = &
        $(wvdir)\win\zapant.ico &
        $(wvdir)\os2\zapant.ico

!ifeq display gui
bitmaps += &
        $(wv_dir)\win\8087.ico &
        $(wv_dir)\os2\8087.ico &
        $(wv_dir)\win\asm.ico &
        $(wv_dir)\os2\asm.ico &
        $(wv_dir)\win\brk.ico &
        $(wv_dir)\os2\brk.ico &
        $(wv_dir)\win\call.ico &
        $(wv_dir)\os2\call.ico &
        $(wv_dir)\win\exe.ico &
        $(wv_dir)\os2\exe.ico &
        $(wv_dir)\win\func.ico &
        $(wv_dir)\os2\func.ico &
        $(wv_dir)\win\glob.ico &
        $(wv_dir)\os2\glob.ico &
        $(wv_dir)\win\hot.ico &
        $(wv_dir)\os2\hot.ico &
        $(wv_dir)\win\io.ico &
        $(wv_dir)\os2\io.ico &
        $(wv_dir)\win\log.ico &
        $(wv_dir)\os2\log.ico &
        $(wv_dir)\win\mem.ico &
        $(wv_dir)\os2\mem.ico &
        $(wv_dir)\win\mod.ico &
        $(wv_dir)\os2\mod.ico &
        $(wv_dir)\win\reg.ico &
        $(wv_dir)\os2\reg.ico &
        $(wv_dir)\win\repl.ico &
        $(wv_dir)\os2\repl.ico &
        $(wv_dir)\win\src.ico &
        $(wv_dir)\os2\src.ico &
        $(wv_dir)\win\srch.ico &
        $(wv_dir)\os2\srch.ico &
        $(wv_dir)\win\thrd.ico &
        $(wv_dir)\os2\thrd.ico &
        $(wv_dir)\win\var.ico &
        $(wv_dir)\os2\var.ico &
        $(wv_dir)\win\splash.bmp &
        $(wv_dir)\win\closebmp.bmp &
        $(wv_dir)\win\read1.bmp &
        $(wv_dir)\win\read2.bmp &
        $(wv_dir)\win\write1.bmp &
        $(wv_dir)\win\write2.bmp &
        $(wv_dir)\win\open1.bmp &
        $(wv_dir)\win\open2.bmp &
        $(wv_dir)\win\close1.bmp &
        $(wv_dir)\win\close2.bmp &
        $(wv_dir)\win\points1.bmp &
        $(wv_dir)\win\points2.bmp &
        $(wv_dir)\win\codeher1.bmp &
        $(wv_dir)\win\codeher2.bmp &
        $(wv_dir)\win\brkhere1.bmp &
        $(wv_dir)\win\brkhere2.bmp &
        $(wv_dir)\win\dimbrk1.bmp &
        $(wv_dir)\win\dimbrk2.bmp &
        $(wv_dir)\win\source1.bmp &
        $(wv_dir)\win\source2.bmp &
        $(wv_dir)\win\source2.bmp &
        $(wv_dir)\win\source1.bmp &
        $(wv_dir)\win\assembl1.bmp &
        $(wv_dir)\win\assembl2.bmp &
        $(wv_dir)\win\assembl2.bmp &
        $(wv_dir)\win\assembl1.bmp &
        $(wv_dir)\win\currlin1.bmp &
        $(wv_dir)\win\currlin2.bmp &
        $(wv_dir)\win\unpoint1.bmp &
        $(wv_dir)\win\unpoint2.bmp &
        $(wv_dir)\win\currbrk1.bmp &
        $(wv_dir)\win\currbrk2.bmp &
        $(wv_dir)\win\currdim1.bmp &
        $(wv_dir)\win\currdim2.bmp &
        $(wv_dir)\win\go.bmp &
        $(wv_dir)\win\over.bmp &
        $(wv_dir)\win\into.bmp &
        $(wv_dir)\win\return.bmp &
        $(wv_dir)\win\back.bmp &
        $(wv_dir)\win\for.bmp &
        $(wv_dir)\win\up.bmp &
        $(wv_dir)\win\down.bmp &
        $(wv_dir)\win\home.bmp
!endif


litdef = $(hdir)\litdef.gh

rcfiles = &
    $(gui_dir)\h\gui.rc &
    $(gui_dir)\h\gui.msg &
    $(aui_dir)\h\aui.rc &
    $(aui_dir)\h\dlgrx.dlg &
    $(aui_dir)\h\dlgsrch.dlg &
    $(aui_dir)\h\dlgsrcha.dlg &
    $(aui_dir)\h\japrx.dlg &
    $(aui_dir)\h\japsrch.dlg &
    $(aui_dir)\h\japsrcha.dlg &
    $(hdir)\dialogs.rc &
    $(hdir)\strings.rc &
    $(litdef) &
    $(hdir)\dlgamb.h &
    $(hdir)\dlgayn.h &
    $(hdir)\dlgstk.h &
    $(hdir)\dlgbrk.h &
    $(hdir)\dlgcmd.h &
    $(hdir)\dlglist.h &
    $(hdir)\dlgnewp.h &
    $(hdir)\dlgoptn.h &
    $(hdir)\dlgvarx.h &
    $(hdir)\dlgwind.h &
    $(hdir)\dlgstk.h &
    dlgayn.dlg &
    dlgbrk.dlg &
    dlgcmd.dlg &
    dlglist.dlg &
    dlgnewp.dlg &
    dlgoptn.dlg &
    dlgvarx.dlg &
    dlgamb.dlg &
    dlgwind.dlg &
    dlgstk.dlg &
    dlghis.dlg &
    japayn.dlg &
    japbrk.dlg &
    japcmd.dlg &
    japlist.dlg &
    japnewp.dlg &
    japoptn.dlg &
    japvarx.dlg &
    japamb.dlg &
    japwind.dlg &
    japstk.dlg &
    japhis.dlg

$(litdef) : wd.str wdengin.str menu.str $(cdir)\mkstr.c $(dig_dir)\h\mad.str
        wcl386 $(cdir)\mkstr -zq -i$(hdir);$(dig_dir)\h
        set DOS4G=quiet
        mkstr $(litdef)
        del mkstr.obj
        del mkstr.exe

wvui.res : $(hdir)\wvui.rc $(rcfiles) .autodepend
        *wrc -r -q -bt=windows -i=$(rcinclude);$(lang_root)\h\win $(hdir)\wvui.rc /fo=wvui.res

wvntdll.res : $(hdir)\strings.rc .autodepend
        *wrc -r -q -bt=nt -i=$(rcinclude);$(lang_root)\h\win $(hdir)\strings.rc /fo=wvntdll.res

wvwin.res : $(hdir)\wvwin.rc $(bitmaps) $(rcfiles) .autodepend
        *wrc -k -r -bt=windows -i=$(rcinclude);$(lang_root)\h\win $(hdir)\wvwin.rc /fo=wvwin.res

wvnt.res : $(hdir)\wvwin.rc $(bitmaps) $(rcfiles) .autodepend
        *wrc -k -r -bt=nt -i=$(rcinclude);$(lang_root)\h\nt $(hdir)\wvwin.rc /fo=wvnt.res

wvpm.res wvpmj.res : $(hdir)\wvpm.rc $(bitmaps) $(rcfiles) .autodepend
        @set oinc=$(%include)
        set include=$(watcom_dir)\os220_h;$(rcinclude);$(%include)
        wcc -dRC -i$(aui_dir)\pm;$(rcinclude) -p -pw=0 -fo=wvpm.i $(hdir)\wvpm.rc
        wcc -dRC -i$(aui_dir)\pm;$(rcinclude) -p -pw=0 -fo=wvpmj.i $(hdir)\wvpm.rc /dJAPANESE_MESSAGES
        set opath=$(%path)
        set path=$(lang_root)\binp
        $(lang_root)\binp\rc -r foros2pm.rc
        del wvpm.i
        copy foros2pm.res wvpm.res
        copy wvpmj.i wvpm.i
        $(lang_root)\binp\rc -r foros2pm.rc
        copy foros2pm.res wvpmj.res
        set path=$(%opath)
        @set include=$(%oinc)

.CONTINUE
# .ERASE

.EXTENSIONS
.EXTENSIONS : .exe .def .cpp .obj .i .h .c .asm .dlg .str

.str: $(hdir)

.h: $(syshdir);$(hdir);$(auidir)\h

.c: $(syscdir);$(cdir);$(auidir)\c;$(dig_srcs)

.c.obj: .autodepend
        $(compile)

.c.i:
        $(compile) /plc /fo=.i

.c.def:
        $(compile) /v /e1000


.asm: $(sysadir)
.asm.obj:
        $(assemble)

litengin.obj :: litengin.c wdengin.str $(dig_dir)\h\mad.str $(litdef)
litwd.obj :: litwd.c wd.str $(dig_dir)\h\mad.str $(litdef)
dbgwthrd.obj :: dbgwthrd.c menutrd.h menudef.h
dbgwass.obj :: dbgwass.c menuasm.h masmopt.h masmshow.h menudef.h gadgets.h
dbgwimg.obj :: dbgwimg.c menuimg.h menudef.h
dbgwmac.obj :: dbgwmac.c menumac.h wndnames.h menudef.h gadgets.h
dbgwbrk.obj :: dbgwbrk.c menubrk.h  menudef.h gadgets.h
dbgwcall.obj :: dbgwcall.c menucall.h  menudef.h gadgets.h
dbgwfil.obj :: dbgwfil.c menufile.h mfilshow.h menudef.h gadgets.h
dbgw87.obj :: dbgw87.c menufpu.h  menudef.h
dbgwglob.obj :: dbgwglob.c menuglob.h  namelist.h menudef.h
dbgwfunc.obj :: dbgwfunc.c menufunc.h namelist.h menudef.h gadgets.h
dbgwmod.obj :: dbgwmod.c menumod.h  menudef.h gadgets.h
dbgwregs.obj :: dbgwregs.c menureg.h menudef.h
dbgwvar.obj :: dbgwvar.c menuvar.h mvartype.h mvaropt.h mvarshow.h menudef.h dbgvar.h gadgets.h
dbgvar.obj :: dbgvar.c dbgvar.h
dbgwmem.obj :: dbgwmem.c menumem.h menudef.h
dbgwio.obj :: dbgwio.c menuio.h menumem.h menudef.h gadgets.h
wndmenu.obj :: wndmenu.c mmbreak.h mmconfig.h mmcode.h mmfile.h mmwind.h &
                       mmhelp.h mmrun.h mmsearch.h mmdat.h mmundo.h &
                       wndnames.h menudef.h dbgmenu.h
dbgdata.obj :: dbgdata.c wndnames.h dbginp.h dbgtoggl.h
dbgdisp.obj :: dbgdisp.c menumem.h wndnames.h dbginp.h
dbginsp.obj :: dbginsp.c wndnames.h menumem.h
dbgicon.obj :: dbgicon.c dbgicon.h
dbgexdat.obj :: dbgexdat.c menumem.h menumem.h
wndsys.obj :: wndsys.c wndnames.h gadgets.h dbginp.h menudef.h
dbgpaint.obj :: dbgpaint.c wndnames.h
dsxtrap.obj :: $(syscdir)\dsxtrap.c $(hdir)\dbgaccmx.h
namelist.obj :: namelist.c namelist.h
dbgbrk.obj :: dbgbrk.c namelist.h dbginp.h
symcomp.obj :: symcomp.c namelist.h
dbgmain.obj :: dbgmain.c dbgcmd.h dbginp.h
dbgwsrch.obj :: dbgwsrch.c menusrch.h menudef.h gadgets.h
dbgexec.obj :: dbgexec.c dbginp.h
dbgtrace.obj :: dbgtrace.c dbginp.h
dbginvk.obj :: dbginvk.c dbginp.h
dbgerr.obj :: dbgerr.c dbgerr.h dbginp.h
dlgcmd.obj :: dlgcmd.c dbginp.h
dbgcap.obj :: dbgcap.c dbginp.h
dlgfile.obj :: dlgfile.c dbginp.h
dbgutil.obj :: dbgutil.c dbginp.h
dbghook.obj :: dbghook.c dbginp.h
dbgfing.obj :: dbgfing.c dbgname.h gadgets.h # aui.h
dbgacc1.obj :: dbgacc1.c dbgaccmx.h
dbgacc2.obj :: dbgacc2.c dbgaccmx.h
dbgwrep.obj :: dbgwrep.c menurep.h menudef.h
dbgwdlg.obj :: dbgwdlg.c menudef.h
wnddump.obj :: wnddump.c gadgets.h
dbglit.obj :: dbglit.c dbglit.h literals.h $(hdir)\wd.str $(dig_dir)\h\mad.str
madinter.obj :: madinter.c $(dig_dir)\h\mad.str

!ifeq display char
.dlg : $(hdir)
!else ifneq host_OS os2
.dlg : $(hdir)
!else
pmfont=-font=
pmfontj=-font=
dlgayn.dlg : $(hdir)\dlgayn.dlg
        parsedlg $(pmfont) $(hdir)\dlgayn.dlg dlgayn.dlg
dlgbrk.dlg : $(hdir)\dlgbrk.dlg
        parsedlg $(pmfont) $(hdir)\dlgbrk.dlg dlgbrk.dlg
dlgcmd.dlg : $(hdir)\dlgcmd.dlg
        parsedlg $(pmfont) $(hdir)\dlgcmd.dlg dlgcmd.dlg
dlglist.dlg : $(hdir)\dlglist.dlg
        parsedlg $(pmfont) $(hdir)\dlglist.dlg dlglist.dlg
dlgnewp.dlg : $(hdir)\dlgnewp.dlg
        parsedlg $(pmfont) $(hdir)\dlgnewp.dlg dlgnewp.dlg
dlgoptn.dlg : $(hdir)\dlgoptn.dlg
        parsedlg $(pmfont) $(hdir)\dlgoptn.dlg dlgoptn.dlg
dlgvarx.dlg : $(hdir)\dlgvarx.dlg
        parsedlg $(pmfont) $(hdir)\dlgvarx.dlg dlgvarx.dlg
dlgamb.dlg : $(hdir)\dlgamb.dlg
        parsedlg $(pmfont) $(hdir)\dlgamb.dlg dlgamb.dlg
dlgwind.dlg : $(hdir)\dlgwind.dlg
        parsedlg $(pmfont) $(hdir)\dlgwind.dlg dlgwind.dlg
dlgstk.dlg : $(hdir)\dlgstk.dlg
        parsedlg $(pmfont) $(hdir)\dlgstk.dlg dlgstk.dlg
dlghis.dlg : $(hdir)\dlghis.dlg
        parsedlg $(pmfont) $(hdir)\dlghis.dlg dlghis.dlg
japayn.dlg : $(hdir)\japayn.dlg
        parsedlg $(pmfontj) $(hdir)\japayn.dlg japayn.dlg
japbrk.dlg : $(hdir)\japbrk.dlg
        parsedlg $(pmfontj) $(hdir)\japbrk.dlg japbrk.dlg
japcmd.dlg : $(hdir)\japcmd.dlg
        parsedlg $(pmfontj) $(hdir)\japcmd.dlg japcmd.dlg
japlist.dlg : $(hdir)\japlist.dlg
        parsedlg $(pmfontj) $(hdir)\japlist.dlg japlist.dlg
japnewp.dlg : $(hdir)\japnewp.dlg
        parsedlg $(pmfontj) $(hdir)\japnewp.dlg japnewp.dlg
japoptn.dlg : $(hdir)\japoptn.dlg
        parsedlg $(pmfontj) $(hdir)\japoptn.dlg japoptn.dlg
japvarx.dlg : $(hdir)\japvarx.dlg
        parsedlg $(pmfontj) $(hdir)\japvarx.dlg japvarx.dlg
japamb.dlg : $(hdir)\japamb.dlg
        parsedlg $(pmfontj) $(hdir)\japamb.dlg japamb.dlg
japwind.dlg : $(hdir)\japwind.dlg
        parsedlg $(pmfontj) $(hdir)\japwind.dlg japwind.dlg
japstk.dlg : $(hdir)\japstk.dlg
        parsedlg $(pmfontj) $(hdir)\japstk.dlg japstk.dlg
japhis.dlg : $(hdir)\japhis.dlg
        parsedlg $(pmfontj) $(hdir)\japhis.dlg japhis.dlg
!endif
