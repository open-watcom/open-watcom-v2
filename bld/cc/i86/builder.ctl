# wcc Builder Control file
# ========================

set PROJDIR=<CWD>
set PROJNAME=wcc

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/bwcc.exe     <OWBINDIR>/bwcc<CMDEXT>
    <CCCMD> <OWOBJDIR>/bwccd<DYEXT> <OWBINDIR>/bwccd<DYEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/bwcc<CMDEXT>
    rm -f <OWBINDIR>/bwcc<CMDEXT>
    rm -f <OWBINDIR>/bwccd<DYEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/wcc.exe        <OWRELROOT>/binw/
    <CCCMD> dos386/wcc.sym        <OWRELROOT>/binw/
    <CCCMD> dos386/wcci8601.int   <OWRELROOT>/binw/
    <CCCMD> os2386/wcc.exe        <OWRELROOT>/binp/
    <CCCMD> os2386/wcc.sym        <OWRELROOT>/binp/
    <CCCMD> os2386/wccd.dll       <OWRELROOT>/binp/dll/
    <CCCMD> os2386/wccd.sym       <OWRELROOT>/binp/dll/
    <CCCMD> os2386/wcci8601.int   <OWRELROOT>/binp/dll/
    <CCCMD> nt386/wcc.exe         <OWRELROOT>/binnt/
    <CCCMD> nt386/wcc.sym         <OWRELROOT>/binnt/
    <CCCMD> nt386/wccd.dll        <OWRELROOT>/binnt/
    <CCCMD> nt386/wccd.sym        <OWRELROOT>/binnt/
    <CCCMD> nt386/wcci8601.int    <OWRELROOT>/binnt/
    <CCCMD> ntaxp/wcc.exe         <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wcc.sym         <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wccd.dll        <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wccd.sym        <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wcci8601.int    <OWRELROOT>/axpnt/
    <CCCMD> qnx386/wcc.exe        <OWRELROOT>/qnx/binq/wcc
    <CCCMD> qnx386/wcc.sym        <OWRELROOT>/qnx/sym/
    <CCCMD> qnx386/wcci8601.int   <OWRELROOT>/qnx/binq/
    <CCCMD> linux386/wcc.exe      <OWRELROOT>/binl/wcc
    <CCCMD> linux386/wcc.sym      <OWRELROOT>/binl/
    <CCCMD> linux386/wcci8601.int <OWRELROOT>/binl/

    <CCCMD> ntx64/wcc.exe         <OWRELROOT>/binnt64/
    <CCCMD> ntx64/wccd.dll        <OWRELROOT>/binnt64/
    <CCCMD> ntx64/wcci8601.int    <OWRELROOT>/binnt64/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
