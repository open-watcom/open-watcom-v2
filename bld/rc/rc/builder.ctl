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

#    <CPCMD> osi386/wrc.exe      <OWRELROOT>/binw/
    <CCCMD> dos386/wrc.exe      <OWRELROOT>/binw/
    <CCCMD> dos386/wrc.sym      <OWRELROOT>/binw/
    <CCCMD> os2386/wrc.exe      <OWRELROOT>/binp/
    <CCCMD> os2386/wrc.sym      <OWRELROOT>/binp/
    <CCCMD> os2386/wrcd.dll     <OWRELROOT>/binp/dll/
    <CCCMD> os2386/wrcd.sym     <OWRELROOT>/binp/dll/
    <CCCMD> nt386/wrc.exe       <OWRELROOT>/binnt/
    <CCCMD> nt386/wrc.sym       <OWRELROOT>/binnt/
    <CCCMD> nt386/wrcd.dll      <OWRELROOT>/binnt/
    <CCCMD> nt386/wrcd.sym      <OWRELROOT>/binnt/
    <CCCMD> ntaxp/wrc.exe       <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wrc.sym       <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wrcd.dll      <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wrcd.sym      <OWRELROOT>/axpnt/
    <CCCMD> linux386/wrc.exe    <OWRELROOT>/binl/wrc
    <CCCMD> linux386/wrc.sym    <OWRELROOT>/binl/

    <CCCMD> linuxx64/wrc.exe    <OWRELROOT>/binl64/wrc
    <CCCMD> ntx64/wrc.exe       <OWRELROOT>/binnt64/
    <CCCMD> ntx64/wrcd.dll      <OWRELROOT>/binnt64/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
