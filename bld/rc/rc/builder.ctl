# Resource Compiler Builder Control file
# ======================================

set PROJDIR=<CWD>
set PROJNAME=wrc

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/bwrc.exe     <OWBINDIR>/bwrc<CMDEXT>
    <CCCMD> <OWOBJDIR>/bwrcd<DYEXT> <OWBINDIR>/bwrcd<DYEXT>
    <CPCMD> kanji.uni <OWBINDIR>/

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/bwrc<CMDEXT>
    rm -f <OWBINDIR>/bwrc<CMDEXT>
    rm -f <OWBINDIR>/bwrcd<DYEXT>
    echo rm -f <OWBINDIR>/kanji.uni
    rm -f <OWBINDIR>/kanji.uni

[ BLOCK <1> rel cprel ]
#======================
    <CPCMD> *.uni               <OWRELROOT>/binw/
    <CPCMD> *.uni               <OWRELROOT>/binl/

#    <CPCMD> osi386/wrce.exe     <OWRELROOT>/binw/wrc.exe
    <CCCMD> dos386/wrce.exe     <OWRELROOT>/binw/wrc.exe
    <CCCMD> dos386/wrce.sym     <OWRELROOT>/binw/wrc.sym
    <CCCMD> os2386/wrce.exe     <OWRELROOT>/binp/wrc.exe
    <CCCMD> os2386/wrce.sym     <OWRELROOT>/binp/wrc.sym
    <CCCMD> os2386/wrced.dll    <OWRELROOT>/binp/dll/wrcd.dll
    <CCCMD> os2386/wrced.sym    <OWRELROOT>/binp/dll/wrcd.sym
    <CCCMD> nt386/wrce.exe      <OWRELROOT>/binnt/wrc.exe
    <CCCMD> nt386/wrce.sym      <OWRELROOT>/binnt/wrc.sym
    <CCCMD> nt386/wrced.dll     <OWRELROOT>/binnt/wrcd.dll
    <CCCMD> nt386/wrced.sym     <OWRELROOT>/binnt/wrcd.sym
    <CCCMD> ntaxp/wrce.exe      <OWRELROOT>/axpnt/wrc.exe
    <CCCMD> ntaxp/wrce.sym      <OWRELROOT>/axpnt/wrc.sym
    <CCCMD> ntaxp/wrced.dll     <OWRELROOT>/axpnt/wrcd.dll
    <CCCMD> ntaxp/wrced.sym     <OWRELROOT>/axpnt/wrcd.sym
    <CCCMD> linux386/wrce.exe   <OWRELROOT>/binl/wrc
    <CCCMD> linux386/wrce.sym   <OWRELROOT>/binl/wrc.sym

[ BLOCK . . ]
#============
cdsay <PROJDIR>
