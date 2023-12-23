# WATCOM Debugger Control file
# ============================

set PROJNAME=wd

set PROJDIR=<CWD>

[ INCLUDE "<OWROOT>/build/prolog.ctl" ]

[ INCLUDE "<OWROOT>/build/defrule.ctl" ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay "<PROJDIR>"

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> dbg/dos386/*.dbg         "<OWRELROOT>/binw/"
    <CCCMD> ssl/dos386/*.prs         "<OWRELROOT>/binw/"
    <CCCMD> dbg/nt386/*.dbg          "<OWRELROOT>/binnt/"
    <CCCMD> ssl/nt386/*.prs          "<OWRELROOT>/binnt/"
    <CCCMD> dbg/os2386/*.dbg         "<OWRELROOT>/binp/"
    <CCCMD> ssl/os2386/*.prs         "<OWRELROOT>/binp/"
    <CCCMD> dbg/linux386/*.dbg       "<OWRELROOT>/binl/"
    <CCCMD> ssl/linux386/*.prs       "<OWRELROOT>/binl/"
    <CCCMD> dbg/rdos386/*.dbg        "<OWRELROOT>/rdos/"
    <CCCMD> ssl/rdos386/*.prs        "<OWRELROOT>/rdos/"
#    <CCCMD> dbg/qnx386/*.dbg        "<OWRELROOT>/qnx/watcom/wd/"
#    <CCCMD> ssl/qnx386/*.prs        "<OWRELROOT>/qnx/watcom/wd/"

    <CCCMD> dbg/linuxx64/*.dbg       "<OWRELROOT>/binl64/"
    <CCCMD> ssl/linuxx64/*.prs       "<OWRELROOT>/binl64/"

    <CCCMD> dbg/ntx64/*.dbg          "<OWRELROOT>/binnt64/"
    <CCCMD> ssl/ntx64/*.prs          "<OWRELROOT>/binnt64/"

    <CCCMD> dsx/dos386/wd.exe        "<OWRELROOT>/binw/"
    <CCCMD> dsx/dos386/wd.sym        "<OWRELROOT>/binw/"
    <CCCMD> win/wini86/wdc.exe       "<OWRELROOT>/binw/"
    <CCCMD> win/wini86/wdc.sym       "<OWRELROOT>/binw/"
    <CCCMD> win/wini86.gui/wdw.exe   "<OWRELROOT>/binw/"
    <CCCMD> win/wini86.gui/wdw.sym   "<OWRELROOT>/binw/"
    <CCCMD> win/wd.pif               "<OWRELROOT>/binw/"
    <CCCMD> rdos/rdos386/wd.exe      "<OWRELROOT>/rdos/"
    <CCCMD> rdos/rdos386/wd.sym      "<OWRELROOT>/rdos/"
    <CCCMD> os2/os2386/wd.exe        "<OWRELROOT>/binp/"
    <CCCMD> os2/os2386/wd.sym        "<OWRELROOT>/binp/"
    <CCCMD> os2/os2386.gui/wdw.exe   "<OWRELROOT>/binp/"
    <CCCMD> os2/os2386.gui/wdw.sym   "<OWRELROOT>/binp/"
#    <CCCMD> os2/os2i86/wd16.exe      "<OWRELROOT>/binp/"
#    <CCCMD> os2/os2i86.gui/wdw16.exe "<OWRELROOT>/binp/"
    <CCCMD> nt/nt386/wd.exe          "<OWRELROOT>/binnt/"
    <CCCMD> nt/nt386/wd.sym          "<OWRELROOT>/binnt/"
    <CCCMD> nt/nt386.gui/wdw.exe     "<OWRELROOT>/binnt/"
    <CCCMD> nt/nt386.gui/wdw.sym     "<OWRELROOT>/binnt/"
    <CCCMD> linux/linux386/wd.exe    "<OWRELROOT>/binl/wd"
    <CCCMD> linux/linux386/wd.sym    "<OWRELROOT>/binl/"
    <CCCMD> nt/ntaxp/wd.exe          "<OWRELROOT>/axpnt/"
    <CCCMD> nt/ntaxp/wd.sym          "<OWRELROOT>/axpnt/"
    <CCCMD> nt/ntaxp.gui/wdw.exe     "<OWRELROOT>/axpnt/"
    <CCCMD> nt/ntaxp.gui/wdw.sym     "<OWRELROOT>/axpnt/"
    <CCCMD> qnx/qnx386/wd.exe        "<OWRELROOT>/qnx/binq/wd"
    <CCCMD> qnx/qnx386/wd.sym        "<OWRELROOT>/qnx/sym/"

    <CCCMD> nt/ntx64/wd.exe          "<OWRELROOT>/binnt64/"
    <CCCMD> nt/ntx64.gui/wdw.exe     "<OWRELROOT>/binnt64/"
    <CCCMD> linux/linuxx64/wd.exe    "<OWRELROOT>/binl64/wd"
    <CCCMD> linux/linuxx64/wd.sym    "<OWRELROOT>/binl64/"

    <CCCMD> rfx/dosi86/rfx.exe       "<OWRELROOT>/binw/"
    <CCCMD> rfx/dosi86/rfx.sym       "<OWRELROOT>/binw/"
    <CCCMD> rfx/os2386/rfx.exe       "<OWRELROOT>/binp/"
    <CCCMD> rfx/os2386/rfx.sym       "<OWRELROOT>/binp/"
    <CCCMD> rfx/nt386/rfx.exe        "<OWRELROOT>/binnt/"
    <CCCMD> rfx/nt386/rfx.sym        "<OWRELROOT>/binnt/"

    <CCCMD> rfx/ntx64/rfx.exe        "<OWRELROOT>/binnt64/"
    <CCCMD> rfx/ntx64/rfx.sym        "<OWRELROOT>/binnt64/"

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
