# wppaxp Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=wppaxp

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> boot ]
#=================
    <CPCMD> <OWOBJDIR>/wcppaxp.exe <OWBINDIR>/bwppaxp<CMDEXT>

[ BLOCK <1> bootclean ]
#======================
    echo rm -f <OWBINDIR>/bwppaxp<CMDEXT>
    rm -f <OWBINDIR>/bwppaxp<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/wppaxp.exe       <OWRELROOT>/binw/
    <CCCMD> dos386/wppaxp.sym       <OWRELROOT>/binw/
    <CCCMD> dos386/wppaxp01.int     <OWRELROOT>/binw/
    <CCCMD> os2386/wppaxp.exe       <OWRELROOT>/binp/
    <CCCMD> os2386/wppaxp.sym       <OWRELROOT>/binp/
    <CCCMD> os2386/wppdaxp.dll      <OWRELROOT>/binp/dll/
    <CCCMD> os2386/wppdaxp.sym      <OWRELROOT>/binp/dll/
    <CCCMD> os2386/wppaxp01.int     <OWRELROOT>/binp/dll/
    <CCCMD> nt386/wppaxp.exe        <OWRELROOT>/binnt/
    <CCCMD> nt386/wppaxp.sym        <OWRELROOT>/binnt/
    <CCCMD> nt386/wppdaxp.dll       <OWRELROOT>/binnt/
    <CCCMD> nt386/wppdaxp.sym       <OWRELROOT>/binnt/
    <CCCMD> nt386/wppaxp01.int      <OWRELROOT>/binnt/
    <CCCMD> ntaxp/wppaxp.exe        <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wppaxp.sym        <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wppdaxp.dll       <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wppdaxp.sym       <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wppaxp01.int      <OWRELROOT>/axpnt/
    <CCCMD> linux386/wppaxp.exe     <OWRELROOT>/binl/wppaxp
    <CCCMD> linux386/wppaxp.sym     <OWRELROOT>/binl/
    <CCCMD> linux386/wppaxp01.int   <OWRELROOT>/binl/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
