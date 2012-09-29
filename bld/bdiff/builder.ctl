# bpatch Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=bdiff

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

    # Make BPATCH.QNX for Database guys. Easier than getting them set up
#    wmake -h bpatch.qnx

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/bdiff.exe     <OWRELROOT>/binw/bdiff.exe
    <CCCMD> dos386/bpatch.exe    <OWRELROOT>/binw/bpatch.exe

    <CCCMD> os2386/bdiff.exe     <OWRELROOT>/binp/bdiff.exe
    <CCCMD> os2386/bpatch.exe    <OWRELROOT>/binp/bpatch.exe

    <CCCMD> nt386/bdiff.exe      <OWRELROOT>/binnt/bdiff.exe
    <CCCMD> nt386/bpatch.exe     <OWRELROOT>/binnt/bpatch.exe

[ BLOCK . . ]
#============
cdsay <PROJDIR>
