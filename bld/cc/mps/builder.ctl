# wccmps Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=wccmps

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/bwccmps.exe      <OWBINDIR>/bwccmps<CMDEXT>
    <CCCMD> <OWOBJDIR>/bwccdmps<DYEXT> <OWBINDIR>/bwccdmps<DYEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/bwccmps<CMDEXT>
    rm -f <OWBINDIR>/bwccmps<CMDEXT>
    rm -f <OWBINDIR>/bwccdmps<DYEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/wccmps.exe        <OWRELROOT>/binw/
    <CCCMD> dos386/wccmps.sym        <OWRELROOT>/binw/
    <CCCMD> dos386/wccmps01.int      <OWRELROOT>/binw/
    <CCCMD> os2386/wccmps.exe        <OWRELROOT>/binp/
    <CCCMD> os2386/wccmps.sym        <OWRELROOT>/binp/
    <CCCMD> os2386/wccdmps.dll       <OWRELROOT>/binp/dll/
    <CCCMD> os2386/wccdmps.sym       <OWRELROOT>/binp/dll/
    <CCCMD> os2386/wccmps01.int      <OWRELROOT>/binp/dll/
    <CCCMD> nt386/wccmps.exe         <OWRELROOT>/binnt/
    <CCCMD> nt386/wccmps.sym         <OWRELROOT>/binnt/
    <CCCMD> nt386/wccdmps.dll        <OWRELROOT>/binnt/
    <CCCMD> nt386/wccdmps.sym        <OWRELROOT>/binnt/
    <CCCMD> nt386/wccmps01.int       <OWRELROOT>/binnt/
    <CCCMD> ntaxp/wccmps.exe         <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wccmps.sym         <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wccdmps.dll        <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wccdmps.sym        <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wccmps01.int       <OWRELROOT>/axpnt/
    <CCCMD> qnx386/wccmps.exe        <OWRELROOT>/qnx/binq/wccmps
    <CCCMD> qnx386/wccmps.sym        <OWRELROOT>/qnx/sym/
    <CCCMD> qnx386/wccmps01.int      <OWRELROOT>/qnx/binq/
    <CCCMD> linux386/wccmps.exe      <OWRELROOT>/binl/wccmps
    <CCCMD> linux386/wccmps.sym      <OWRELROOT>/binl/
    <CCCMD> linux386/wccmps01.int    <OWRELROOT>/binl/

    <CCCMD> ntx64/wccmps.exe         <OWRELROOT>/binnt64/
    <CCCMD> ntx64/wccdmps.dll        <OWRELROOT>/binnt64/
    <CCCMD> ntx64/wccmps01.int       <OWRELROOT>/binnt64/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
