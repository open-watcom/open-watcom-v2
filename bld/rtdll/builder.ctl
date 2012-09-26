# Builder Control file
# ==========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
#    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
  [ IFDEF (os_os2 "") <2*> ]
#    <CPCMD> wrtlib/os2.386/mf_r/wr*.lib         <OWRELROOT>/lib386/os2/
#    <CPCMD> wrtlib/os2.386/mf_r/wr*.dll         <OWRELROOT>/binp/dll/
#    <CPCMD> wrtlib/os2.386/mf_r/wr*.sym         <OWRELROOT>/binp/dll/
#    <CPCMD> wrtlib/os2.386/mf_s/wr*.lib         <OWRELROOT>/lib386/os2/
#    <CPCMD> wrtlib/os2.386/mf_s/wr*.dll         <OWRELROOT>/binp/dll/
#    <CPCMD> wrtlib/os2.386/mf_s/wr*.sym         <OWRELROOT>/binp/dll/

  [ IFDEF (os_nt "") <2*> ]
#    <CPCMD> wrtlib/winnt.386/mf_r/wr*.lib       <OWRELROOT>/lib386/nt/
#    <CPCMD> wrtlib/winnt.386/mf_r/wr*.dll       <OWRELROOT>/binnt/
#    <CPCMD> wrtlib/winnt.386/mf_r/wr*.sym       <OWRELROOT>/binnt/
#    <CPCMD> wrtlib/winnt.386/mf_s/wr*.lib       <OWRELROOT>/lib386/nt/
#    <CPCMD> wrtlib/winnt.386/mf_s/wr*.dll       <OWRELROOT>/binnt/
#    <CPCMD> wrtlib/winnt.386/mf_s/wr*.sym       <OWRELROOT>/binnt/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
