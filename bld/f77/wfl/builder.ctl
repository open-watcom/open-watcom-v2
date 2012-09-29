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
    <CCCMD> i86/dosi86/wfl.exe          <OWRELROOT>/binw/wfl.exe
    <CCCMD> i86/dosi86/wfl.sym          <OWRELROOT>/binw/wfl.sym
    <CCCMD> 386/dosi86/wfl386.exe       <OWRELROOT>/binw/wfl386.exe
    <CCCMD> 386/dosi86/wfl386.sym       <OWRELROOT>/binw/wfl386.sym

    <CCCMD> i86/os2386/wfl.exe          <OWRELROOT>/binp/wfl.exe
    <CCCMD> i86/os2386/wfl.sym          <OWRELROOT>/binp/wfl.sym
    <CCCMD> 386/os2386/wfl386.exe       <OWRELROOT>/binp/wfl386.exe
    <CCCMD> 386/os2386/wfl386.sym       <OWRELROOT>/binp/wfl386.sym
 # I wonder if these builds should be enabled to prevent bit rot. They seem to build fine. Mat Nieuwenhoven
#    <CCCMD> axp/os2386/wflaxp.exe       <OWRELROOT>/binp/wflaxp.exe

    <CCCMD> i86/nt386/wfl.exe           <OWRELROOT>/binnt/wfl.exe
    <CCCMD> i86/nt386/wfl.sym           <OWRELROOT>/binnt/wfl.sym
    <CCCMD> 386/nt386/wfl386.exe        <OWRELROOT>/binnt/wfl386.exe
    <CCCMD> 386/nt386/wfl386.sym        <OWRELROOT>/binnt/wfl386.sym
 # I wonder if these builds should be enabled to prevent bit rot. They seem to build fine. Mat Nieuwenhoven
#    <CCCMD> axp/nt386/wflaxp.exe        <OWRELROOT>/binnt/wflaxp.exe

    <CCCMD> i86/linux386/wfl.exe        <OWRELROOT>/binl/wfl
    <CCCMD> i86/linux386/wfl.sym        <OWRELROOT>/binl/wfl.sym
    <CCCMD> 386/linux386/wfl386.exe     <OWRELROOT>/binl/wfl386
    <CCCMD> 386/linux386/wfl386.sym     <OWRELROOT>/binl/wfl386.sym

# axp compilers and link utilities (NT)
    <CCCMD> axp/ntaxp/wflaxp.exe        <OWRELROOT>/axpnt/wflaxp.exe

[ BLOCK . . ]
#==================
cdsay <PROJDIR>
