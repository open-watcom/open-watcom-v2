# WFC Builder Control file
# ========================

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

# intel compilers and link utilities
  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> wfc/dos386.i86/wfc.exe          <OWRELROOT>/binw/wfc.exe
    <CPCMD> wfc/dos386.i86/wfc.sym          <OWRELROOT>/binw/wfc.sym
    <CPCMD> wfc/dos386.386/wfc386.exe       <OWRELROOT>/binw/wfc386.exe
    <CPCMD> wfc/dos386.386/wfc386.sym       <OWRELROOT>/binw/wfc386.sym
    <CPCMD> wfl/dosi86.i86/wfl.exe          <OWRELROOT>/binw/wfl.exe
    <CPCMD> wfl/dosi86.i86/wfl.sym          <OWRELROOT>/binw/wfl.sym
    <CPCMD> wfl/dosi86.386/wfl386.exe       <OWRELROOT>/binw/wfl386.exe
    <CPCMD> wfl/dosi86.386/wfl386.sym       <OWRELROOT>/binw/wfl386.sym

  [ IFDEF (os_win "") <2*> ]

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> wfc/os2386.i86/wfc.exe          <OWRELROOT>/binp/wfc.exe
    <CPCMD> wfc/os2386.i86/wfc.sym          <OWRELROOT>/binp/wfc.sym
    <CPCMD> wfc/os2386.386/wfc386.exe       <OWRELROOT>/binp/wfc386.exe
    <CPCMD> wfc/os2386.386/wfc386.sym       <OWRELROOT>/binp/wfc386.sym
    <CPCMD> wfl/os2386.i86/wfl.exe          <OWRELROOT>/binp/wfl.exe
    <CPCMD> wfl/os2386.i86/wfl.sym          <OWRELROOT>/binp/wfl.sym
    <CPCMD> wfl/os2386.386/wfl386.exe       <OWRELROOT>/binp/wfl386.exe
    <CPCMD> wfl/os2386.386/wfl386.sym       <OWRELROOT>/binp/wfl386.sym
 # I wonder if these builds should be enabled to prevent bit rot. They seem to build fine. Mat Nieuwenhoven
#    <CPCMD> wfc/os2386.axp/wfcaxp.exe       <OWRELROOT>/binp/wfcaxp.exe
#    <CPCMD> wfl/os2386.axp/wflaxp.exe       <OWRELROOT>/binp/wflaxp.exe

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> wfc/nt386.i86/wfc.exe           <OWRELROOT>/binnt/wfc.exe
    <CPCMD> wfc/nt386.i86/wfc.sym           <OWRELROOT>/binnt/wfc.sym
    <CPCMD> wfc/nt386.386/wfc386.exe        <OWRELROOT>/binnt/wfc386.exe
    <CPCMD> wfc/nt386.386/wfc386.sym        <OWRELROOT>/binnt/wfc386.sym
    <CPCMD> wfl/nt386.i86/wfl.exe           <OWRELROOT>/binnt/wfl.exe
    <CPCMD> wfl/nt386.i86/wfl.sym           <OWRELROOT>/binnt/wfl.sym
    <CPCMD> wfl/nt386.386/wfl386.exe        <OWRELROOT>/binnt/wfl386.exe
    <CPCMD> wfl/nt386.386/wfl386.sym        <OWRELROOT>/binnt/wfl386.sym
 # I wonder if these builds should be enabled to prevent bit rot. They seem to build fine. Mat Nieuwenhoven
#    <CPCMD> wfc/nt386.axp/wfcaxp.exe        <OWRELROOT>/binnt/wfcaxp.exe
#    <CPCMD> wfl/nt386.axp/wflaxp.exe        <OWRELROOT>/binnt/wflaxp.exe

  [ IFDEF (os_nov "") <2*> ]

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> wfc/linux386.i86/wfc.exe        <OWRELROOT>/binl/wfc
    <CPCMD> wfc/linux386.i86/wfc.sym        <OWRELROOT>/binl/wfc.sym
    <CPCMD> wfc/linux386.386/wfc386.exe     <OWRELROOT>/binl/wfc386
    <CPCMD> wfc/linux386.386/wfc386.sym     <OWRELROOT>/binl/wfc386.sym
    <CPCMD> wfl/linux386.i86/wfl.exe        <OWRELROOT>/binl/wfl
    <CPCMD> wfl/linux386.i86/wfl.sym        <OWRELROOT>/binl/wfl.sym
    <CPCMD> wfl/linux386.386/wfl386.exe     <OWRELROOT>/binl/wfl386
    <CPCMD> wfl/linux386.386/wfl386.sym     <OWRELROOT>/binl/wfl386.sym

# axp compilers and link utilities (NT)
  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> wfc/ntaxp.axp/wfcaxp.exe        <OWRELROOT>/axpnt/wfcaxp.exe
    <CPCMD> wfl/ntaxp.axp/wflaxp.exe        <OWRELROOT>/axpnt/wflaxp.exe

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#==================

cdsay <PROJDIR>
