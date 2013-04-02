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
    <CCCMD> dos386/wpaxp.exe        <OWRELROOT>/binw/wppaxp.exe
    <CCCMD> dos386/wpaxp.sym        <OWRELROOT>/binw/wppaxp.sym
    <CCCMD> dos386/wppaxp01.int     <OWRELROOT>/binw/
    <CCCMD> os2386/wpaxp.exe        <OWRELROOT>/binp/wppaxp.exe
    <CCCMD> os2386/wpaxp.sym        <OWRELROOT>/binp/wppaxp.sym
    <CCCMD> os2386/wpdaxp.dll       <OWRELROOT>/binp/dll/wppdaxp.dll
    <CCCMD> os2386/wpdaxp.sym       <OWRELROOT>/binp/dll/wppdaxp.sym
    <CCCMD> os2386/wppaxp01.int     <OWRELROOT>/binp/dll/
    <CCCMD> nt386/wpaxp.exe         <OWRELROOT>/binnt/wppaxp.exe
    <CCCMD> nt386/wpaxp.sym         <OWRELROOT>/binnt/wppaxp.sym
    <CCCMD> nt386/wpdaxp.dll        <OWRELROOT>/binnt/wppdaxp.dll
    <CCCMD> nt386/wpdaxp.sym        <OWRELROOT>/binnt/wppdaxp.sym
    <CCCMD> nt386/wppaxp01.int      <OWRELROOT>/binnt/
    <CCCMD> ntaxp/wpaxp.exe         <OWRELROOT>/axpnt/wppaxp.exe
    <CCCMD> ntaxp/wpaxp.sym         <OWRELROOT>/axpnt/wppaxp.sym
    <CCCMD> ntaxp/wpdaxp.dll        <OWRELROOT>/axpnt/wppdaxp.dll
    <CCCMD> ntaxp/wpdaxp.sym        <OWRELROOT>/axpnt/wppdaxp.sym
    <CCCMD> ntaxp/wppaxp01.int      <OWRELROOT>/axpnt/
    <CCCMD> linux386/wpaxp.exe      <OWRELROOT>/binl/wppaxp
    <CCCMD> linux386/wpaxp.sym      <OWRELROOT>/binl/wppaxp.sym
    <CCCMD> linux386/wppaxp01.int   <OWRELROOT>/binl/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
