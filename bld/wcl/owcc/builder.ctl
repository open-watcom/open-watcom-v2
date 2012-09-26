# wcl Builder Control file
# ========================

set PROJDIR=<CWD>
set PROJNAME=owcc

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> boot rel ]
    cdsay <PROJDIR>

[ BLOCK <1> boot ]
#=================
    <CPCMD> <OWOBJDIR>/bowcc.exe <OWBINDIR>/bowcc<CMDEXT>
    <CCCMD> <PROJDIR>/<OWOBJDIR>/bspecs.owc <OWBINDIR>/bspecs.owc

[ BLOCK <1> bootclean ]
#======================
    echo rm -f <OWBINDIR>/bowcc<CMDEXT>
    rm -f <OWBINDIR>/bowcc<CMDEXT>
    echo rm -f <OWBINDIR>/bspecs.owc
    rm -f <OWBINDIR>/bspecs.owc

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> <PROJDIR>/dosi86/specs.owc     <OWRELROOT>/binw/specs.owc
    <CCCMD> <PROJDIR>/dosi86/owcc.exe      <OWRELROOT>/binw/owcc.exe
    <CCCMD> <PROJDIR>/dosi86/owcc.sym      <OWRELROOT>/binw/owcc.sym
    <CCCMD> <PROJDIR>/nt386/specs.owc      <OWRELROOT>/binnt/specs.owc
    <CCCMD> <PROJDIR>/nt386/owcc.exe       <OWRELROOT>/binnt/owcc.exe
    <CCCMD> <PROJDIR>/nt386/owcc.sym       <OWRELROOT>/binnt/owcc.sym
    <CCCMD> <PROJDIR>/os2386/specs.owc     <OWRELROOT>/binp/specs.owc
    <CCCMD> <PROJDIR>/os2386/owcc.exe      <OWRELROOT>/binp/owcc.exe
    <CCCMD> <PROJDIR>/os2386/owcc.sym      <OWRELROOT>/binp/owcc.sym
    <CCCMD> <PROJDIR>/linux386/specs.owc   <OWRELROOT>/binl/specs.owc
    <CCCMD> <PROJDIR>/linux386/owcc.exe    <OWRELROOT>/binl/owcc
    <CCCMD> <PROJDIR>/linux386/owcc.sym    <OWRELROOT>/binl/owcc.sym

[ BLOCK . . ]
#============
cdsay <PROJDIR>
