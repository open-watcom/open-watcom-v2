name = wspy
namej = wspy
dllname = wspyhk
lnk = $(name).lnk
dlllnk = $(dllname).lnk

msgh = rcstr.h
mastermsg = rcstr.msg
msgfiles = spy.msg ..\misc\savelbox.msg ..\misc\log.msg ..\misc\about.msg &
        ..\misc\hint.msg

ntname = wspynt
ntnamej = wspyntj
ntlnk = $(ntname).lnk
ntdllname = ntspyhk
ntdlllnk = $(ntdllname).lnk

cflags= -zq-oaxt-s-fpc-w4-i=..\misc
cflagswin= -zu-ml-2-zW-fo=obj\.obj
cflagswindll= -mc-zu-2-zW
cflagsntdll= -bd-bt=nt -i=$(watcom_nt_h)
cflagsnt= -mf-4-bt=nt-fo=obj\.3bj -i=$(watcom_nt_h)

rcopts1= -r -i=..\misc -zk0
rcopts2= -k

!ifndef on_build_machine
cflags += -d2
!endif

rc = wrc
link = $(linker)
cc = $(comp286)
cc386 = $(comp386)

.EXTENSIONS:
.EXTENSIONS: .exe
.EXTENSIONS: .obj .3bj
.EXTENSIONS: .asm .c .h

objs =  spy.obj spyglob.obj spybox.obj spyproc.obj spymsgs.obj &
        spyfilt.obj spypick.obj spymdlgs.obj spyarray.obj &
        spymisc.obj spycfg.obj spylog.obj spytool.obj spysel.obj &
        savelbox.obj toolbr.obj font.obj fontstr.obj mem.obj log.obj &
        mark.obj about.obj ldstr.obj mstatwnd.obj hint.obj wwinhelp.obj &
        jdlg.obj

resources = $(name).rc 10spy.ico msgs.dlg winpick.dlg &
            msgsel.dlg ..\misc\about.dlg peekmsg.dlg selwin.dlg &
            peekwin.dlg ..\misc\log.rc $(mastermsg)
ntresources = spymsgnt.dlg
winresources = spymsgs.dlg

dllobjs = spydll.obj

ntdllobjs = spydll.3bj

ntobjs =  spy.3bj spyglob.3bj spybox.3bj spyproc.3bj spymsgs.3bj &
        spyfilt.3bj spypick.3bj spymdlgs.3bj spyarray.3bj spyzord.3bj&
        spymisc.3bj spycfg.3bj spylog.3bj spytool.3bj spysel.3bj &
        savelbox.3bj toolbr.3bj font.3bj fontstr.3bj mem.3bj log.3bj &
        mark.3bj about.3bj desknt.3bj ldstr.3bj mstatwnd.3bj hint.3bj &
        wwinhelp.3bj jdlg.3bj

all : $(msgh) spymsgs.dlg spymsg.h $(dllname).dll $(name).exe .SYMBOLIC
        @%null

nt : $(msgh) spymsgnt.dlg spymsgnt.h $(ntdllname).dll $(ntname).exe .SYMBOLIC
        @%null


$(msgh) : $(mastermsg)
        vi -i -q "-k :source ..\\\\misc\\\\msgtoh.vi $(msgh)\n" $(mastermsg)
#       del obj\*.obj
#       del obj\*.3bj

$(mastermsg) : $(msgfiles)
        $(comp286) -zk0 -p -i=..\misc spy.msg > $(mastermsg)

spymsgs.dlg spymsg.h: spyarray.h
        cd utils
        wmake
        cd ..
        utils\mkdlg spymsgs.dlg spymsg.h

spymsgnt.dlg spymsgnt.h: spyarray.h
        cd utils
        wmake defines=-dNT_MSGS
        cd ..
        utils\mkdlg spymsgnt.dlg spymsgnt.h

$(name).exe : $(objs) $(lnk) $(name).res $(dllname).lib
        $(link) @$(lnk)
        $(RC) $(rcopts2) $(name).res

$(dllname).dll : $(dllobjs) $(dlllnk)
        $(link) @$(dlllnk)
        @erase $(dllname).lib > NUL
        wlib $(dllname).lib +$(dllname).dll

