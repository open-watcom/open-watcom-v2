name = drnt
lnk = $(name).lnk

msgh = rcstr.h
mastermsg = rcstr.msg
msgfiles = drnt.msg ..\..\misc\about.msg ..\..\misc\savelbox.msg &
           ..\..\misc\memwnd.msg

cflags= -zq-oaxt-s-fpc-w4 -mf-4-bt=nt-fo=obj\.obj

!ifndef on_build_machine
cflags += -d2
# cflags += -dDEBUG
!endif

rc = wrcp
link = $(linker)
cc = $(comp286)
cc386 = $(comp386)

disasm = $(build_root)\wdisasm\vid\ofw

.BEFORE
        set include=..;..\..\misc;$(wdisasmdir)h;$(watcom_nt_h);$(dip_dir)\h;$(%include)

.EXTENSIONS:
.EXTENSIONS: .exe
.EXTENSIONS: .obj
.EXTENSIONS: .asm .c .h

dip_os = nt
!include $(dip_dir)\client.mif

objs = global.obj param.obj procctl.obj dipcli.obj memory.obj&
        drwatcom.obj drproc.obj handler.obj sym.obj memview.obj&
        proclist.obj  profile.obj fault.obj thrdctl.obj pefile.obj&
        disasm.obj log.obj reg.obj autoget.obj&
        listbox.obj stat.obj lognote.obj lddips.obj&
        font.obj fontstr.obj mem.obj srchmsg.obj about.obj savelbox.obj &
        mark.obj dlgmod.obj sdkasm.obj memwnd.obj memwndcd.obj ldstr.obj&
        $(dip_objs)


dumobjs = dummy.obj

resources = thrdctl.dlg procctl.dlg notelog.dlg thrdctl.dlg &
            priority.dlg retcode.dlg log.dlg memory.dlg ..\stat.dlg&
            memdmp.dlg $(mastermsg) &
            ..\..\misc\about.dlg ..\..\misc\mark.rc &
            ..\intdlg.dlg

###########################################################################
#
# Dependencies for the main program
#
###########################################################################
$(name).exe : $(msgh) $(objs) $(lnk) $(name).res
        $(link) @$(lnk)
        $(RC) -k $(name).res

$(msgh) : $(mastermsg)
        vi -i -q "-k :source ..\\\\..\\\\misc\\\\msgtoh.vi $(msgh)\n" $(mastermsg)
        del obj\*.obj

$(mastermsg) : $(msgfiles)
        $(comp286) -p -i=..\..\misc drnt.msg > $(mastermsg)

$(name).res : $(name).rc $(resources)
    $(RC) -r $(name).rc  -d__NT__ -fo=$(name).res -i=..\..\misc;..\ -bt=nt

$(lnk) : makefile
        %create $(lnk)
!ifndef on_build_machine
        @%append $(lnk) debug all
!endif
        @%append $(lnk) sys nt_win
        @%append $(lnk) name $(name).exe
        @%append $(lnk) option map
        @%append $(lnk) option stack=9k
        @%append $(lnk) option heapsize=2k
        @for %i in ($(objs)) do @%append $(lnk) file obj\%i
        @%append $(lnk) path $(disasm) @$(disasm)\o2a.lnk

.obj : obj
.c : .;..;..\..\misc;$(dip_dir)\c

sym.obj : sym.c
        $(cc386) $[* -ot-i=$(build_root)\wv\h $(cflags)

mem.obj : mem.c
       $(cc386) $[* $(cflags) -dWANT_MSGS -dNO_WPI

.c.obj :
        $(cc386) $[* $(cflags)
