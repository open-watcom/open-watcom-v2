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
    <CPCMD> wfc/i86/dos386/wfc.exe          <OWRELROOT>/binw/wfc.exe
    <CPCMD> wfc/i86/dos386/wfc.sym          <OWRELROOT>/binw/wfc.sym
    <CPCMD> wfc/386/dos386/wfc386.exe       <OWRELROOT>/binw/wfc386.exe
    <CPCMD> wfc/386/dos386/wfc386.sym       <OWRELROOT>/binw/wfc386.sym
    <CPCMD> wfl/i86/dosi86/wfl.exe          <OWRELROOT>/binw/wfl.exe
    <CPCMD> wfl/i86/dosi86/wfl.sym          <OWRELROOT>/binw/wfl.sym
    <CPCMD> wfl/386/dosi86/wfl386.exe       <OWRELROOT>/binw/wfl386.exe
    <CPCMD> wfl/386/dosi86/wfl386.sym       <OWRELROOT>/binw/wfl386.sym

  [ IFDEF (os_win "") <2*> ]

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> wfc/i86/os2386/wfc.exe          <OWRELROOT>/binp/wfc.exe
    <CPCMD> wfc/i86/os2386/wfc.sym          <OWRELROOT>/binp/wfc.sym
    <CPCMD> wfc/386/os2386/wfc386.exe       <OWRELROOT>/binp/wfc386.exe
    <CPCMD> wfc/386/os2386/wfc386.sym       <OWRELROOT>/binp/wfc386.sym
    <CPCMD> wfl/i86/os2386/wfl.exe          <OWRELROOT>/binp/wfl.exe
    <CPCMD> wfl/i86/os2386/wfl.sym          <OWRELROOT>/binp/wfl.sym
    <CPCMD> wfl/386/os2386/wfl386.exe       <OWRELROOT>/binp/wfl386.exe
    <CPCMD> wfl/386/os2386/wfl386.sym       <OWRELROOT>/binp/wfl386.sym
 # I wonder if these builds should be enabled to prevent bit rot. They seem to build fine. Mat Nieuwenhoven
#    <CPCMD> wfc/axp/os2386/wfcaxp.exe       <OWRELROOT>/binp/wfcaxp.exe
#    <CPCMD> wfl/axp/os2386/wflaxp.exe       <OWRELROOT>/binp/wflaxp.exe

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> wfc/i86/nt386/wfc.exe           <OWRELROOT>/binnt/wfc.exe
    <CPCMD> wfc/i86/nt386/wfc.sym           <OWRELROOT>/binnt/wfc.sym
    <CPCMD> wfc/386/nt386/wfc386.exe        <OWRELROOT>/binnt/wfc386.exe
    <CPCMD> wfc/386/nt386/wfc386.sym        <OWRELROOT>/binnt/wfc386.sym
    <CPCMD> wfl/i86/nt386/wfl.exe           <OWRELROOT>/binnt/wfl.exe
    <CPCMD> wfl/i86/nt386/wfl.sym           <OWRELROOT>/binnt/wfl.sym
    <CPCMD> wfl/386/nt386/wfl386.exe        <OWRELROOT>/binnt/wfl386.exe
    <CPCMD> wfl/386/nt386/wfl386.sym        <OWRELROOT>/binnt/wfl386.sym
 # I wonder if these builds should be enabled to prevent bit rot. They seem to build fine. Mat Nieuwenhoven
#    <CPCMD> wfc/axp/nt386/wfcaxp.exe        <OWRELROOT>/binnt/wfcaxp.exe
#    <CPCMD> wfl/axp/nt386/wflaxp.exe        <OWRELROOT>/binnt/wflaxp.exe

  [ IFDEF (os_nov "") <2*> ]

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> wfc/i86/linux386/wfc.exe        <OWRELROOT>/binl/wfc
    <CPCMD> wfc/i86/linux386/wfc.sym        <OWRELROOT>/binl/wfc.sym
    <CPCMD> wfc/386/linux386/wfc386.exe     <OWRELROOT>/binl/wfc386
    <CPCMD> wfc/386/linux386/wfc386.sym     <OWRELROOT>/binl/wfc386.sym
    <CPCMD> wfl/i86/linux386/wfl.exe        <OWRELROOT>/binl/wfl
    <CPCMD> wfl/i86/linux386/wfl.sym        <OWRELROOT>/binl/wfl.sym
    <CPCMD> wfl/386/linux386/wfl386.exe     <OWRELROOT>/binl/wfl386
    <CPCMD> wfl/386/linux386/wfl386.sym     <OWRELROOT>/binl/wfl386.sym

# axp compilers and link utilities (NT)
  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> wfc/axp/ntaxp/wfcaxp.exe        <OWRELROOT>/axpnt/wfcaxp.exe
    <CPCMD> wfl/axp/ntaxp/wflaxp.exe        <OWRELROOT>/axpnt/wflaxp.exe

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#==================

cdsay <PROJDIR>
