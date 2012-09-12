# wpp Builder Control file
# ========================

set PROJDIR=<CWD>
set PROJNAME=wpp

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> boot ]
#=================
    <CPCMD> <PROJDIR>/<OWOBJDIR>/wcppi86.exe <OWBINDIR>/bwpp<CMDEXT>

[ BLOCK <1> bootclean ]
#======================
    echo rm -f <OWBINDIR>/bwpp<CMDEXT>
    rm -f <OWBINDIR>/bwpp<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/wppi86.exe       <OWRELROOT>/binw/wpp.exe
    <CCCMD> dos386/wppi86.sym       <OWRELROOT>/binw/wpp.sym
    <CCCMD> dos386/wppi8601.int     <OWRELROOT>/binw/
    <CCCMD> os2386/wppi86.exe       <OWRELROOT>/binp/wpp.exe
    <CCCMD> os2386/wppi86.sym       <OWRELROOT>/binp/wpp.sym
    <CCCMD> os2386/wppdi86.dll      <OWRELROOT>/binp/dll/
    <CCCMD> os2386/wppdi86.sym      <OWRELROOT>/binp/dll/
    <CCCMD> os2386/wppi8601.int     <OWRELROOT>/binp/dll/
    <CCCMD> nt386/wppi86.exe        <OWRELROOT>/binnt/wpp.exe
    <CCCMD> nt386/wppi86.sym        <OWRELROOT>/binnt/wpp.sym
    <CCCMD> nt386/wppdi86.dll       <OWRELROOT>/binnt/
    <CCCMD> nt386/wppdi86.sym       <OWRELROOT>/binnt/
    <CCCMD> nt386/wppi8601.int      <OWRELROOT>/binnt/
    <CCCMD> ntaxp/wppi86.exe        <OWRELROOT>/axpnt/wpp.exe
    <CCCMD> ntaxp/wppi86.sym        <OWRELROOT>/axpnt/wpp.sym
    <CCCMD> ntaxp/wppdi86.dll       <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wppdi86.sym       <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wppi8601.int      <OWRELROOT>/axpnt/
    <CCCMD> qnx386/wppi86.exe       <OWRELROOT>/qnx/binq/wpp
    <CCCMD> qnx386/wppi86.sym       <OWRELROOT>/qnx/sym/wpp.sym
    <CCCMD> qnx386/wppi8601.int     <OWRELROOT>/qnx/binq/
    <CCCMD> linux386/wppi86.exe     <OWRELROOT>/binl/wpp
    <CCCMD> linux386/wppi86.sym     <OWRELROOT>/binl/wpp.sym
    <CCCMD> linux386/wppi8601.int   <OWRELROOT>/binl/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
