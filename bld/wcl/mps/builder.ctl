# wcl Builder Control file
# ========================

set PROJNAME=wclmps

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
    <CPCMD> <OWOBJDIR>/bwclmps.exe <OWBINDIR>/<OWOBJDIR>/bwclmps<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/<OWOBJDIR>/bwclmps<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/bwclmps<CMDEXT>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> nt386/wclmps.exe      <OWRELROOT>/binnt/
    <CCCMD> nt386/wclmps.sym      <OWRELROOT>/binnt/
    <CCCMD> os2386/wclmps.exe     <OWRELROOT>/binp/
    <CCCMD> os2386/wclmps.sym     <OWRELROOT>/binp/
    <CCCMD> linux386/wclmps.exe   <OWRELROOT>/binl/wclmps
    <CCCMD> linux386/wclmps.sym   <OWRELROOT>/binl/

    <CCCMD> ntx64/wclmps.exe      <OWRELROOT>/binnt64/
    <CCCMD> linuxx64/wclmps.exe   <OWRELROOT>/binl64/wclmps

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
