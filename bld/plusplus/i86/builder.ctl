# wpp Builder Control file
# ========================

set PROJDIR=<CWD>
set PROJNAME=wpp

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/bwpp.exe     <OWBINDIR>/bwpp<CMDEXT>
    <CCCMD> <OWOBJDIR>/bwppd<DYEXT> <OWBINDIR>/bwppd<DYEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/bwpp<CMDEXT>
    rm -f <OWBINDIR>/bwpp<CMDEXT>
    rm -f <OWBINDIR>/bwppd<DYEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/wp.exe           <OWRELROOT>/binw/wpp.exe
    <CCCMD> dos386/wp.sym           <OWRELROOT>/binw/wpp.sym
    <CCCMD> dos386/wppi8601.int     <OWRELROOT>/binw/
    <CCCMD> os2386/wp.exe           <OWRELROOT>/binp/wpp.exe
    <CCCMD> os2386/wp.sym           <OWRELROOT>/binp/wpp.sym
    <CCCMD> os2386/wpdi86.dll       <OWRELROOT>/binp/dll/wppdi86.dll
    <CCCMD> os2386/wpdi86.sym       <OWRELROOT>/binp/dll/wppdi86.sym
    <CCCMD> os2386/wppi8601.int     <OWRELROOT>/binp/dll/
    <CCCMD> nt386/wp.exe            <OWRELROOT>/binnt/wpp.exe
    <CCCMD> nt386/wp.sym            <OWRELROOT>/binnt/wpp.sym
    <CCCMD> nt386/wpdi86.dll        <OWRELROOT>/binnt/wppdi86.dll
    <CCCMD> nt386/wpdi86.sym        <OWRELROOT>/binnt/wppdi86.sym
    <CCCMD> nt386/wppi8601.int      <OWRELROOT>/binnt/
    <CCCMD> ntaxp/wp.exe            <OWRELROOT>/axpnt/wpp.exe
    <CCCMD> ntaxp/wp.sym            <OWRELROOT>/axpnt/wpp.sym
    <CCCMD> ntaxp/wpdi86.dll        <OWRELROOT>/axpnt/wppdi86.dll
    <CCCMD> ntaxp/wpdi86.sym        <OWRELROOT>/axpnt/wppdi86.sym
    <CCCMD> ntaxp/wppi8601.int      <OWRELROOT>/axpnt/
    <CCCMD> qnx386/wp.exe           <OWRELROOT>/qnx/binq/wpp
    <CCCMD> qnx386/wp.sym           <OWRELROOT>/qnx/sym/wpp.sym
    <CCCMD> qnx386/wppi8601.int     <OWRELROOT>/qnx/binq/
    <CCCMD> linux386/wp.exe         <OWRELROOT>/binl/wpp
    <CCCMD> linux386/wp.sym         <OWRELROOT>/binl/wpp.sym
    <CCCMD> linux386/wppi8601.int   <OWRELROOT>/binl/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
