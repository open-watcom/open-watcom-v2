# WIPFC Builder Control file
# ==========================

set PROJDIR=<CWD>
set PROJNAME=bwipfc

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/bintool.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CPCMD> helper/*.nls          <OWRELROOT>/wipfc/
    <CPCMD> helper/*.txt          <OWRELROOT>/wipfc/

    <CCCMD> dos386/wipfc.exe      <OWRELROOT>/binw/wipfc.exe
    <CCCMD> os2386/wipfc.exe      <OWRELROOT>/binp/wipfc.exe
    <CCCMD> nt386/wipfc.exe       <OWRELROOT>/binnt/wipfc.exe
    <CCCMD> linux386/wipfc.exe    <OWRELROOT>/binl/wipfc

[ BLOCK . . ]
#============
cdsay <PROJDIR>
