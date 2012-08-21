# VI Prerequisite Build Control File
# ==================================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <BUILD_PLATFORM> dos386 ]
#================================
    cdsay <PROJDIR>/ctl/dos386
    wmake -h
    cdsay <PROJDIR>/bind/dos386
    wmake -h

[ BLOCK <BUILD_PLATFORM> os2386 ]
#================================
    cdsay <PROJDIR>/ctl/os2386
    wmake -h
    cdsay <PROJDIR>/bind/os2386
    wmake -h

[ BLOCK <BUILD_PLATFORM> nt386 ]
#===============================
    cdsay <PROJDIR>/ctl/nt386
    wmake -h
    cdsay <PROJDIR>/bind/nt386
    wmake -h

[ BLOCK <BUILD_PLATFORM> ntaxp ]
#===============================
    cdsay <PROJDIR>/ctl/ntaxp
    wmake -h
    cdsay <PROJDIR>/bind/ntaxp
    wmake -h

[ BLOCK <BUILD_PLATFORM> linux386 ]
#===============================
    cdsay <PROJDIR>/ctl/linux386
    wmake -h
    cdsay <PROJDIR>/bind/linux386
    wmake -h

[ BLOCK . . ]
#============

cdsay <PROJDIR>
