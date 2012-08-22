# WIN386 Builder Control file
# ===========================

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
  [ IFDEF (os_win "") <2*> ]
    <CPCMD> ext/win386.ext              <RELROOT>/binw/win386.ext
    <CPCMD> dll/w386dll.ext             <RELROOT>/binw/w386dll.ext
    <CPCMD> conv/win386/win386.lib      <RELROOT>/lib386/win/win386.lib
    <CPCMD> vxd/wdebug.386              <RELROOT>/binw/wdebug.386
    <CPCMD> vxd/wemu387.386             <RELROOT>/binw/wemu387.386

  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> wbind/dosi86/wbind.exe      <RELROOT>/binw/wbind.exe

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> wbind/os2i86/wbind.exe      <RELROOT>/binp/wbind.exe

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> wbind/nt386/wbind.exe       <RELROOT>/binnt/wbind.exe

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> wbind/linux386/wbind.exe    <RELROOT>/binl/wbind

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean
    @rm -f asm/dllthk.asm c/dllthunk.c asm/winglue.asm h/winglue.inc
    @rm -f ext/win386.ext dll/w386dll.ext
    @rm -f vxd/wdebug.386 vxd/wemu387.386

[ BLOCK . . ]
#============

cdsay <PROJDIR>
