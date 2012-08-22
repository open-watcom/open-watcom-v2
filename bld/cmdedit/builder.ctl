# ce Builder Control file
# =======================

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

[ BLOCK <1> rel2 cprel2 ]
#========================
  <CPCMD> cmdedit.txt           <OWRELROOT>/binw/cmdedit.txt

  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> dosi86/cmdedit.exe  <OWRELROOT>/binw/cmdedit.exe

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> os2i86/os2edit.exe  <OWRELROOT>/binp/os2edit.exe
    <CPCMD> os2i86/os22edit.exe <OWRELROOT>/binp/os22edit.exe
    <CPCMD> os2i86/os2edit.dll  <OWRELROOT>/binp/dll/os2edit.dll

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
