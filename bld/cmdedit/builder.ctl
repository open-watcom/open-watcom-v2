# ce Builder Control file
# =======================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel cprel ]
#======================
  <CPCMD> cmdedit.txt           <OWRELROOT>/binw/cmdedit.txt

  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> dosi86/cmdedit.exe  <OWRELROOT>/binw/cmdedit.exe

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> os2i86/os2edit.exe  <OWRELROOT>/binp/os2edit.exe
    <CPCMD> os2i86/os22edit.exe <OWRELROOT>/binp/os22edit.exe
    <CPCMD> os2i86/os2edit.dll  <OWRELROOT>/binp/dll/os2edit.dll

[ BLOCK . . ]
#============
cdsay <PROJDIR>
