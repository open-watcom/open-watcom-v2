# i86 FPU emulator Builder Control file
# =====================================

set PROJDIR=<CWD>
set PROJNAME=FPU emu i86

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel cprel ]
#======================
  [ IFDEF (os_dos os_win os_os2 "") <2*> ]
    <CPCMD> stubi86/noemu87.lib  <OWRELROOT>/lib286/noemu87.lib

  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> dosi86/emu87.lib     <OWRELROOT>/lib286/dos/emu87.lib

  [ IFDEF (os_win "") <2*> ]
    <CPCMD> wini86/emu87.lib     <OWRELROOT>/lib286/win/emu87.lib

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> os2i86/emu87.lib     <OWRELROOT>/lib286/os2/emu87.lib

# Libs not built by default
    <CCCMD> stubi86/noemu87.lib  <OWRELROOT>/lib286/qnx/emu87.lib
    <CCCMD> qnxi86/emu86         <OWRELROOT>/qnx/binq/emu86
    <CCCMD> qnxi86/emu86_16      <OWRELROOT>/qnx/binq/emu86_16
    <CCCMD> qnxi86/emu86_32      <OWRELROOT>/qnx/binq/emu86_32

[ BLOCK . . ]
#============
cdsay <PROJDIR>
