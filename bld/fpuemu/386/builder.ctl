# 386 FPU emulator Builder Control file
# =====================================

set PROJNAME=emu386

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
#================
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> stub386/emu387.lib   <OWRELROOT>/lib386/
    <CCCMD> stub386/noemu387.lib <OWRELROOT>/lib386/

    <CCCMD> dos386/emu387.lib    <OWRELROOT>/lib386/dos/

# QNX emulator setup programs
    <CCCMD> qnx386/emu387.exe    <OWRELROOT>/qnx/binq/emu387

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
