inc_dirs_lang_qnx = $(lang_root)\qh
inc_dirs_lang_netware = $(lang_root)\novh

!ifdef inc_dirs_lang_$(host_OS)
inc_dirs_lang   = $(inc_dirs_lang_$(host_OS))
!else
inc_dirs_lang   = $(lang_root)\h
!endif

.before:
        @set incpaths=-i$(auidir)\h;$(guidir)\h;..\h;$(posix_dir)\h;$(inc_dirs_lang)

.extensions:
.extensions: .exe .lib .obj .i .c .h .res .dlg .rc
.c: $(auidir)\c;.
.h: $(auidir)\h;.

!ifdef on_build_machine
copts = /zq /oxt /d1
!else
!ifndef nodebug
!ifdef codeview
copts = /hc /d1 /zq
!else
copts = /zq /d2
!endif
!else
copts = /zq /oxt /d1
!endif
!endif
!ifndef axp
copts += /fpc
!endif
copts += @incpaths

!ifndef librarian
librarian = wlib
!endif

.c.obj:
        $(compile) $(copts) /fh

.c.i:
        $(compile) $(copts) /fh /plc /fo

!ifdef pm
dlgs = dlgrx.dlg dlgsrch.dlg dlgsrcha.dlg &
       japrx.dlg japsrch.dlg japsrcha.dlg
auirc = $(auidir)\pm\aui.rc
!else
auirc = $(auidir)\h\aui.rc
!endif

all : .symbolic $(auidir)\h\auistr.h aui.lib $(dlgs)
        @%null

eraseall.obj : $(auidir)\compile.gbl
        if exist *.obj erase *.obj
        wtouch eraseall.obj

regexp.obj : $(posix_dir)\misc\regexp.c ..\h\wndregx.h
        $(compile) $(copts) -fi=wndregx.h

objs =  guimain.obj &
        wndgadgt.obj &
        wndrect.obj &
        wndtbatr.obj &
        wndbgch.obj &
        wndstyle.obj &
        wndhist.obj &
        wndhelp.obj &
        wndstat.obj &
        wndchoos.obj &
        wndcreat.obj &
        wndcrsr.obj &
        wndcurr.obj &
        wndid.obj &
        wndmem.obj &
        wndmeth.obj &
        wndutil.obj &
        wndchar.obj &
        wndpop.obj &
        regexp.obj &
        wndscrol.obj &
        wndsel.obj &
        wndsrch.obj &
        wndfdlg.obj &
        wnddlg.obj &
        dlgsrch.obj &
        dlgpick.obj &
        dlgnew.obj &
        wndspawn.obj &
        wndalloc.obj &
        wndshrnk.obj &
        wndtool.obj &
        wnddoin.obj &
        wndresiz.obj &
        wndsysin.obj &
        wndclean.obj &
        wndmacro.obj &
        wndring.obj &
        wndefra.obj &
        wndfonhk.obj &
        wndshthk.obj &
        wndqryhk.obj &
        wndendhk.obj &
        wndrszhk.obj &
        wndsfra.obj &
        wndfont.obj &
        wndldstr.obj &
        wndfront.obj &
        wndpaint.obj &
        wnddirt.obj &
        wnddfclr.obj &
        wndcolor.obj &
        wndmouse.obj &
        wndrxerr.obj &
        wndfref.obj &
        wndmsg.obj &
        wndmnsz.obj &
        wndimain.obj &

aui.lib : eraseall.obj $(objs) $(__makefiles__) $(auirc)
        @if exist tmp.lbc erase tmp.lbc
        @for %i in ($(objs)) do @%append tmp.lbc +%i
        $(librarian) /q /b /n aui @tmp.lbc

.rc: ..\h
!ifdef pm
$(auirc) : $(auidir)\h\aui.rc
    copy $(auidir)\h\aui.rc

dlgrx.dlg : ..\h\dlgrx.dlg
    parsedlg -font ..\h\dlgrx.dlg dlgrx.dlg

dlgsrch.dlg : ..\h\dlgsrch.dlg
    parsedlg -font ..\h\dlgsrch.dlg dlgsrch.dlg

dlgsrcha.dlg : ..\h\dlgsrcha.dlg
    parsedlg -font ..\h\dlgsrcha.dlg dlgsrcha.dlg

japrx.dlg : ..\h\japrx.dlg
    parsedlg -font ..\h\japrx.dlg japrx.dlg

japsrch.dlg : ..\h\japsrch.dlg
    parsedlg -font ..\h\japsrch.dlg japsrch.dlg

japsrcha.dlg : ..\h\japsrcha.dlg
    parsedlg -font ..\h\japsrcha.dlg japsrcha.dlg
!else
.dlg : ..\h
!endif

dlgfiles = dlgrx.h dlgsrch.h dlgrx.dlg dlgsrch.dlg dlgsrcha.dlg $(auidir)\h\auistr.h auistr.rc auigui.rc

$(auidir)\h\auistr.h : $(auidir)\h\aui.msg $(auidir)\c\mkstr.c
        wcl386 -zq $(auidir)\c\mkstr
        mkstr $(auidir)\h\auistr.h
        del mkstr.obj
        del mkstr.exe

clean : .SYMBOLIC
        @if exist *.obj @del *.obj
        @if exist *.pch @del *.pch
        @if exist aui.lib @del aui.lib
        @if exist aui.rc @del aui.rc
        @if exist tmp.lbc @del tmp.lbc
        @if exist dlg*.dlg @del dlg*.dlg
        @if exist jap*.dlg @del jap*.dlg
        @if exist $(auidir)\h\auistr.h @del $(auidir)\h\auistr.h

