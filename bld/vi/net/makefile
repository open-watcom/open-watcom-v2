.EXTENSIONS
.EXTENSIONS : .exe
.EXTENSIONS : ._vi
.EXTENSIONS : .vi .cfg

lang = r:\dlang

VIOBJS = &
        ed._vi &
        rdme._vi &
        wrme._vi &
        rcs._vi &
        qall._vi &
        proc._vi &
        err._vi &
        chkout._vi &
        unlock._vi &
        forceout._vi &
        mcsel._vi &
        mlsel._vi &
        lnum._vi

VIS = &
        ed.cfg &
        rdme.vi &
        wrme.vi &
        rcs.vi &
        qall.vi &
        proc.vi &
        err.vi &
        chkout.vi &
        unlock.vi &
        forceout.vi &
        mcsel.vi &
        mlsel.vi &
        lnum.vi

DATS = keys.dat errmsg.dat error.dat edbind.dat
HLPS = key.hlp cmd.hlp regexp.hlp script.hlp start.hlp
BINDERS = edbind.exe
EXES = vi.exe vip.exe

all : .symbolic $(VIS) $(DATS) $(HLPS) $(BINDERS) $(EXES)

netcopy : .procedure
        copy $[@ $^@

ed.cfg :: $(lang)\eddat\ed.cfg
        @%make netcopy
rdme.vi :: $(lang)\eddat\rdme.vi
        @%make netcopy
wrme.vi :: $(lang)\eddat\wrme.vi
        @%make netcopy
rcs.vi :: $(lang)\eddat\rcs.vi
        @%make netcopy
qall.vi :: $(lang)\eddat\qall.vi
        @%make netcopy
proc.vi :: $(lang)\eddat\proc.vi
        @%make netcopy
err.vi :: $(lang)\eddat\err.vi
        @%make netcopy
chkout.vi :: $(lang)\eddat\chkout.vi
        @%make netcopy
unlock.vi :: $(lang)\eddat\unlock.vi
        @%make netcopy
forceout.vi :: $(lang)\eddat\forceout.vi
        @%make netcopy
mcsel.vi :: $(lang)\eddat\mcsel.vi
        @%make netcopy
mlsel.vi :: $(lang)\eddat\mlsel.vi
        @%make netcopy
lnum.vi :: $(lang)\eddat\lnum.vi
        @%make netcopy
edbind.dat :: $(lang)\eddat\ed.cfg
        @%make netcopy
keys.dat :: $(lang)\eddat\keys.dat
        @%make netcopy
errmsg.dat :: $(lang)\eddat\errmsg.dat
        @%make netcopy
error.dat :: $(lang)\eddat\error.dat
        @%make netcopy
key.hlp :: $(lang)\eddat\key.hlp
        @%make netcopy
cmd.hlp :: $(lang)\eddat\cmd.hlp
        @%make netcopy
mouse.hlp :: $(lang)\eddat\mouse.hlp
        @%make netcopy
regexp.hlp :: $(lang)\eddat\regexp.hlp
        @%make netcopy
script.hlp :: $(lang)\eddat\script.hlp
        @%make netcopy
start.hlp :: $(lang)\eddat\start.hlp
        @%make netcopy
edbind.exe :: $(lang)\binb\edbind.exe
        @%make netcopy
vi.exe :: $(lang)\bin\vi.exe
        @%make netcopy
vip.exe :: $(lang)\bin\vip.exe
        @%make netcopy

recomp._vi : $(lang)\eddat\ed._vi
        erase *._vi
        wtouch recomp._vi

vi.exe :: $(VIOBJS) edbind.dat $(lang)\bin\vi.exe
        edbind vi.exe>nul
vip.exe :: $(VIOBJS) edbind.dat $(lang)\bin\vip.exe
        edbind vip.exe>nul

clean: .SYMBOLIC
        @erase *._vi

.vi._vi :
        @%create $[*.scr
        @%append $[*.scr comp $[*.vi
        @%append $[*.scr quit
        vi -c -q -s$[*.scr
        @erase $[*.scr

ed._vi : ed.cfg
        @%create ed.scr
        @%append ed.scr comp -a ed.cfg
        @%append ed.scr quit
        vi -c -q -sed.scr
        @erase ed.scr
