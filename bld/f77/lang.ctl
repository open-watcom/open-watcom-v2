# WFC Builder Control file
# ========================

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

# intel compilers and link utilities
  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> wfc/dos386.i86/wfc.exe          <RELROOT>/binw/wfc.exe
    <CPCMD> wfc/dos386.i86/wfc.sym          <RELROOT>/binw/wfc.sym
    <CPCMD> wfc/dos386.386/wfc386.exe       <RELROOT>/binw/wfc386.exe
    <CPCMD> wfc/dos386.386/wfc386.sym       <RELROOT>/binw/wfc386.sym
    <CPCMD> wfl/dosi86.i86/wfl.exe          <RELROOT>/binw/wfl.exe
    <CPCMD> wfl/dosi86.i86/wfl.sym          <RELROOT>/binw/wfl.sym
    <CPCMD> wfl/dosi86.386/wfl386.exe       <RELROOT>/binw/wfl386.exe
    <CPCMD> wfl/dosi86.386/wfl386.sym       <RELROOT>/binw/wfl386.sym

  [ IFDEF (os_win "") <2*> ]

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> wfc/os2386.i86/wfc.exe          <RELROOT>/binp/wfc.exe
    <CPCMD> wfc/os2386.i86/wfc.sym          <RELROOT>/binp/wfc.sym
    <CPCMD> wfc/os2386.386/wfc386.exe       <RELROOT>/binp/wfc386.exe
    <CPCMD> wfc/os2386.386/wfc386.sym       <RELROOT>/binp/wfc386.sym
    <CPCMD> wfl/os2386.i86/wfl.exe          <RELROOT>/binp/wfl.exe
    <CPCMD> wfl/os2386.i86/wfl.sym          <RELROOT>/binp/wfl.sym
    <CPCMD> wfl/os2386.386/wfl386.exe       <RELROOT>/binp/wfl386.exe
    <CPCMD> wfl/os2386.386/wfl386.sym       <RELROOT>/binp/wfl386.sym
 # I wonder if these builds should be enabled to prevent bit rot. They seem to build fine. Mat Nieuwenhoven
#    <CPCMD> wfc/os2386.axp/wfcaxp.exe       <RELROOT>/binp/wfcaxp.exe
#    <CPCMD> wfl/os2386.axp/wflaxp.exe       <RELROOT>/binp/wflaxp.exe

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> wfc/nt386.i86/wfc.exe           <RELROOT>/binnt/wfc.exe
    <CPCMD> wfc/nt386.i86/wfc.sym           <RELROOT>/binnt/wfc.sym
    <CPCMD> wfc/nt386.386/wfc386.exe        <RELROOT>/binnt/wfc386.exe
    <CPCMD> wfc/nt386.386/wfc386.sym        <RELROOT>/binnt/wfc386.sym
    <CPCMD> wfl/nt386.i86/wfl.exe           <RELROOT>/binnt/wfl.exe
    <CPCMD> wfl/nt386.i86/wfl.sym           <RELROOT>/binnt/wfl.sym
    <CPCMD> wfl/nt386.386/wfl386.exe        <RELROOT>/binnt/wfl386.exe
    <CPCMD> wfl/nt386.386/wfl386.sym        <RELROOT>/binnt/wfl386.sym
 # I wonder if these builds should be enabled to prevent bit rot. They seem to build fine. Mat Nieuwenhoven
#    <CPCMD> wfc/nt386.axp/wfcaxp.exe        <RELROOT>/binnt/wfcaxp.exe
#    <CPCMD> wfl/nt386.axp/wflaxp.exe        <RELROOT>/binnt/wflaxp.exe

  [ IFDEF (os_nov "") <2*> ]

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> wfc/linux386.i86/wfc.exe        <RELROOT>/binl/wfc
    <CPCMD> wfc/linux386.i86/wfc.sym        <RELROOT>/binl/wfc.sym
    <CPCMD> wfc/linux386.386/wfc386.exe     <RELROOT>/binl/wfc386
    <CPCMD> wfc/linux386.386/wfc386.sym     <RELROOT>/binl/wfc386.sym
    <CPCMD> wfl/linux386.i86/wfl.exe        <RELROOT>/binl/wfl
    <CPCMD> wfl/linux386.i86/wfl.sym        <RELROOT>/binl/wfl.sym
    <CPCMD> wfl/linux386.386/wfl386.exe     <RELROOT>/binl/wfl386
    <CPCMD> wfl/linux386.386/wfl386.sym     <RELROOT>/binl/wfl386.sym

# axp compilers and link utilities (NT)
  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> wfc/ntaxp.axp/wfcaxp.exe        <RELROOT>/axpnt/wfcaxp.exe
    <CPCMD> wfl/ntaxp.axp/wflaxp.exe        <RELROOT>/axpnt/wflaxp.exe

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#==================

cdsay <PROJDIR>
