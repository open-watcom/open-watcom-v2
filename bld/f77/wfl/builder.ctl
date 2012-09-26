# WFC Builder Control file
# ========================

set PROJDIR=<CWD>
set PROJNAME=wfl

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/deftool.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================

# intel compilers and link utilities
  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> i86/dosi86/wfl.exe          <OWRELROOT>/binw/wfl.exe
    <CPCMD> i86/dosi86/wfl.sym          <OWRELROOT>/binw/wfl.sym
    <CPCMD> 386/dosi86/wfl386.exe       <OWRELROOT>/binw/wfl386.exe
    <CPCMD> 386/dosi86/wfl386.sym       <OWRELROOT>/binw/wfl386.sym

  [ IFDEF (os_win "") <2*> ]

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> i86/os2386/wfl.exe          <OWRELROOT>/binp/wfl.exe
    <CPCMD> i86/os2386/wfl.sym          <OWRELROOT>/binp/wfl.sym
    <CPCMD> 386/os2386/wfl386.exe       <OWRELROOT>/binp/wfl386.exe
    <CPCMD> 386/os2386/wfl386.sym       <OWRELROOT>/binp/wfl386.sym
 # I wonder if these builds should be enabled to prevent bit rot. They seem to build fine. Mat Nieuwenhoven
#    <CPCMD> axp/os2386/wflaxp.exe       <OWRELROOT>/binp/wflaxp.exe

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> i86/nt386/wfl.exe           <OWRELROOT>/binnt/wfl.exe
    <CPCMD> i86/nt386/wfl.sym           <OWRELROOT>/binnt/wfl.sym
    <CPCMD> 386/nt386/wfl386.exe        <OWRELROOT>/binnt/wfl386.exe
    <CPCMD> 386/nt386/wfl386.sym        <OWRELROOT>/binnt/wfl386.sym
 # I wonder if these builds should be enabled to prevent bit rot. They seem to build fine. Mat Nieuwenhoven
#    <CPCMD> axp/nt386/wflaxp.exe        <OWRELROOT>/binnt/wflaxp.exe

  [ IFDEF (os_nov "") <2*> ]

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> i86/linux386/wfl.exe        <OWRELROOT>/binl/wfl
    <CPCMD> i86/linux386/wfl.sym        <OWRELROOT>/binl/wfl.sym
    <CPCMD> 386/linux386/wfl386.exe     <OWRELROOT>/binl/wfl386
    <CPCMD> 386/linux386/wfl386.sym     <OWRELROOT>/binl/wfl386.sym

# axp compilers and link utilities (NT)
  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> axp/ntaxp/wflaxp.exe        <OWRELROOT>/axpnt/wflaxp.exe

[ BLOCK . . ]
#==================
cdsay <PROJDIR>
