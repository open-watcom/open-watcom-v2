# EMU86 Builder Control file
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
  [ IFDEF (os_dos os_win os_os2 "") <2*> ]
    <CPCMD> <DEVDIR>/emu86/stubi86/noemu87.lib  <RELROOT>/lib286/noemu87.lib

  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> <DEVDIR>/emu86/dosi86/emu87.lib     <RELROOT>/lib286/dos/emu87.lib

  [ IFDEF (os_win "") <2*> ]
    <CPCMD> <DEVDIR>/emu86/wini86/emu87.lib     <RELROOT>/lib286/win/emu87.lib

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> <DEVDIR>/emu86/os2i86/emu87.lib     <RELROOT>/lib286/os2/emu87.lib

  [ IFDEF (os_qnx) <2*> ]
    <CPCMD> <DEVDIR>/emu86/stubi86/noemu87.lib  <RELROOT>/lib286/qnx/emu87.lib
    <CPCMD> <DEVDIR>/emu86/qnxi86/emu86         <RELROOT>/qnx/binq/emu86
    <CPCMD> <DEVDIR>/emu86/qnxi86/emu86_16      <RELROOT>/qnx/binq/emu86_16
    <CPCMD> <DEVDIR>/emu86/qnxi86/emu86_32      <RELROOT>/qnx/binq/emu86_32

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