$(ntname).exe : $(ntobjs) $(ntlnk) $(ntname).res $(ntdllname).lib
        $(link) @$(ntlnk)
        $(RC) $(rcopts2) $(ntname).res

$(ntdllname).dll : $(ntdllobjs) $(ntdlllnk)
        $(link) @$(ntdlllnk)
        @erase $(ntdllname).lib > NUL
        wlib $(ntdllname).lib +$(ntdllname).dll

$(name).res : $(resources) $(winresources)
    $(RC) $(rcopts1) $(name).rc -bt=windows

$(ntname).res : $(resources) $(ntresources)
    $(RC) $(rcopts1) $(name).rc  -d__NT__ -fo=$(ntname).res -bt=nt

$(lnk) : makefile
        %create $(lnk)
!ifndef on_build_machine
        @%append $(lnk) debug all
!endif
        @%append $(lnk) system windows font mem
        @%append $(lnk) name $(name).exe
        @%append $(lnk) library windows.lib,commdlg.lib,toolhelp.lib
        @%append $(lnk) library $(dllname).lib
        @%append $(lnk) library ..\misc\ctl3d.lib
        @%append $(lnk) option map
!ifndef on_build_machine
        @%append $(lnk) option rwr
!endif
        @%append $(lnk) option stack=9k
        @%append $(lnk) option heapsize=2k
        @for %i in ($(objs)) do @%append $(lnk) file obj\%i

$(ntlnk) : makefile
        %create $(ntlnk)
!ifndef on_build_machine
        @%append $(ntlnk) debug all
!endif
        @%append $(ntlnk) sys nt_win
        @%append $(ntlnk) name $(ntname).exe
        @%append $(ntlnk) library $(ntdllname).lib
        @%append $(ntlnk) library ctl3d32.lib
        @%append $(ntlnk) option map
        @%append $(ntlnk) option stack=64k
        @%append $(ntlnk) option heapsize=2k
        @for %i in ($(ntobjs)) do @%append $(ntlnk) file obj\%i

$(dlllnk) : makefile
        %create $(dlllnk)
!ifndef on_build_machine
        @%append $(dlllnk) debug all
!endif
        @%append $(dlllnk) system windows dll font mem
        @%append $(dlllnk) name $(dllname).dll
        @%append $(dlllnk) library windows.lib
        @%append $(dlllnk) exp WEP.1 resident
        @%append $(dlllnk) option map
!ifndef on_build_machine
        @%append $(dlllnk) option rwr
!endif
        @%append $(dlllnk) option stack=2k
        @%append $(dlllnk) segment CLASS 'CODE' PRELOAD FIXED
        @%append $(dlllnk) libfile libentry.obj
        @for %i in ($(dllobjs)) do @%append $(dlllnk) file obj\%i

$(ntdlllnk) : makefile
        %create $(ntdlllnk)
!ifndef on_build_machine
        @%append $(ntdlllnk) debug all
!endif
        @%append $(ntdlllnk) system nt_dll initglobal
        @%append $(ntdlllnk) name $(ntdllname).dll
        @%append $(ntdlllnk) libpath d:\utils\watcom\lib
        @%append $(ntdlllnk) library nt
        @%append $(ntdlllnk) option map
        @for %i in ($(ntdllobjs)) do @%append $(ntdlllnk) file obj\%i

.obj : obj
.3bj : obj
.c : .;..\misc

spydll.obj : spydll.c
        $(cc) spydll $(cflags) $(cflagswindll) -fo=obj\spydll.obj

spydll.3bj : spydll.c
        $(cc386) spydll $(cflags) $(cflagsntdll) -fo=obj\spydll.3bj

mem.obj : mem.c
        $(cc) $[* $(cflags) $(cflagswin) -dNO_WPI

spymdlgs.obj: spymdlgs.c spymsg.h
        $(cc) spymdlgs.c $(cflags) $(cflagswin)

spymdlgs.3bj: spymdlgs.c spymsgnt.h
        $(cc386) spymdlgs.c $(cflags) $(cflagsnt)

mem.3bj : mem.c
        $(cc386) $[* $(cflags) $(cflagsnt) -dNO_WPI


.c.obj :
        $(cc) $[* $(cflags) $(cflagswin)
.c.3bj :
        $(cc386) $[* $(cflags) $(cflagsnt)
