# FMEDIT Builder Control file
# ===========================
# This should really be part of the SDK
# it is called from SDK\lang.ctl

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
  [ IFDEF (os_win "") <2*> ]
    <CPCMD> <PROJDIR>/wini86/fmedit.dll <RELROOT>/binw/
  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> <PROJDIR>/nt386/fmedit.dll <RELROOT>/binnt/
  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> <PROJDIR>/ntaxp/fmedit.dll <RELROOT>/axpnt/

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
