name = drwatcom
lnk = $(name).lnk


msgh = rcstr.h
mastermsg = rcstr.msg
msgfiles = drwatcom.msg ..\misc\savelbox.msg ..\misc\memwnd.msg &
           ..\misc\about.msg

.before
        @set wcc=-dNOUSE3D

cflags= -ml-zq-2-s-zu-d1-zW-fpc-w4-i=..\misc;$(wdisasmdir)h;$(dip_dir)\h -fo=obj\.obj

rc = wrc

disasm = $(build_root)\wdisasm\vid\olw

.EXTENSIONS:
.EXTENSIONS: .exe
.EXTENSIONS: .obj
.EXTENSIONS: .asm .c .h .dlg

dip_os = win
!include $(dip_dir)\client.mif

objs =  drwatcom.obj fault.obj globals.obj &
        notify.obj winproc.obj debug.obj dump.obj disasm.obj &
        stack.obj stat.obj log.obj sym.obj listbox.obj&
        profile.obj win32app.obj lddips.obj&
        mem.obj mythelp.obj memwnd.obj memwndcd.obj dlgmod.obj &
        font.obj fontstr.obj mark.obj about.obj ldstr.obj&
        intdata.obj inth.obj savelbox.obj dbg386.obj sdkasm.obj &
        ismod32.obj segmem.obj segmem2.obj getsaddr.obj getcsip.obj &
        $(dip_objs)

$(name).exe : $(msgh) $(objs) $(lnk) $(name).res
        wlinkp @$(lnk)
        $(RC) -k $(name).res

.ERASE
$(msgh) : $(mastermsg)
        vi -i -q "-k :source ..\\\\misc\\\\msgtoh.vi $(msgh)\n" $(mastermsg)
        del obj\*.obj

$(mastermsg) : $(msgfiles)
        $(comp286) -p -i=..\misc drwatcom.msg > $(mastermsg)

sym.exe : dsym.obj
        wlinkp d a sys windows file dsym.obj name sym.exe

dsym.obj : sym.c
        wcc -zw -mc -dDEBUG -w4 -d2 sym.c -fo=dsym.obj -i=$(build_root)\wv\h;..\misc;$(wdisasmdir)h

$(name).res : $(name).rc stackt.dlg stat.dlg intdlg.dlg $(name).ico &
                seginfo.dlg log.dlg dmptask.dlg taskctl.dlg segmap.dlg&
                ..\misc\about.dlg  dbgopt.dlg dipload.dlg $(mastermsg)
    $(RC) -r -i=..\misc $(name).rc

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

.c : .;..\misc;$(dip_dir)\c
.asm : .;..\misc
.dlg : .;..\misc

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
