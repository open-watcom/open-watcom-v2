# wppaxp Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=wppaxp

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/bwppaxp.exe     <OWBINDIR>/bwppaxp<CMDEXT>
    <CCCMD> <OWOBJDIR>/bwppdaxp<DYEXT> <OWBINDIR>/bwppdaxp<DYEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/bwppaxp<CMDEXT>
    rm -f <OWBINDIR>/bwppaxp<CMDEXT>
    rm -f <OWBINDIR>/bwppdaxp<DYEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/wppaxp.exe       <OWRELROOT>/binw/
    <CCCMD> dos386/wppaxp.sym       <OWRELROOT>/binw/
    <CCCMD> dos386/wppaxp01.int     <OWRELROOT>/binw/
    <CCCMD> os2386.dll/wppaxp.exe   <OWRELROOT>/binp/
    <CCCMD> os2386.dll/wppaxp.sym   <OWRELROOT>/binp/
    <CCCMD> os2386.dll/wppdaxp.dll  <OWRELROOT>/binp/dll/
    <CCCMD> os2386.dll/wppdaxp.sym  <OWRELROOT>/binp/dll/
    <CCCMD> os2386.dll/wppaxp01.int <OWRELROOT>/binp/dll/
    <CCCMD> nt386.dll/wppaxp.exe    <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/wppaxp.sym    <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/wppdaxp.dll   <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/wppdaxp.sym   <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/wppaxp01.int  <OWRELROOT>/binnt/
    <CCCMD> ntaxp.dll/wppaxp.exe    <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/wppaxp.sym    <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/wppdaxp.dll   <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/wppdaxp.sym   <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/wppaxp01.int  <OWRELROOT>/axpnt/
    <CCCMD> linux386/wppaxp.exe     <OWRELROOT>/binl/wppaxp
    <CCCMD> linux386/wppaxp.sym     <OWRELROOT>/binl/
    <CCCMD> linux386/wppaxp01.int   <OWRELROOT>/binl/

    <CCCMD> ntx64.dll/wppaxp.exe    <OWRELROOT>/binnt64/
    <CCCMD> ntx64.dll/wppdaxp.dll   <OWRELROOT>/binnt64/
    <CCCMD> ntx64.dll/wppaxp01.int  <OWRELROOT>/binnt64/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
