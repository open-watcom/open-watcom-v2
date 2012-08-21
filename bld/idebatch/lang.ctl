# IDEBAT Builder Control file
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
  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> os2/batserv.exe  <RELROOT>/binp/batserv.exe
  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt/batserv.exe   <RELROOT>/binnt/batserv.exe
  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> dos/int.exe      <RELROOT>/binw/dosserv.exe
# there is no makefile in the wini86 dir
  [ IFDEF (os_win "") <2*> ]
    <CPCMD> wini86/batchbox.pif <RELROOT>/binw/batchbox.pif
  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> nt/axp/batserv.exe  <RELROOT>/axpnt/batserv.exe

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
