# EMU Builder Control file
# ==========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/bat/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
#=======================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> rel2 cprel2 ]
#========================
  [ IFDEF (os_osi os_dos os_win os_os2 os_nt os_linux os_nov os_rdos "") <2*> ]
    <CPCMD> <DEVDIR>/emu/stub386/emu387.lib <RELROOT>/lib386/noemu387.lib
    <CPCMD> <DEVDIR>/emu/stub386/emu387.lib <RELROOT>/lib386/osi/emu387.lib

  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> <DEVDIR>/emu/dos386/emu387.lib  <RELROOT>/lib386/dos/emu387.lib

  [ IFDEF (os_win "") <2*> ]
    <CPCMD> <DEVDIR>/emu/stub386/emu387.lib <RELROOT>/lib386/win/emu387.lib

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> <DEVDIR>/emu/stub386/emu387.lib <RELROOT>/lib386/os2/emu387.lib

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> <DEVDIR>/emu/stub386/emu387.lib <RELROOT>/lib386/nt/emu387.lib

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> <DEVDIR>/emu/stub386/emu387.lib <RELROOT>/lib386/linux/emu387.lib

  [ IFDEF (os_nov "") <2*> ]
    <CPCMD> <DEVDIR>/emu/stub386/emu387.lib <RELROOT>/lib386/netware/emu387.lib

  [ IFDEF (os_rdos "") <2*> ]
    <CPCMD> <DEVDIR>/emu/stub386/emu387.lib <RELROOT>/lib386/rdos/emu387.lib

  [ ENDIF ]

# Libs not built by default
    <CCCMD> <DEVDIR>/emu/stub386/emu387.lib <RELROOT>/lib386/qnx/emu387.lib
    <CCCMD> <DEVDIR>/emu/qnx386/emu387      <RELROOT>/qnx/binq/emu387

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
