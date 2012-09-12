# wccmps Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=wccmps

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> boot ]
#=================
    <CPCMD> <OWOBJDIR>/wcmps.exe <OWBINDIR>/bwccmps<CMDEXT>

[ BLOCK <1> bootclean ]
#======================
    echo rm -f <OWBINDIR>/bwccmps<CMDEXT>
    rm -f <OWBINDIR>/bwccmps<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/wcmps.exe        <OWRELROOT>/binw/wccmps.exe
    <CCCMD> dos386/wcmps.sym        <OWRELROOT>/binw/wccmps.sym
    <CCCMD> dos386/wcmps01.int      <OWRELROOT>/binw/wccmps01.int
    <CCCMD> os2386/wcmps.exe        <OWRELROOT>/binp/wccmps.exe
    <CCCMD> os2386/wcmps.sym        <OWRELROOT>/binp/wccmps.sym
    <CCCMD> os2386/wcdmps.dll       <OWRELROOT>/binp/dll/wccdmps.dll
    <CCCMD> os2386/wcdmps.sym       <OWRELROOT>/binp/dll/wccdmps.sym
    <CCCMD> os2386/wcmps01.int      <OWRELROOT>/binp/dll/wccmps01.int
    <CCCMD> nt386/wcmps.exe         <OWRELROOT>/binnt/wccmps.exe
    <CCCMD> nt386/wcmps.sym         <OWRELROOT>/binnt/wccmps.sym
    <CCCMD> nt386/wcdmps.dll        <OWRELROOT>/binnt/wccdmps.dll
    <CCCMD> nt386/wcdmps.sym        <OWRELROOT>/binnt/wccdmps.sym
    <CCCMD> nt386/wcmps01.int       <OWRELROOT>/binnt/wccmps01.int
    <CCCMD> ntaxp/wcmps.exe         <OWRELROOT>/axpnt/wccmps.exe
    <CCCMD> ntaxp/wcmps.sym         <OWRELROOT>/axpnt/wccmps.sym
    <CCCMD> ntaxp/wcdmps.dll        <OWRELROOT>/axpnt/wccdmps.dll
    <CCCMD> ntaxp/wcdmps.sym        <OWRELROOT>/axpnt/wccdmps.sym
    <CCCMD> ntaxp/wcmps01.int       <OWRELROOT>/axpnt/wccmps01.int
    <CCCMD> qnx386/wcmps.exe        <OWRELROOT>/qnx/binq/wccmps
    <CCCMD> qnx386/wcmps.sym        <OWRELROOT>/qnx/sym/wccmps.sym
    <CCCMD> qnx386/wcmps01.int      <OWRELROOT>/qnx/binq/wccmps01.int
    <CCCMD> linux386/wcmps.exe      <OWRELROOT>/binl/wccmps
    <CCCMD> linux386/wcmps.sym      <OWRELROOT>/binl/wccmps.sym
    <CCCMD> linux386/wcmps01.int    <OWRELROOT>/binl/wccmps01.int

[ BLOCK . . ]
#============
cdsay <PROJDIR>
