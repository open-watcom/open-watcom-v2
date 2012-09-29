# wlib Builder Control file
# =========================

set PROJDIR=<CWD>
set PROJNAME=wlib

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK ( <1> <BINTOOL> ) rel ]
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> rel ]
#======================
    <CPCMD> <OWOBJDIR>/bwlib.exe     <OWBINDIR>/bwlib<CMDEXT>
    <CCCMD> <OWOBJDIR>/bwlibd<DYEXT> <OWBINDIR>/bwlibd<DYEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/bwlib<CMDEXT>
    rm -f <OWBINDIR>/bwlib<CMDEXT>
    rm -f <OWBINDIR>/bwlibd<DYEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/wlb.exe      <OWRELROOT>/binw/wlib.exe
    <CCCMD> dos386/wlb.sym      <OWRELROOT>/binw/wlib.sym
    <CCCMD> os2386/wlb.exe      <OWRELROOT>/binp/wlib.exe
    <CCCMD> os2386/wlb.sym      <OWRELROOT>/binp/wlib.sym
    <CCCMD> os2386/wlbd.dll     <OWRELROOT>/binp/dll/wlibd.dll
    <CCCMD> os2386/wlbd.sym     <OWRELROOT>/binp/dll/wlibd.sym
    <CCCMD> nt386/wlb.exe       <OWRELROOT>/binnt/wlib.exe
    <CCCMD> nt386/wlb.sym       <OWRELROOT>/binnt/wlib.sym
    <CCCMD> nt386/wlbd.dll      <OWRELROOT>/binnt/wlibd.dll
    <CCCMD> nt386/wlbd.sym      <OWRELROOT>/binnt/wlibd.sym
    <CCCMD> ntaxp/wlb.exe       <OWRELROOT>/axpnt/wlib.exe
    <CCCMD> ntaxp/wlb.sym       <OWRELROOT>/axpnt/wlib.sym
    <CCCMD> ntaxp/wlbd.dll      <OWRELROOT>/axpnt/wlibd.dll
    <CCCMD> ntaxp/wlbd.sym      <OWRELROOT>/axpnt/wlibd.sym
    <CCCMD> qnx386/wlb.exe      <OWRELROOT>/qnx/binq/wlib
    <CCCMD> qnx386/wlb.sym      <OWRELROOT>/qnx/sym/wlib.sym
    <CCCMD> linux386/wlb.exe    <OWRELROOT>/binl/wlib
    <CCCMD> linux386/wlb.sym    <OWRELROOT>/binl/wlib.sym

[ BLOCK . . ]
#============
cdsay <PROJDIR>
