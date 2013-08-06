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
    <CCCMD> dos386/wcc.exe          <OWRELROOT>/binw/
    <CCCMD> dos386/wcc.sym          <OWRELROOT>/binw/
    <CCCMD> dos386/wcci8601.int     <OWRELROOT>/binw/
    <CCCMD> os2386.dll/wcc.exe      <OWRELROOT>/binp/
    <CCCMD> os2386.dll/wcc.sym      <OWRELROOT>/binp/
    <CCCMD> os2386.dll/wccd.dll     <OWRELROOT>/binp/dll/
    <CCCMD> os2386.dll/wccd.sym     <OWRELROOT>/binp/dll/
    <CCCMD> os2386.dll/wcci8601.int <OWRELROOT>/binp/dll/
    <CCCMD> nt386.dll/wcc.exe       <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/wcc.sym       <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/wccd.dll      <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/wccd.sym      <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/wcci8601.int  <OWRELROOT>/binnt/
    <CCCMD> ntaxp.dll/wcc.exe       <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/wcc.sym       <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/wccd.dll      <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/wccd.sym      <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/wcci8601.int  <OWRELROOT>/axpnt/
    <CCCMD> qnx386/wcc.exe          <OWRELROOT>/qnx/binq/wcc
    <CCCMD> qnx386/wcc.sym          <OWRELROOT>/qnx/sym/
    <CCCMD> qnx386/wcci8601.int     <OWRELROOT>/qnx/binq/
    <CCCMD> linux386/wcc.exe        <OWRELROOT>/binl/wcc
    <CCCMD> linux386/wcc.sym        <OWRELROOT>/binl/
    <CCCMD> linux386/wcci8601.int   <OWRELROOT>/binl/

    <CCCMD> ntx64.dll/wcc.exe       <OWRELROOT>/binnt64/
    <CCCMD> ntx64.dll/wccd.dll      <OWRELROOT>/binnt64/
    <CCCMD> ntx64.dll/wcci8601.int  <OWRELROOT>/binnt64/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
