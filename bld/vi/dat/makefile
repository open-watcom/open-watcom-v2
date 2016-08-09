tmpvi = x.x
!ifndef 386
vicc = ..\obj286v\viv.exe
!else
vicc = ..\obj386\vi386.exe
!endif

.EXTENSIONS
.EXTENSIONS : .exe
.EXTENSIONS : ._vi
.EXTENSIONS : .vi .cfg

VIS = &
    ed._vi rdme._vi wrme._vi rcs._vi qall._vi proc._vi err._vi &
    chkout._vi unlock._vi forceout._vi mcsel._vi mlsel._vi lnum._vi &
    fin._vi menu._vi qh._vi qh2._vi menuwin._vi

lastdef.tim : exetest.tim $(VIS) edbind.dat
    @wtouch lastdef.tim

exetest.tim : $(vicc)
    @erase *._vi
    @wtouch exetest.tim
    @%null

touch : .SYMBOLIC
    @wtouch lastdef.tim
    @wtouch exetest.tim

clean: .SYMBOLIC
    erase *._vi

.vi._vi :
    $(vicc) -d -q -k ":comp $[*.vi\n:quit\n"

ed._vi : ed.cfg
    $(vicc) -d -q -k ":comp -a ed.cfg\n:quit\n"

win._vi : win.cfg
    $(vicc) -d -q -k ":comp -a win.cfg\n:quit\n"
