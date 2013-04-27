# whc Builder Control file
# ========================

set PROJDIR=<CWD>
set PROJNAME=whc

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/whc.exe <OWBINDIR>/bwhc<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/whc.exe      <OWRELROOT>/binw/
    <CCCMD> nt386/whc.exe       <OWRELROOT>/binnt/
    <CCCMD> os2386/whc.exe      <OWRELROOT>/binp/
    <CCCMD> linux386/whc.exe    <OWRELROOT>/binl/whc

    <CCCMD> ntx64/whc.exe       <OWRELROOT>/binnt64/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
