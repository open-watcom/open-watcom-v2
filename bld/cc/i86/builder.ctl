# wcc Builder Control file
# ========================

set PROJDIR=<CWD>
set PROJNAME=wcc

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> boot rel ]
    cdsay <PROJDIR>

[ BLOCK <1> boot ]
#=================
    <CPCMD> <OWOBJDIR>/bwcc.exe     <OWBINDIR>/bwcc<CMDEXT>
    <CCCMD> <OWOBJDIR>/bwccd<DYEXT> <OWBINDIR>/bwccd<DYEXT>

[ BLOCK <1> bootclean ]
#======================
    echo rm -f <OWBINDIR>/bwcc<CMDEXT>
    rm -f <OWBINDIR>/bwcc<CMDEXT>
    rm -f <OWBINDIR>/bwccd<DYEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/wci86.exe        <OWRELROOT>/binw/wcc.exe
    <CCCMD> dos386/wci86.sym        <OWRELROOT>/binw/wcc.sym
    <CCCMD> dos386/wci8601.int      <OWRELROOT>/binw/wcci8601.int
    <CCCMD> os2386/wci86.exe        <OWRELROOT>/binp/wcc.exe
    <CCCMD> os2386/wci86.sym        <OWRELROOT>/binp/wcc.sym
    <CCCMD> os2386/wcdi86.dll       <OWRELROOT>/binp/dll/wccd.dll
    <CCCMD> os2386/wcdi86.sym       <OWRELROOT>/binp/dll/wccd.sym
    <CCCMD> os2386/wci8601.int      <OWRELROOT>/binp/dll/wcci8601.int
    <CCCMD> nt386/wci86.exe         <OWRELROOT>/binnt/wcc.exe
    <CCCMD> nt386/wci86.sym         <OWRELROOT>/binnt/wcc.sym
    <CCCMD> nt386/wcdi86.dll        <OWRELROOT>/binnt/wccd.dll
    <CCCMD> nt386/wcdi86.sym        <OWRELROOT>/binnt/wccd.sym
    <CCCMD> nt386/wci8601.int       <OWRELROOT>/binnt/wcci8601.int
    <CCCMD> ntaxp/wci86.exe         <OWRELROOT>/axpnt/wcc.exe
    <CCCMD> ntaxp/wci86.sym         <OWRELROOT>/axpnt/wcc.sym
    <CCCMD> ntaxp/wcdi86.dll        <OWRELROOT>/axpnt/wccd.dll
    <CCCMD> ntaxp/wcdi86.sym        <OWRELROOT>/axpnt/wccd.sym
    <CCCMD> ntaxp/wci8601.int       <OWRELROOT>/axpnt/wcci8601.int
    <CCCMD> qnx386/wci86.exe        <OWRELROOT>/qnx/binq/wcc
    <CCCMD> qnx386/wci86.sym        <OWRELROOT>/qnx/sym/wcc.sym
    <CCCMD> qnx386/wci8601.int      <OWRELROOT>/qnx/binq/wcci8601.int
    <CCCMD> linux386/wci86.exe      <OWRELROOT>/binl/wcc
    <CCCMD> linux386/wci86.sym      <OWRELROOT>/binl/wcc.sym
    <CCCMD> linux386/wci8601.int    <OWRELROOT>/binl/wcci8601.int

[ BLOCK . . ]
#============
cdsay <PROJDIR>
