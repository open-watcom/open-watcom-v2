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
    <CCCMD> i86/dosi86/wfl.exe          <OWRELROOT>/binw/
    <CCCMD> i86/dosi86/wfl.sym          <OWRELROOT>/binw/
    <CCCMD> 386/dosi86/wfl386.exe       <OWRELROOT>/binw/
    <CCCMD> 386/dosi86/wfl386.sym       <OWRELROOT>/binw/

    <CCCMD> i86/os2386/wfl.exe          <OWRELROOT>/binp/
    <CCCMD> i86/os2386/wfl.sym          <OWRELROOT>/binp/
    <CCCMD> 386/os2386/wfl386.exe       <OWRELROOT>/binp/
    <CCCMD> 386/os2386/wfl386.sym       <OWRELROOT>/binp/
 # I wonder if these builds should be enabled to prevent bit rot. They seem to build fine. Mat Nieuwenhoven
#    <CCCMD> axp/os2386/wflaxp.exe       <OWRELROOT>/binp/

    <CCCMD> i86/nt386/wfl.exe           <OWRELROOT>/binnt/
    <CCCMD> i86/nt386/wfl.sym           <OWRELROOT>/binnt/
    <CCCMD> 386/nt386/wfl386.exe        <OWRELROOT>/binnt/
    <CCCMD> 386/nt386/wfl386.sym        <OWRELROOT>/binnt/
 # I wonder if these builds should be enabled to prevent bit rot. They seem to build fine. Mat Nieuwenhoven
#    <CCCMD> axp/nt386/wflaxp.exe        <OWRELROOT>/binnt/

    <CCCMD> i86/linux386/wfl.exe        <OWRELROOT>/binl/wfl
    <CCCMD> i86/linux386/wfl.sym        <OWRELROOT>/binl/
    <CCCMD> 386/linux386/wfl386.exe     <OWRELROOT>/binl/wfl386
    <CCCMD> 386/linux386/wfl386.sym     <OWRELROOT>/binl/

# axp compilers and link utilities (NT)
    <CCCMD> axp/ntaxp/wflaxp.exe        <OWRELROOT>/axpnt/

    <CCCMD> i86/ntx64/wfl.exe           <OWRELROOT>/binnt64/
    <CCCMD> 386/ntx64/wfl386.exe        <OWRELROOT>/binnt64/
    <CCCMD> axp/ntx64/wflaxp.exe        <OWRELROOT>/binnt64/

[ BLOCK . . ]
#==================
cdsay <PROJDIR>
