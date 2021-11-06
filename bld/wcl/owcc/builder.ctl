# wcl Builder Control file
# ========================

set PROJNAME=owcc

set BINTOOL=0

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/bowcc.exe <OWBINDIR>/<OWOBJDIR>/bowcc<CMDEXT>
    <CCCMD> <OWOBJDIR>/bspecs.owc <OWBINDIR>/<OWOBJDIR>/bspecs.owc

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/<OWOBJDIR>/bowcc<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/bowcc<CMDEXT>
    echo rm -f <OWBINDIR>/<OWOBJDIR>/bspecs.owc
    rm -f <OWBINDIR>/<OWOBJDIR>/bspecs.owc

[ BLOCK <BLDRULE> rel cprel ]
#============================
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
    <CCCMD> linuxx64/specs.owc   <OWRELROOT>/binl64/
    <CCCMD> linuxx64/owcc.exe    <OWRELROOT>/binl64/owcc
    <CCCMD> linuxarm/specs.owc   <OWRELROOT>/arml/
    <CCCMD> linuxarm/owcc.exe    <OWRELROOT>/arml/owcc
    <CCCMD> osxx64/specs.owc     <OWRELROOT>/osx64/
    <CCCMD> osxx64/owcc.exe      <OWRELROOT>/osx64/owcc

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
