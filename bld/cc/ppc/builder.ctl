# wccppc Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=wccppc

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/bwccppc.exe     <OWBINDIR>/bwccppc<CMDEXT>
    <CCCMD> <OWOBJDIR>/bwccdppc<DYEXT> <OWBINDIR>/bwccdppc<DYEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/bwccppc<CMDEXT>
    rm -f <OWBINDIR>/bwccppc<CMDEXT>
    rm -f <OWBINDIR>/bwccdppc<DYEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/wccppc.exe       <OWRELROOT>/binw/
    <CCCMD> dos386/wccppc.sym       <OWRELROOT>/binw/
    <CCCMD> dos386/wccppc01.int     <OWRELROOT>/binw/
    <CCCMD> os2386.dll/wccppc.exe   <OWRELROOT>/binp/
    <CCCMD> os2386.dll/wccppc.sym   <OWRELROOT>/binp/
    <CCCMD> os2386.dll/wccdppc.dll  <OWRELROOT>/binp/dll/
    <CCCMD> os2386.dll/wccdppc.sym  <OWRELROOT>/binp/dll/
    <CCCMD> os2386.dll/wccppc01.int <OWRELROOT>/binp/dll/
    <CCCMD> nt386.dll/wccppc.exe    <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/wccppc.sym    <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/wccdppc.dll   <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/wccdppc.sym   <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/wccppc01.int  <OWRELROOT>/binnt/
    <CCCMD> ntaxp.dll/wccppc.exe    <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/wccppc.sym    <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/wccdppc.dll   <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/wccdppc.sym   <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/wccppc01.int  <OWRELROOT>/axpnt/
    <CCCMD> qnx386/wccppc.exe       <OWRELROOT>/qnx/binq/wccppc
    <CCCMD> qnx386/wccppc.sym       <OWRELROOT>/qnx/sym/
    <CCCMD> qnx386/wccppc01.int     <OWRELROOT>/qnx/binq/
    <CCCMD> linux386/wccppc.exe     <OWRELROOT>/binl/wccppc
    <CCCMD> linux386/wccppc.sym     <OWRELROOT>/binl/
    <CCCMD> linux386/wccppc01.int   <OWRELROOT>/binl/

    <CCCMD> ntx64.dll/wccppc.exe    <OWRELROOT>/binnt64/
    <CCCMD> ntx64.dll/wccdppc.dll   <OWRELROOT>/binnt64/
    <CCCMD> ntx64.dll/wccppc01.int  <OWRELROOT>/binnt64/
    <CCCMD> linuxx64/wccppc.exe     <OWRELROOT>/binl64/wccppc
    <CCCMD> linuxx64/wccppc01.int   <OWRELROOT>/binl64/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
