name = drwatcom
lnk = $(name).lnk
sdk_misc = $(sdk_dir)\misc
common_dir = ..\common


msgh = rcstr.h
mastermsg = rcstr.msg
msgfiles = drwatcom.msg $(sdk_misc)\savelbox.msg $(sdk_misc)\memwnd.msg &
           $(sdk_misc)\about.msg

.before
        @set wcc=-dNOUSE3D
        @set INCLUDE=$(%include);$(common_dir);$(wdisasmdir)h;$(dig_dir)\h

cflags= -ml-zq-2-s-zu-d1-zW-fpc-w4 -i=$(sdk_misc) -fo=obj\.obj


!ifndef on_build_machine
cflags += -d2
!endif

rc = wrc

disasm = $(build_root)\wdisasm\vid\olw

.EXTENSIONS:
.EXTENSIONS: .exe
.EXTENSIONS: .obj
.EXTENSIONS: .asm .c .h .dlg .ico

dig_os = win
!include $(dig_dir)\mif\dipcli.mif

objs =  drwatcom.obj fault.obj globals.obj &
        notify.obj winproc.obj debug.obj dump.obj disasm.obj &
        stack.obj stat.obj log.obj sym.obj listbox.obj&
        profile.obj win32app.obj lddips.obj lognote.obj &
        mem.obj mythelp.obj memwnd.obj memwndcd.obj dlgmod.obj &
        font.obj fontstr.obj mark.obj about.obj ldstr.obj jdlg.obj&
        intdata.obj inth.obj savelbox.obj dbg386.obj sdkasm.obj &
        ismod32.obj segmem.obj segmem2.obj getsaddr.obj getcsip.obj &
        wwinhelp.obj $(dip_objs)

$(name).exe : $(msgh) $(objs) $(lnk) $(name).res
        wlink @$(lnk)
        $(RC) -k $(name).res

.ERASE
$(msgh) : $(mastermsg)
        vi -i -q -s $(sdk_misc)\msgtoh.vi -p "$(msgh)" $(mastermsg)
        del obj\*.obj

$(mastermsg) : $(msgfiles)
        $(comp286) -zk0 -p -i=$(sdk_misc) drwatcom.msg > $(mastermsg)

sym.exe : dsym.obj
        wlinkp d a sys windows file dsym.obj name sym.exe

dsym.obj : sym.c
        wcc -zw -mc -dDEBUG -w4 -d2 sym.c -fo=dsym.obj -i=$(build_root)\wv\h;$(sdk_misc);$(wdisasmdir)h

$(name).res : $(name).rc stackt.dlg ..\common\stat.dlg ..\common\intdlg.dlg &
                ..\common\10drwat.ico ..\common\10err.ico &
                $(sdk_misc)\seginfo.dlg log.dlg &
                dmptask.dlg taskctl.dlg segmap.dlg &
                $(sdk_misc)\about.dlg  dbgopt.dlg &
                ..\common\dipload.dlg $(mastermsg)
    $(RC) -zk -r -i=$(sdk_misc) $(name).rc

$(lnk) : makefile $(disasm)\compile.tim
        %create $(lnk)
!ifndef on_build_machine
        @%append $(lnk) debug all
!endif
        @%append $(lnk) system windows
        @%append $(lnk) name $(name).exe
        @%append $(lnk) library windows.lib
        @%append $(lnk) library toolhelp.lib
        @%append $(lnk) library commdlg.lib
        @%append $(lnk) option map
        @%append $(lnk) option stack=12k
        @%append $(lnk) option heapsize=2k
        @for %i in ($(objs)) do @%append $(lnk) file obj\%i
        @%append $(lnk) path $(disasm) @$(disasm)\o2a.lnk

.obj : obj

.c : .;$(common_dir);$(sdk_misc);$(dig_srcs)
.asm : .;$(sdk_misc)
.dlg : .;$(common_dir);$(sdk_misc)
.ico : .;$(common_dir)

disasm.obj : disasm.c
        $(comp286) $[* -ot-i=$(build_root)\wdisasm\h $(cflags)

sym.obj : sym.c
        $(comp286) $[* -ot-i=$(build_root)\wv\h $(cflags)

mem.obj : mem.c
        $(comp286) $[* $(cflags) -dNO_WPI

.c.obj :
        $(comp286) $[* $(cflags)
.asm.obj:
        masm /Mx /Zd $[*,obj\$[&.obj;
