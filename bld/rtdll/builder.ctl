# Builder Control file
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
  [ IFDEF (os_os2 "") <2*> ]
#    <CPCMD> wrtlib/os2.386/mf_r/wr*.lib         <RELROOT>/lib386/os2/
#    <CPCMD> wrtlib/os2.386/mf_r/wr*.dll         <RELROOT>/binp/dll/
#    <CPCMD> wrtlib/os2.386/mf_r/wr*.sym         <RELROOT>/binp/dll/
#    <CPCMD> wrtlib/os2.386/mf_s/wr*.lib         <RELROOT>/lib386/os2/
#    <CPCMD> wrtlib/os2.386/mf_s/wr*.dll         <RELROOT>/binp/dll/
#    <CPCMD> wrtlib/os2.386/mf_s/wr*.sym         <RELROOT>/binp/dll/

  [ IFDEF (os_nt "") <2*> ]
#    <CPCMD> wrtlib/winnt.386/mf_r/wr*.lib       <RELROOT>/lib386/nt/
#    <CPCMD> wrtlib/winnt.386/mf_r/wr*.dll       <RELROOT>/binnt/
#    <CPCMD> wrtlib/winnt.386/mf_r/wr*.sym       <RELROOT>/binnt/
#    <CPCMD> wrtlib/winnt.386/mf_s/wr*.lib       <RELROOT>/lib386/nt/
#    <CPCMD> wrtlib/winnt.386/mf_s/wr*.dll       <RELROOT>/binnt/
#    <CPCMD> wrtlib/winnt.386/mf_s/wr*.sym       <RELROOT>/binnt/

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
