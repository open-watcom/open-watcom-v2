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
    <CCCMD> dosi86/specs.owc     <OWRELROOT>/binw/
    <CCCMD> dosi86/owcc.exe      <OWRELROOT>/binw/
    <CCCMD> dosi86/owcc.sym      <OWRELROOT>/binw/
    <CCCMD> nt386/specs.owc      <OWRELROOT>/binnt/
    <CCCMD> nt386/owcc.exe       <OWRELROOT>/binnt/
    <CCCMD> nt386/owcc.sym       <OWRELROOT>/binnt/
    <CCCMD> os2386/specs.owc     <OWRELROOT>/binp/
    <CCCMD> os2386/owcc.exe      <OWRELROOT>/binp/
    <CCCMD> os2386/owcc.sym      <OWRELROOT>/binp/
    <CCCMD> linux386/specs.owc   <OWRELROOT>/binl/
    <CCCMD> linux386/owcc.exe    <OWRELROOT>/binl/owcc
    <CCCMD> linux386/owcc.sym    <OWRELROOT>/binl/

    <CCCMD> ntx64/specs.owc      <OWRELROOT>/binnt64/
    <CCCMD> ntx64/owcc.exe       <OWRELROOT>/binnt64/
    <CCCMD> linuxx64/owcc.exe    <OWRELROOT>/binl64/owcc

[ BLOCK . . ]
#============
cdsay <PROJDIR>
