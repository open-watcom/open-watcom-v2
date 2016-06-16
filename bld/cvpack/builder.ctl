# cvpack Builder Control file
# ===========================

set PROJNAME=cvpack

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/deftool.ctl ]

[ BLOCK <1> rel ]
#================
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/cvpack.exe    <OWRELROOT>/binw/
    <CCCMD> os2386/cvpack.exe    <OWRELROOT>/binp/
    <CCCMD> nt386/cvpack.exe     <OWRELROOT>/binnt/
    <CCCMD> linux386/cvpack.exe  <OWRELROOT>/binl/cvpack
    <CCCMD> ntaxp/cvpack.exe     <OWRELROOT>/axpnt/

#    <CCCMD> linuxx64/cvpack.exe  <OWRELROOT>/binl64/cvpack
    <CCCMD> ntx64/cvpack.exe     <OWRELROOT>/binnt64/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
