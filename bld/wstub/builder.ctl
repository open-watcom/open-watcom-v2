# WSTUB Builder Control file
# ==========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> boot ]
#=================
    <CPCMD> <OWOBJDIR>/wstub.exe          <OWBINDIR>/wstub<CMDEXT>

[ BLOCK <1> bootclean ]
#======================
    echo rm -f <OWBINDIR>/wstub<CMDEXT>
    rm -f <OWBINDIR>/wstub<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
  [ IFDEF (all build os_dos os_win cpu_i86 cpu_386 "") <2*> ]
    <CPCMD> wstub.exe   <OWRELROOT>/binw/wstub.exe
    <CPCMD> wstubq.exe  <OWRELROOT>/binw/wstubq.exe
    <CPCMD> wstub.c     <OWRELROOT>/src/wstub.c

[ BLOCK . . ]
#============
cdsay <PROJDIR>
