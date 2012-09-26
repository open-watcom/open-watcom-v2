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
    <CPCMD> dbg/*.dbg               <OWRELROOT>/binw/
    <CPCMD> ssl/*.prs               <OWRELROOT>/binw/

  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> rfx/dosi86/rfx.exe      <OWRELROOT>/binw/rfx.exe
    <CPCMD> dsx/dos386/wv.exe       <OWRELROOT>/binw/wd.exe
    <CPCMD> dsx/dos386/wv.sym       <OWRELROOT>/binw/wd.sym

  [ IFDEF (os_win "") <2*> ]
    <CPCMD> win/wini86/wv.exe       <OWRELROOT>/binw/wdc.exe
    <CPCMD> win/wini86/wv.sym       <OWRELROOT>/binw/wdc.sym
    <CPCMD> win/wini86.gui/wv.exe   <OWRELROOT>/binw/wdw.exe
    <CPCMD> win/wini86.gui/wv.sym   <OWRELROOT>/binw/wdw.sym

  [ IFDEF (os_rdos "") <2*> ]
    <CPCMD> rdos/rdos386/wv.exe       <OWRELROOT>/rdos/wd.exe
    <CPCMD> rdos/rdos386/wv.sym       <OWRELROOT>/rdos/wd.sym
    <CPCMD> dbg/*.dbg                 <OWRELROOT>/rdos/
    <CPCMD> ssl/*.prs                 <OWRELROOT>/rdos/

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> rfx/os2386/rfx.exe      <OWRELROOT>/binp/rfx.exe
    <CPCMD> os2/os2386/wv.exe       <OWRELROOT>/binp/wd.exe
    <CPCMD> os2/os2386/wv.sym       <OWRELROOT>/binp/wd.sym
    <CPCMD> os2/os2386.gui/wv.exe   <OWRELROOT>/binp/wdw.exe
    <CPCMD> os2/os2386.gui/wv.sym   <OWRELROOT>/binp/wdw.sym
#   <CPCMD> os2/os2i86/wv.exe       <OWRELROOT>/binp/wd16.exe
#   <CPCMD> os2/os2i86.gui/wv.exe   <OWRELROOT>/binp/wdw16.exe

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> win/nt386/wv.exe        <OWRELROOT>/binnt/wd.exe
    <CPCMD> win/nt386/wv.sym        <OWRELROOT>/binnt/wd.sym
    <CPCMD> win/nt386.gui/wv.exe    <OWRELROOT>/binnt/wdw.exe
    <CPCMD> win/nt386.gui/wv.sym    <OWRELROOT>/binnt/wdw.sym

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> linux/linux386/wv.exe   <OWRELROOT>/binl/wd
    <CPCMD> linux/linux386/wv.sym   <OWRELROOT>/binl/wd.sym
    <CPCMD> dbg/*.dbg               <OWRELROOT>/binl/
    <CPCMD> ssl/*.prs               <OWRELROOT>/binl/

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> win/ntaxp/wv.exe        <OWRELROOT>/axpnt/wd.exe
    <CPCMD> win/ntaxp/wv.sym        <OWRELROOT>/axpnt/wd.sym
    <CPCMD> win/ntaxp.gui/wv.exe    <OWRELROOT>/axpnt/wdw.exe
    <CPCMD> win/ntaxp.gui/wv.sym    <OWRELROOT>/axpnt/wdw.sym

  [ IFDEF (os_qnx) <2*> ]
    <CPCMD> qnx/qnx386/wv.qnx       <OWRELROOT>/qnx/binq/wd
    <CPCMD> qnx/qnx386/wv.sym       <OWRELROOT>/qnx/sym/wd.sym
    <CPCMD> dbg/*.dbg               <OWRELROOT>/qnx/watcom/wd/
    <CPCMD> ssl/*.prs               <OWRELROOT>/qnx/watcom/wd/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
