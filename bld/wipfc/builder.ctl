# WIPFC Builder Control file
# ==========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ INCLUDE prereq.ctl ]

cdsay .

[ BLOCK <1> build rel2 ]
#=======================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> rel2 cprel2 ]
#========================
    <CPCMD> helper/*.nls          <RELROOT>/wipfc/
    <CPCMD> helper/*.txt          <RELROOT>/wipfc/

  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> dos386/wipfc.exe      <RELROOT>/binw/wipfc.exe

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> os2386/wipfc.exe      <RELROOT>/binp/wipfc.exe

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/wipfc.exe       <RELROOT>/binnt/wipfc.exe

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> linux386/wipfc.exe    <RELROOT>/binl/wipfc

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
