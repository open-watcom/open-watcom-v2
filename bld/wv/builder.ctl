# WATCOM Debugger Control file
# ============================

set PROJDIR=<CWD>
set PROJNAME=wd

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dbg/dos386/*.dbg        <OWRELROOT>/binw/
    <CCCMD> ssl/dos386/*.prs        <OWRELROOT>/binw/
    <CCCMD> dbg/nt386/*.dbg         <OWRELROOT>/binnt/
    <CCCMD> ssl/nt386/*.prs         <OWRELROOT>/binnt/
    <CCCMD> dbg/os2386/*.dbg        <OWRELROOT>/binp/
    <CCCMD> ssl/os2386/*.prs        <OWRELROOT>/binp/
    <CCCMD> dbg/linux386/*.dbg      <OWRELROOT>/binl/
    <CCCMD> ssl/linux386/*.prs      <OWRELROOT>/binl/
    <CCCMD> dbg/rdos386/*.dbg       <OWRELROOT>/rdos/
    <CCCMD> ssl/rdos386/*.prs       <OWRELROOT>/rdos/
#    <CCCMD> dbg/qnx386/*.dbg        <OWRELROOT>/qnx/watcom/wd/
#    <CCCMD> ssl/qnx386/*.prs        <OWRELROOT>/qnx/watcom/wd/

    <CCCMD> dbg/linuxx64/*.dbg      <OWRELROOT>/binl64/
    <CCCMD> ssl/linuxx64/*.prs      <OWRELROOT>/binl64/

    <CCCMD> dbg/ntx64/*.dbg         <OWRELROOT>/binnt64/
    <CCCMD> ssl/ntx64/*.prs         <OWRELROOT>/binnt64/

    <CCCMD> rfx/dosi86/rfx.exe      <OWRELROOT>/binw/rfx.exe
    <CCCMD> dsx/dos386/wv.exe       <OWRELROOT>/binw/wd.exe
    <CCCMD> dsx/dos386/wv.sym       <OWRELROOT>/binw/wd.sym
    <CCCMD> win/wini86/wv.exe       <OWRELROOT>/binw/wdc.exe
    <CCCMD> win/wini86/wv.sym       <OWRELROOT>/binw/wdc.sym
    <CCCMD> win/wini86.gui/wv.exe   <OWRELROOT>/binw/wdw.exe
    <CCCMD> win/wini86.gui/wv.sym   <OWRELROOT>/binw/wdw.sym
    <CCCMD> rdos/rdos386/wv.exe     <OWRELROOT>/rdos/wd.exe
    <CCCMD> rdos/rdos386/wv.sym     <OWRELROOT>/rdos/wd.sym
    <CCCMD> rfx/os2386/rfx.exe      <OWRELROOT>/binp/rfx.exe
    <CCCMD> os2/os2386/wv.exe       <OWRELROOT>/binp/wd.exe
    <CCCMD> os2/os2386/wv.sym       <OWRELROOT>/binp/wd.sym
    <CCCMD> os2/os2386.gui/wv.exe   <OWRELROOT>/binp/wdw.exe
    <CCCMD> os2/os2386.gui/wv.sym   <OWRELROOT>/binp/wdw.sym
#    <CCCMD> os2/os2i86/wv.exe       <OWRELROOT>/binp/wd16.exe
#    <CCCMD> os2/os2i86.gui/wv.exe   <OWRELROOT>/binp/wdw16.exe
    <CCCMD> win/nt386/wv.exe        <OWRELROOT>/binnt/wd.exe
    <CCCMD> win/nt386/wv.sym        <OWRELROOT>/binnt/wd.sym
    <CCCMD> win/nt386.gui/wv.exe    <OWRELROOT>/binnt/wdw.exe
    <CCCMD> win/nt386.gui/wv.sym    <OWRELROOT>/binnt/wdw.sym
    <CCCMD> linux/linux386/wv.exe   <OWRELROOT>/binl/wd
    <CCCMD> linux/linux386/wv.sym   <OWRELROOT>/binl/wd.sym
    <CCCMD> win/ntaxp/wv.exe        <OWRELROOT>/axpnt/wd.exe
    <CCCMD> win/ntaxp/wv.sym        <OWRELROOT>/axpnt/wd.sym
    <CCCMD> win/ntaxp.gui/wv.exe    <OWRELROOT>/axpnt/wdw.exe
    <CCCMD> win/ntaxp.gui/wv.sym    <OWRELROOT>/axpnt/wdw.sym
    <CCCMD> qnx/qnx386/wv.qnx       <OWRELROOT>/qnx/binq/wd
    <CCCMD> qnx/qnx386/wv.sym       <OWRELROOT>/qnx/sym/wd.sym

    <CCCMD> win/ntx64/wv.exe        <OWRELROOT>/binnt64/wd.exe
    <CCCMD> win/ntx64.gui/wv.exe    <OWRELROOT>/binnt64/wdw.exe

[ BLOCK . . ]
#============
cdsay <PROJDIR>
