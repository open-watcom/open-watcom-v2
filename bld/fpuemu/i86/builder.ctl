# i86 FPU emulator Builder Control file
# =====================================

set PROJNAME=emui86

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> stubi86/emu87.lib    <OWRELROOT>/lib286/
    <CCCMD> stubi86/noemu87.lib  <OWRELROOT>/lib286/

    <CCCMD> dosi86/emu87.lib     <OWRELROOT>/lib286/dos/
    <CCCMD> wini86/emu87.lib     <OWRELROOT>/lib286/win/
    <CCCMD> os2i86/emu87.lib     <OWRELROOT>/lib286/os2/

# QNX emulators setup program
    <CCCMD> qnxi86/emu87         <OWRELROOT>/qnx/binq/emu86
# QNX 8087 emulators
    <CCCMD> qnxi86/emu87_16      <OWRELROOT>/qnx/binq/emu86_16
    <CCCMD> qnxi86/emu87_32      <OWRELROOT>/qnx/binq/emu86_32

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
