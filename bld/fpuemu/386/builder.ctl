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
    <CCCMD> stub386/noemu387.lib <OWRELROOT>/lib386/noemu387.lib
    <CCCMD> stub386/emu387.lib   <OWRELROOT>/lib386/osi/emu387.lib
    <CCCMD> dos386/emu387.lib    <OWRELROOT>/lib386/dos/emu387.lib
    <CCCMD> stub386/emu387.lib   <OWRELROOT>/lib386/win/emu387.lib
    <CCCMD> stub386/emu387.lib   <OWRELROOT>/lib386/os2/emu387.lib
    <CCCMD> stub386/emu387.lib   <OWRELROOT>/lib386/nt/emu387.lib
    <CCCMD> stub386/emu387.lib   <OWRELROOT>/lib386/linux/emu387.lib
    <CCCMD> stub386/emu387.lib   <OWRELROOT>/lib386/netware/emu387.lib
    <CCCMD> stub386/emu387.lib   <OWRELROOT>/lib386/rdos/emu387.lib
# Libs not built by default
    <CCCMD> stub386/emu387.lib   <OWRELROOT>/lib386/qnx/emu387.lib
    <CCCMD> qnx386/emu387        <OWRELROOT>/qnx/binq/emu387

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
