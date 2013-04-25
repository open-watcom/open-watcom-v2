# wccaxp Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=wccaxp

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/bwccaxp.exe     <OWBINDIR>/bwccaxp<CMDEXT>
    <CCCMD> <OWOBJDIR>/bwccdaxp<DYEXT> <OWBINDIR>/bwccdaxp<DYEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/bwccaxp<CMDEXT>
    rm -f <OWBINDIR>/bwccaxp<CMDEXT>
    rm -f <OWBINDIR>/bwccdaxp<DYEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/wccaxp.exe        <OWRELROOT>/binw/
    <CCCMD> dos386/wccaxp.sym        <OWRELROOT>/binw/
    <CCCMD> dos386/wccaxp01.int      <OWRELROOT>/binw/
    <CCCMD> os2386/wccaxp.exe        <OWRELROOT>/binp/
    <CCCMD> os2386/wccaxp.sym        <OWRELROOT>/binp/
    <CCCMD> os2386/wccdaxp.dll       <OWRELROOT>/binp/dll/
    <CCCMD> os2386/wccdaxp.sym       <OWRELROOT>/binp/dll/
    <CCCMD> os2386/wccaxp01.int      <OWRELROOT>/binp/dll/
    <CCCMD> nt386/wccaxp.exe         <OWRELROOT>/binnt/
    <CCCMD> nt386/wccaxp.sym         <OWRELROOT>/binnt/
    <CCCMD> nt386/wccdaxp.dll        <OWRELROOT>/binnt/
    <CCCMD> nt386/wccdaxp.sym        <OWRELROOT>/binnt/
    <CCCMD> nt386/wccaxp01.int       <OWRELROOT>/binnt/
    <CCCMD> ntaxp/wccaxp.exe         <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wccaxp.sym         <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wccdaxp.dll        <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wccdaxp.sym        <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wccaxp01.int       <OWRELROOT>/axpnt/
    <CCCMD> qnx386/wccaxp.exe        <OWRELROOT>/qnx/binq/wccaxp
    <CCCMD> qnx386/wccaxp.sym        <OWRELROOT>/qnx/sym/
    <CCCMD> dos386/wccaxp01.int      <OWRELROOT>/qnx/binq/
    <CCCMD> linux386/wccaxp.exe      <OWRELROOT>/binl/wccaxp
    <CCCMD> linux386/wccaxp.sym      <OWRELROOT>/binl/
    <CCCMD> linux386/wccaxp01.int    <OWRELROOT>/binl/

    <CCCMD> ntx64/wccaxp.exe         <OWRELROOT>/binnt64/
    <CCCMD> ntx64/wccdaxp.dll        <OWRELROOT>/binnt64/
    <CCCMD> ntx64/wccaxp01.int       <OWRELROOT>/binnt64/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
