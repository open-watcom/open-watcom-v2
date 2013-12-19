# WIPFC Builder Control file
# ==========================

set PROJDIR=<CWD>
set PROJNAME=wipfc

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/wipfc.exe <OWBINDIR>/bwipfc<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CPCMD> helper/*.nls          <OWRELROOT>/wipfc/
    <CPCMD> helper/*.txt          <OWRELROOT>/wipfc/

    <CCCMD> dos386/wipfc.exe      <OWRELROOT>/binw/
    <CCCMD> os2386/wipfc.exe      <OWRELROOT>/binp/
    <CCCMD> nt386/wipfc.exe       <OWRELROOT>/binnt/
    <CCCMD> linux386/wipfc.exe    <OWRELROOT>/binl/wipfc

    <CCCMD> ntx64/wipfc.exe       <OWRELROOT>/binnt64/
    <CCCMD> linuxx64/wipfc.exe    <OWRELROOT>/binl64/wipfc

[ BLOCK . . ]
#============
cdsay <PROJDIR>
