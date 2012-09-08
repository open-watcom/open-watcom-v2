# cfe Builder Control file
# ==========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
#=======================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> rel2 cprel2 acprel2 ]
#================================
  [ IFDEF (os_osi os_dos os_win os_os2 os_nt os_linux os_nov os_rdos "") <2*> ]
    <CPCMD> 386/stub386/emu387.lib <OWRELROOT>/lib386/noemu387.lib
    <CPCMD> 386/stub386/emu387.lib <OWRELROOT>/lib386/osi/emu387.lib

  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> 386/dos386/emu387.lib  <OWRELROOT>/lib386/dos/emu387.lib

  [ IFDEF (os_win "") <2*> ]
    <CPCMD> 386/stub386/emu387.lib <OWRELROOT>/lib386/win/emu387.lib

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> 386/stub386/emu387.lib <OWRELROOT>/lib386/os2/emu387.lib

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> 386/stub386/emu387.lib <OWRELROOT>/lib386/nt/emu387.lib

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> 386/stub386/emu387.lib <OWRELROOT>/lib386/linux/emu387.lib

  [ IFDEF (os_nov "") <2*> ]
    <CPCMD> 386/stub386/emu387.lib <OWRELROOT>/lib386/netware/emu387.lib

  [ IFDEF (os_rdos "") <2*> ]
    <CPCMD> 386/stub386/emu387.lib <OWRELROOT>/lib386/rdos/emu387.lib

  [ ENDIF ]

  [ IFDEF (os_dos os_win os_os2 "") <2*> ]
    <CPCMD> i86/stubi86/noemu87.lib  <OWRELROOT>/lib286/noemu87.lib

  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> i86/dosi86/emu87.lib     <OWRELROOT>/lib286/dos/emu87.lib

  [ IFDEF (os_win "") <2*> ]
    <CPCMD> i86/wini86/emu87.lib     <OWRELROOT>/lib286/win/emu87.lib

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> i86/os2i86/emu87.lib     <OWRELROOT>/lib286/os2/emu87.lib

# Libs not built by default
    <CCCMD> 386/stub386/emu387.lib <OWRELROOT>/lib386/qnx/emu387.lib
    <CCCMD> 386/qnx386/emu387      <OWRELROOT>/qnx/binq/emu387

    <CCCMD> i86/stubi86/noemu87.lib  <OWRELROOT>/lib286/qnx/emu87.lib
    <CCCMD> i86/qnxi86/emu86         <OWRELROOT>/qnx/binq/emu86
    <CCCMD> i86/qnxi86/emu86_16      <OWRELROOT>/qnx/binq/emu86_16
    <CCCMD> i86/qnxi86/emu86_32      <OWRELROOT>/qnx/binq/emu86_32

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
