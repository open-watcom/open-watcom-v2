# wcl Builder Control file
# ========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/bat/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
#=======================
    echo wsplice -k Pspecs <OWROOT>/bld/wl/specs.sp specs.owc
    wsplice -k Pspecs <OWROOT>/bld/wl/specs.sp specs.owc
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> rel2 cprel2 acprel2 ]
#================================

    <CPCMD> specs.owc                 <RELROOT>/binw/specs.owc
    
  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> owcc/dosi86/owcc.exe      <RELROOT>/binw/owcc.exe
    <CPCMD> i86/dosi86/wcl.exe        <RELROOT>/binw/wcl.exe
    <CPCMD> 386/dosi86/wcl386.exe     <RELROOT>/binw/wcl386.exe
    <CPCMD> axp/dosi86/wclaxp.exe     <RELROOT>/binw/wclaxp.exe
    <CPCMD> ppc/dosi86/wclppc.exe     <RELROOT>/binw/wclppc.exe

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> owcc/nt386/owcc.exe       <RELROOT>/binnt/owcc.exe
    <CPCMD> i86/nt386/wcl.exe         <RELROOT>/binnt/wcl.exe
    <CPCMD> 386/nt386/wcl386.exe      <RELROOT>/binnt/wcl386.exe
    <CPCMD> axp/nt386/wclaxp.exe      <RELROOT>/binnt/wclaxp.exe
    <CPCMD> ppc/nt386/wclppc.exe      <RELROOT>/binnt/wclppc.exe
    <CPCMD> mps/nt386/wclmps.exe      <RELROOT>/binnt/wclmps.exe

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> owcc/os2386/owcc.exe      <RELROOT>/binp/owcc.exe
    <CPCMD> i86/os2386/wcl.exe        <RELROOT>/binp/wcl.exe
    <CPCMD> 386/os2386/wcl386.exe     <RELROOT>/binp/wcl386.exe
    <CPCMD> axp/os2386/wclaxp.exe     <RELROOT>/binp/wclaxp.exe
    <CPCMD> ppc/os2386/wclppc.exe     <RELROOT>/binp/wclppc.exe
    <CPCMD> mps/os2386/wclmps.exe     <RELROOT>/binp/wclmps.exe

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> specs.owc                 <RELROOT>/binl/specs.owc
    <CPCMD> owcc/linux386/owcc.exe    <RELROOT>/binl/owcc
    <CPCMD> i86/linux386/wcl.exe      <RELROOT>/binl/wcl
    <CPCMD> i86/linux386/wcl.sym      <RELROOT>/binl/wcl.sym
    <CPCMD> 386/linux386/wcl386.exe   <RELROOT>/binl/wcl386
    <CPCMD> 386/linux386/wcl386.sym   <RELROOT>/binl/wcl386.sym
    <CPCMD> axp/linux386/wclaxp.exe   <RELROOT>/binl/wclaxp
    <CPCMD> mps/linux386/wclmps.exe   <RELROOT>/binl/wclmps

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> i86/ntaxp/wcl.exe         <RELROOT>/axpnt/wcl.exe
    <CPCMD> 386/ntaxp/wcl386.exe      <RELROOT>/axpnt/wcl386.exe
    <CPCMD> axp/ntaxp/wclaxp.exe      <RELROOT>/axpnt/wclaxp.exe

[ BLOCK <1> clean ]
#==================
    rm  -f specs.owc
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
