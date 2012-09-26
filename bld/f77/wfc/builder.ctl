# WFC Builder Control file
# ========================

set PROJDIR=<CWD>
set PROJNAME=wfc

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/deftool.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================

# intel compilers and link utilities
  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> i86/dos386/wfc.exe          <OWRELROOT>/binw/wfc.exe
    <CPCMD> i86/dos386/wfc.sym          <OWRELROOT>/binw/wfc.sym
    <CPCMD> 386/dos386/wfc386.exe       <OWRELROOT>/binw/wfc386.exe
    <CPCMD> 386/dos386/wfc386.sym       <OWRELROOT>/binw/wfc386.sym

  [ IFDEF (os_win "") <2*> ]

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> i86/os2386/wfc.exe          <OWRELROOT>/binp/wfc.exe
    <CPCMD> i86/os2386/wfc.sym          <OWRELROOT>/binp/wfc.sym
    <CPCMD> 386/os2386/wfc386.exe       <OWRELROOT>/binp/wfc386.exe
    <CPCMD> 386/os2386/wfc386.sym       <OWRELROOT>/binp/wfc386.sym
 # I wonder if these builds should be enabled to prevent bit rot. They seem to build fine. Mat Nieuwenhoven
#    <CPCMD> axp/os2386/wfcaxp.exe       <OWRELROOT>/binp/wfcaxp.exe

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> i86/nt386/wfc.exe           <OWRELROOT>/binnt/wfc.exe
    <CPCMD> i86/nt386/wfc.sym           <OWRELROOT>/binnt/wfc.sym
    <CPCMD> 386/nt386/wfc386.exe        <OWRELROOT>/binnt/wfc386.exe
    <CPCMD> 386/nt386/wfc386.sym        <OWRELROOT>/binnt/wfc386.sym
 # I wonder if these builds should be enabled to prevent bit rot. They seem to build fine. Mat Nieuwenhoven
#    <CPCMD> axp/nt386/wfcaxp.exe        <OWRELROOT>/binnt/wfcaxp.exe

  [ IFDEF (os_nov "") <2*> ]

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> i86/linux386/wfc.exe        <OWRELROOT>/binl/wfc
    <CPCMD> i86/linux386/wfc.sym        <OWRELROOT>/binl/wfc.sym
    <CPCMD> 386/linux386/wfc386.exe     <OWRELROOT>/binl/wfc386
    <CPCMD> 386/linux386/wfc386.sym     <OWRELROOT>/binl/wfc386.sym

# axp compilers and link utilities (NT)
  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> axp/ntaxp/wfcaxp.exe        <OWRELROOT>/axpnt/wfcaxp.exe

[ BLOCK . . ]
#==================
cdsay <PROJDIR>
