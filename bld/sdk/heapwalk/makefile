name = wheapwlk

msgh = rcstr.h
mastermsg = rcstr.msg
msgfiles = heapwalk.msg ..\misc\savelbox.msg ..\misc\memwnd.msg &
           ..\misc\about.msg

lnk = $(name).lnk
cflags= -zq-2-s-zu-d1-zW-fpc-w4 -fo=obj\$[&.obj -i=..\misc;$(wdisasmdir)h;$(win31_hdrs)
!ifndef on_build_machine
cflags += -d2
!endif

rc = wrc

.EXTENSIONS:
.EXTENSIONS: .exe
.EXTENSIONS: .obj
.EXTENSIONS: .asm .c .h .dlg

objs =  heapwalk.obj hwlist.obj hwproc.obj hwglob.obj hwsort.obj &
        hwbox.obj hwdisp.obj hwsave.obj hwobjec.obj hwlocal.obj&
        hwlsort.obj hwconfig.obj  hwinfo.obj hwalloc.obj hwutil.obj&
        hwbiglb.obj hwmonit.obj hwtable.obj hwldstr.obj&
        mythelp.obj mem.obj font.obj fontstr.obj segmem.obj segmem2.obj &
        dlgmod.obj savelbox.obj memwnd.obj memwndcd.obj srchmsg.obj&
        pushwin.obj sdkasm.obj about.obj ldstr.obj wwinhelp.obj jdlg.obj

!include $(wdisasmdir)mif\disasm.mif

resources = 10hw.ico heapwlk.dlg ..\misc\memwnd.rc &
            heapinfo.dlg memman.dlg lclinfo.dlg add.dlg config.dlg &
            freen.dlg alloc.dlg code.dlg config.dlg ..\misc\about.dlg

$(name).exe : $(msgh) $(objs) $(disasm) $(lnk) $(name).res
        wlink @$(lnk)
        $(RC) -k $(name).res

$(name).res : $(name).rc $(resources) $(msgh)
    $(RC) -zk0 -r -bt=windows $(name).rc -i=..\misc

$(lnk) : makefile
        %create $(lnk)
!ifndef on_build_machine
        @%append $(lnk) debug all
!endif
        @%append $(lnk) system windows
        @%append $(lnk) name $(name).exe
        @%append $(lnk) library windows.lib
        @%append $(lnk) library toolhelp.lib
        @%append $(lnk) library commdlg.lib
        @%append $(lnk) library stress.lib
        @%append $(lnk) library ..\misc\ctl3d.lib
        @%append $(lnk) option map
        @%append $(lnk) option stack=7k
        @%append $(lnk) option heapsize=2k
        @for %i in ($(objs)) do @%append $(lnk) file obj\%i
        @for %i in ($(disasm)) do @%append $(lnk) file obj\%i

$(msgh) : $(mastermsg)
        vi -i -q "-k :source ..\\\\misc\\\\msgtoh.vi $(msgh)\n" $(mastermsg)
        -del obj\*.obj

$(mastermsg) : $(msgfiles)
        $(comp286) -zk0 -p -i=..\misc heapwalk.msg > $(mastermsg)

ldstr.obj : ldstr.c
        $(comp286) $[* -ml$(cflags) -dSPECIAL_STRING_LOADING

.obj : obj

.c : .;..\misc;$(wdisasmdir)c
.asm : .;..\misc
.dlg : .;..\misc

mem.obj : mem.c
        $(comp286) $[* -ml$(cflags) -dNO_WPI

.c.obj :
        $(comp286) $[* -ml$(cflags)
.asm.obj:
        masm /Mx /Zd $[*,obj\$[&.obj;

clean : .symbolic
    @rm -f *.exe *.res *.lnk *.map
    @rm -f $(msgh)
    @rm -f $(mastermsg)
    @rm -f obj\*.obj

