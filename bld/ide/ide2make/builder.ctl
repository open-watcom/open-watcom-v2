# ide2make Builder Control file
# =============================

set PROJDIR=<CWD>
set PROJNAME=ide2make

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/ide2make.exe <OWBINDIR>/bide2mak<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/ide2make.exe   <OWRELROOT>/binw/
    <CCCMD> os2386/ide2make.exe   <OWRELROOT>/binp/
    <CCCMD> nt386/ide2make.exe    <OWRELROOT>/binnt/
    <CCCMD> linux386/ide2make.exe <OWRELROOT>/binl/ide2make
    <CCCMD> ntaxp/ide2make.exe    <OWRELROOT>/axpnt/

[ BLOCK . . ]
#============
cdsay <PROJDIR>

