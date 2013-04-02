# wcl Builder Control file
# ========================

set PROJDIR=<CWD>
set PROJNAME=owcc

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/bowcc.exe <OWBINDIR>/bowcc<CMDEXT>
    <CCCMD> <OWOBJDIR>/bspecs.owc <OWBINDIR>/bspecs.owc

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/bowcc<CMDEXT>
    rm -f <OWBINDIR>/bowcc<CMDEXT>
    echo rm -f <OWBINDIR>/bspecs.owc
    rm -f <OWBINDIR>/bspecs.owc

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dosi86/specs.owc     <OWRELROOT>/binw/specs.owc
    <CCCMD> dosi86/owcc.exe      <OWRELROOT>/binw/owcc.exe
    <CCCMD> dosi86/owcc.sym      <OWRELROOT>/binw/owcc.sym
    <CCCMD> nt386/specs.owc      <OWRELROOT>/binnt/specs.owc
    <CCCMD> nt386/owcc.exe       <OWRELROOT>/binnt/owcc.exe
    <CCCMD> nt386/owcc.sym       <OWRELROOT>/binnt/owcc.sym
    <CCCMD> os2386/specs.owc     <OWRELROOT>/binp/specs.owc
    <CCCMD> os2386/owcc.exe      <OWRELROOT>/binp/owcc.exe
    <CCCMD> os2386/owcc.sym      <OWRELROOT>/binp/owcc.sym
    <CCCMD> linux386/specs.owc   <OWRELROOT>/binl/specs.owc
    <CCCMD> linux386/owcc.exe    <OWRELROOT>/binl/owcc
    <CCCMD> linux386/owcc.sym    <OWRELROOT>/binl/owcc.sym

[ BLOCK . . ]
#============
cdsay <PROJDIR>
