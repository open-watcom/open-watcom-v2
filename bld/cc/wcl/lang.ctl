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
    pmake -d buildwcl <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> rel2 cprel2 acprel2 ]
#================================

    <CPCMD> <PROJDIR>/specs.owc                 <RELROOT>/binw/specs.owc
    
  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> <PROJDIR>/dosi86/owcc.exe           <RELROOT>/binw/owcc.exe
    <CPCMD> <PROJDIR>/dosi86.i86/wcl.exe        <RELROOT>/binw/wcl.exe
    <CPCMD> <PROJDIR>/dosi86.386/wcl386.exe     <RELROOT>/binw/wcl386.exe
    <CPCMD> <PROJDIR>/dosi86.axp/wclaxp.exe     <RELROOT>/binw/wclaxp.exe
    <CPCMD> <PROJDIR>/dosi86.ppc/wclppc.exe     <RELROOT>/binw/wclppc.exe

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> <PROJDIR>/nt386/owcc.exe            <RELROOT>/binnt/owcc.exe
    <CPCMD> <PROJDIR>/nt386.i86/wcl.exe         <RELROOT>/binnt/wcl.exe
    <CPCMD> <PROJDIR>/nt386.386/wcl386.exe      <RELROOT>/binnt/wcl386.exe
    <CPCMD> <PROJDIR>/nt386.axp/wclaxp.exe      <RELROOT>/binnt/wclaxp.exe
    <CPCMD> <PROJDIR>/nt386.ppc/wclppc.exe      <RELROOT>/binnt/wclppc.exe
    <CPCMD> <PROJDIR>/nt386.mps/wclmps.exe      <RELROOT>/binnt/wclmps.exe

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> <PROJDIR>/os2386/owcc.exe           <RELROOT>/binp/owcc.exe
    <CPCMD> <PROJDIR>/os2386.i86/wcl.exe        <RELROOT>/binp/wcl.exe
    <CPCMD> <PROJDIR>/os2386.386/wcl386.exe     <RELROOT>/binp/wcl386.exe
    <CPCMD> <PROJDIR>/os2386.axp/wclaxp.exe     <RELROOT>/binp/wclaxp.exe
    <CPCMD> <PROJDIR>/os2386.ppc/wclppc.exe     <RELROOT>/binp/wclppc.exe
    <CPCMD> <PROJDIR>/os2386.mps/wclmps.exe     <RELROOT>/binp/wclmps.exe

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> <PROJDIR>/specs.owc                 <RELROOT>/binl/specs.owc
    <CPCMD> <PROJDIR>/linux386/owcc.exe         <RELROOT>/binl/owcc
    <CPCMD> <PROJDIR>/linux386.i86/wcl.exe      <RELROOT>/binl/wcl
    <CPCMD> <PROJDIR>/linux386.i86/wcl.sym      <RELROOT>/binl/wcl.sym
    <CPCMD> <PROJDIR>/linux386.386/wcl386.exe   <RELROOT>/binl/wcl386
    <CPCMD> <PROJDIR>/linux386.386/wcl386.sym   <RELROOT>/binl/wcl386.sym
    <CPCMD> <PROJDIR>/linux386.axp/wclaxp.exe   <RELROOT>/binl/wclaxp
    <CPCMD> <PROJDIR>/linux386.mps/wclmps.exe   <RELROOT>/binl/wclmps

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> <PROJDIR>/ntaxp.i86/wcl.exe         <RELROOT>/axpnt/wcl.exe
    <CPCMD> <PROJDIR>/ntaxp.386/wcl386.exe      <RELROOT>/axpnt/wcl386.exe
    <CPCMD> <PROJDIR>/ntaxp.axp/wclaxp.exe      <RELROOT>/axpnt/wclaxp.exe

[ BLOCK <1> clean ]
#==================
    rm  -f specs.owc
    pmake -d buildwcl <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
