# wpp386 Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=wpp386

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> boot ]
#=================
    <CPCMD> <OWOBJDIR>/bwpp386.exe     <OWBINDIR>/bwpp386<CMDEXT>
    <CCCMD> <OWOBJDIR>/bwppd386<DYEXT> <OWBINDIR>/bwppd386<DYEXT>

[ BLOCK <1> bootclean ]
#======================
    echo rm -f <OWBINDIR>/bwpp386<CMDEXT>
    rm -f <OWBINDIR>/bwpp386<CMDEXT>
    rm -f <OWBINDIR>/bwppd386<DYEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/wp386.exe        <OWRELROOT>/binw/wpp386.exe
    <CCCMD> dos386/wp386.sym        <OWRELROOT>/binw/wpp386.sym
    <CCCMD> dos386/wpp38601.int     <OWRELROOT>/binw/
    <CCCMD> os2386/wp386.exe        <OWRELROOT>/binp/wpp386.exe
    <CCCMD> os2386/wp386.sym        <OWRELROOT>/binp/wpp386.sym
    <CCCMD> os2386/wpd386.dll       <OWRELROOT>/binp/dll/wppd386.dll
    <CCCMD> os2386/wpd386.sym       <OWRELROOT>/binp/dll/wppd386.sym
    <CCCMD> os2386/wpp38601.int     <OWRELROOT>/binp/dll/
    <CCCMD> nt386/wp386.exe         <OWRELROOT>/binnt/wpp386.exe
    <CCCMD> nt386/wp386.sym         <OWRELROOT>/binnt/wpp386.sym
    <CCCMD> nt386/wpd386.dll        <OWRELROOT>/binnt/wppd386.dll
    <CCCMD> nt386/wpd386.sym        <OWRELROOT>/binnt/wppd386.sym
    <CCCMD> nt386/wpp38601.int      <OWRELROOT>/binnt/
    <CCCMD> ntaxp/wp386.exe         <OWRELROOT>/axpnt/wpp386.exe
    <CCCMD> ntaxp/wp386.sym         <OWRELROOT>/axpnt/wpp386.sym
    <CCCMD> ntaxp/wpd386.dll        <OWRELROOT>/axpnt/wppd386.dll
    <CCCMD> ntaxp/wpd386.sym        <OWRELROOT>/axpnt/wppd386.sym
    <CCCMD> ntaxp/wpp38601.int      <OWRELROOT>/axpnt/
    <CCCMD> qnx386/wp386.exe        <OWRELROOT>/qnx/binq/wpp386
    <CCCMD> qnx386/wp386.sym        <OWRELROOT>/qnx/sym/wpp386.sym
    <CCCMD> qnx386/wpp38601.int     <OWRELROOT>/qnx/binq/
    <CCCMD> linux386/wp386.exe      <OWRELROOT>/binl/wpp386
    <CCCMD> linux386/wp386.sym      <OWRELROOT>/binl/wpp386.sym
    <CCCMD> linux386/wpp38601.int   <OWRELROOT>/binl/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
