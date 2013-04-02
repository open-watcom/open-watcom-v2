# wccppc Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=wccppc

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/bwcppc.exe      <OWBINDIR>/bwccppc<CMDEXT>
    <CCCMD> <OWOBJDIR>/bwccdppc<DYEXT> <OWBINDIR>/bwccdppc<DYEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/bwccppc<CMDEXT>
    rm -f <OWBINDIR>/bwccppc<CMDEXT>
    rm -f <OWBINDIR>/bwccdppc<DYEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/wcppc.exe        <OWRELROOT>/binw/wccppc.exe
    <CCCMD> dos386/wcppc.sym        <OWRELROOT>/binw/wccppc.sym
    <CCCMD> dos386/wcppc01.int      <OWRELROOT>/binw/wccppc01.int
    <CCCMD> os2386/wcppc.exe        <OWRELROOT>/binp/wccppc.exe
    <CCCMD> os2386/wcppc.sym        <OWRELROOT>/binp/wccppc.sym
    <CCCMD> os2386/wcdppc.dll       <OWRELROOT>/binp/dll/wccdppc.dll
    <CCCMD> os2386/wcdppc.sym       <OWRELROOT>/binp/dll/wccdppc.sym
    <CCCMD> os2386/wcppc01.int      <OWRELROOT>/binp/dll/wccppc01.int
    <CCCMD> nt386/wcppc.exe         <OWRELROOT>/binnt/wccppc.exe
    <CCCMD> nt386/wcppc.sym         <OWRELROOT>/binnt/wccppc.sym
    <CCCMD> nt386/wcdppc.dll        <OWRELROOT>/binnt/wccdppc.dll
    <CCCMD> nt386/wcdppc.sym        <OWRELROOT>/binnt/wccdppc.sym
    <CCCMD> nt386/wcppc01.int       <OWRELROOT>/binnt/wccppc01.int
    <CCCMD> ntaxp/wcppc.exe         <OWRELROOT>/axpnt/wccppc.exe
    <CCCMD> ntaxp/wcppc.sym         <OWRELROOT>/axpnt/wccppc.sym
    <CCCMD> ntaxp/wcdppc.dll        <OWRELROOT>/axpnt/wccdppc.dll
    <CCCMD> ntaxp/wcdppc.sym        <OWRELROOT>/axpnt/wccdppc.sym
    <CCCMD> ntaxp/wcppc01.int       <OWRELROOT>/axpnt/wccppc01.int
    <CCCMD> qnx386/wcppc.exe        <OWRELROOT>/qnx/binq/wccppc
    <CCCMD> qnx386/wcppc.sym        <OWRELROOT>/qnx/sym/wccppc.sym
    <CCCMD> qnx386/wcppc01.int      <OWRELROOT>/qnx/binq/wccppc01.int
    <CCCMD> linux386/wcppc.exe      <OWRELROOT>/binl/wccppc
    <CCCMD> linux386/wcppc.sym      <OWRELROOT>/binl/wccppc.sym
    <CCCMD> linux386/wcppc01.int    <OWRELROOT>/binl/wccppc01.int

[ BLOCK . . ]
#============
cdsay <PROJDIR>
