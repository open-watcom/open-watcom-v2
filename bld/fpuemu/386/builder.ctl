# 386 FPU emulator Builder Control file
# =====================================

set PROJDIR=<CWD>
set PROJNAME=FPU emu 386

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
  [ IFDEF (os_osi os_dos os_win os_os2 os_nt os_linux os_nov os_rdos "") <2*> ]
    <CPCMD> stub386/noemu387.lib <OWRELROOT>/lib386/noemu387.lib
    <CPCMD> stub386/emu387.lib <OWRELROOT>/lib386/osi/emu387.lib

  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> dos386/emu387.lib  <OWRELROOT>/lib386/dos/emu387.lib

  [ IFDEF (os_win "") <2*> ]
    <CPCMD> stub386/emu387.lib <OWRELROOT>/lib386/win/emu387.lib

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> stub386/emu387.lib <OWRELROOT>/lib386/os2/emu387.lib

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> stub386/emu387.lib <OWRELROOT>/lib386/nt/emu387.lib

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> stub386/emu387.lib <OWRELROOT>/lib386/linux/emu387.lib

  [ IFDEF (os_nov "") <2*> ]
    <CPCMD> stub386/emu387.lib <OWRELROOT>/lib386/netware/emu387.lib

  [ IFDEF (os_rdos "") <2*> ]
    <CPCMD> stub386/emu387.lib <OWRELROOT>/lib386/rdos/emu387.lib

  [ ENDIF ]

# Libs not built by default
    <CCCMD> stub386/emu387.lib <OWRELROOT>/lib386/qnx/emu387.lib
    <CCCMD> qnx386/emu387      <OWRELROOT>/qnx/binq/emu387

[ BLOCK . . ]
#============
cdsay <PROJDIR>
