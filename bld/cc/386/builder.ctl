# wcc386 Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=wcc386

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> boot rel ]
    cdsay <PROJDIR>

[ BLOCK <1> boot ]
#=================
    <CPCMD> <OWOBJDIR>/bwcc386.exe     <OWBINDIR>/bwcc386<CMDEXT>
    <CCCMD> <OWOBJDIR>/bwccd386<DYEXT> <OWBINDIR>/bwccd386<DYEXT>

[ BLOCK <1> bootclean ]
#======================
    echo rm -f <OWBINDIR>/bwcc386<CMDEXT>
    rm -f <OWBINDIR>/bwcc386<CMDEXT>
    rm -f <OWBINDIR>/bwccd386<DYEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/wc386.exe        <OWRELROOT>/binw/wcc386.exe
    <CCCMD> dos386/wc386.sym        <OWRELROOT>/binw/wcc386.sym
    <CCCMD> dos386/wc38601.int      <OWRELROOT>/binw/wcc38601.int
    <CCCMD> os2386/wc386.exe        <OWRELROOT>/binp/wcc386.exe
    <CCCMD> os2386/wc386.sym        <OWRELROOT>/binp/wcc386.sym
    <CCCMD> os2386/wcd386.dll       <OWRELROOT>/binp/dll/wccd386.dll
    <CCCMD> os2386/wcd386.sym       <OWRELROOT>/binp/dll/wccd386.sym
    <CCCMD> os2386/wc38601.int      <OWRELROOT>/binp/dll/wcc38601.int
    <CCCMD> nt386/wc386.exe         <OWRELROOT>/binnt/wcc386.exe
    <CCCMD> nt386/wc386.sym         <OWRELROOT>/binnt/wcc386.sym
    <CCCMD> nt386/wcd386.dll        <OWRELROOT>/binnt/wccd386.dll
    <CCCMD> nt386/wcd386.sym        <OWRELROOT>/binnt/wccd386.sym
    <CCCMD> nt386/wc38601.int       <OWRELROOT>/binnt/wcc38601.int
    <CCCMD> ntaxp/wc386.exe         <OWRELROOT>/axpnt/wcc386.exe
    <CCCMD> ntaxp/wc386.sym         <OWRELROOT>/axpnt/wcc386.sym
    <CCCMD> ntaxp/wcd386.dll        <OWRELROOT>/axpnt/wccd386.dll
    <CCCMD> ntaxp/wcd386.sym        <OWRELROOT>/axpnt/wccd386.sym
    <CCCMD> ntaxp/wc38601.int       <OWRELROOT>/axpnt/wcc38601.int
    <CCCMD> qnx386/wc386.exe        <OWRELROOT>/qnx/binq/wcc386
    <CCCMD> qnx386/wc386.sym        <OWRELROOT>/qnx/sym/wcc386.sym
    <CCCMD> qnx386/wc38601.int      <OWRELROOT>/qnx/binq/wcc38601.int
    <CCCMD> linux386/wc386.exe      <OWRELROOT>/binl/wcc386
    <CCCMD> linux386/wc386.sym      <OWRELROOT>/binl/wcc386.sym
    <CCCMD> linux386/wc38601.int    <OWRELROOT>/binl/wcc38601.int

[ BLOCK . . ]
#============
cdsay <PROJDIR>
