#pmake: build os2 lcl 32 support x86
#
# Create WDPMHELP.EXE, WDPMHOOK.DLL
#

.EXTENSIONS
.EXTENSIONS : .dll .lbj .obj .c .asm .res .rc

!ifndef bin_root
!ifdef bin_root_wv
bin_root = $(bin_root_wv)
!endif
!endif

bdir = $(bin_root)\binp

w d : $(bdir)\dll\wdpmhook.dll $(bdir)\wdpmhelp.exe .SYMBOLIC
        @%null

tmplnk = $(%ramdisk)\tmp.lnk

$(bdir)\dll\wdpmhook.dll: pmhook.obj loadstd.obj
    @%write  $(tmplnk) name $(bdir)\dll\wdpmhook.dll sys os2 dll initglobal
    @%append $(tmplnk) debug all op symfile
    @%append $(tmplnk) option dosseg,quiet
    @%append $(tmplnk) option map
    @%append $(tmplnk) file loadstd,pmhook
    @%append $(tmplnk) library $(watcom_dir)\lib\os2.lib
    @%append $(tmplnk) export SendMsgHookProc.1,SetHmqDebugee.2
    @%append $(tmplnk) segment class 'DATA' shared
    @%append $(tmplnk) segment class 'BSS' shared
    $(linker) @$(tmplnk)

$(bdir)\wdpmhelp.exe: pmhelp.lbj pmhelp.res
    @%write  $(tmplnk) name $(bdir)\wdpmhelp.exe system os2 pm
    @%append $(tmplnk) debug all op symfile
    @%append $(tmplnk) file pmhelp.lbj
    @%append $(tmplnk) library clibl.lib
    @%append $(tmplnk) library $(watcom_dir)\lib\os2.lib
    @%append $(tmplnk) option alignment=16
    @%append $(tmplnk) option nodefaultlibs,quiet
    @%append $(tmplnk) option modname=wdpmhelp
    @%append $(tmplnk) option description 'OS/2 Presentation Manager Sample'
    @%append $(tmplnk) option manyautodata
    @%append $(tmplnk) option stack=4096
    @%append $(tmplnk) option heapsize=1024
    @%append $(tmplnk) option protmode
    $(linker) @$(tmplnk)
    $(os2_tools_root)\rc pmhelp.res $(bdir)\wdpmhelp.exe

.c: ..\c

.c.obj:
        $(comp286) $[* -bt=os2 -zq-fpc-d1-oais-s-w3-mc-zu-i$(watcom_dir)\h;$(watcom_dir)\os21x_h;$(watcom_dir)\os220_h;$(trap_dir)\h

.c.lbj
        $(comp286) $[* -bt=os2 -zq-fpc-d1-ml-zu-w3-s-i$(watcom_dir)\os21x_h;$(trap_dir)\h -fo=.lbj

.asm: ..\asm

.asm.obj:
        wasm -zq -2 -bt=os2 $[*

pmlock.obj : pmlock.c ..\h\wdpmhelp.h

#.rc.res:
#       @set oinc=$(%include)
#       set include=$(watcom_dir)\os220_h;$(%include)
#       set opath=$(%path)
#       set path=$(%toolkit)\os2bin
#        $(%toolkit)\os2bin\rc -r $*.rc
#       set path=$(%opath)
#       @set include=$(%oinc)

pmhelp.res: pmhelp.rc ..\h\wdpmhelp.h
        @set oinc=$(%include)
        @set include=$(watcom_dir)\os21x_h;..\h;$(%include)
        @set opath=$(%path)
        @set path=$(os2_tools_root)
        $(os2_tools_root)\rc -r pmhelp.rc
        @set path=$(%opath)
        @set include=$(%oinc)
