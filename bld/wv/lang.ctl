# WATCOM Debugger Control file
# ============================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/bat/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
#=======================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> rel2 cprel2 ]
#========================
    <CPCMD> <DEVDIR>/wv/dbg/*.dbg               <RELROOT>/binw/
    <CPCMD> <DEVDIR>/wv/ssl/*.prs               <RELROOT>/binw/

  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> <DEVDIR>/wv/rfx/dosi86/rfx.exe      <RELROOT>/binw/rfx.exe
    <CPCMD> <DEVDIR>/wv/dsx/dos386/wv.exe       <RELROOT>/binw/wd.exe
    <CPCMD> <DEVDIR>/wv/dsx/dos386/wv.sym       <RELROOT>/binw/wd.sym

  [ IFDEF (os_win "") <2*> ]
    <CPCMD> <DEVDIR>/wv/win/wini86/wv.exe       <RELROOT>/binw/wdc.exe
    <CPCMD> <DEVDIR>/wv/win/wini86/wv.sym       <RELROOT>/binw/wdc.sym
    <CPCMD> <DEVDIR>/wv/win/wini86.gui/wv.exe   <RELROOT>/binw/wdw.exe
    <CPCMD> <DEVDIR>/wv/win/wini86.gui/wv.sym   <RELROOT>/binw/wdw.sym

  [ IFDEF (os_rdos "") <2*> ]
    <CPCMD> <DEVDIR>/wv/rdos/rdos386/wv.exe       <RELROOT>/rdos/wd.exe
    <CPCMD> <DEVDIR>/wv/rdos/rdos386/wv.sym       <RELROOT>/rdos/wd.sym
    <CPCMD> <DEVDIR>/wv/dbg/*.dbg                 <RELROOT>/rdos/
    <CPCMD> <DEVDIR>/wv/ssl/*.prs                 <RELROOT>/rdos/

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> <DEVDIR>/wv/rfx/os2386/rfx.exe      <RELROOT>/binp/rfx.exe
    <CPCMD> <DEVDIR>/wv/os2/os2386/wv.exe       <RELROOT>/binp/wd.exe
    <CPCMD> <DEVDIR>/wv/os2/os2386/wv.sym       <RELROOT>/binp/wd.sym
    <CPCMD> <DEVDIR>/wv/os2/os2386.gui/wv.exe   <RELROOT>/binp/wdw.exe
    <CPCMD> <DEVDIR>/wv/os2/os2386.gui/wv.sym   <RELROOT>/binp/wdw.sym
#   <CPCMD> <DEVDIR>/wv/os2/os2i86/wv.exe       <RELROOT>/binp/wd16.exe
#   <CPCMD> <DEVDIR>/wv/os2/os2i86.gui/wv.exe   <RELROOT>/binp/wdw16.exe

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> <DEVDIR>/wv/win/nt386/wv.exe        <RELROOT>/binnt/wd.exe
    <CPCMD> <DEVDIR>/wv/win/nt386/wv.sym        <RELROOT>/binnt/wd.sym
    <CPCMD> <DEVDIR>/wv/win/nt386.gui/wv.exe    <RELROOT>/binnt/wdw.exe
    <CPCMD> <DEVDIR>/wv/win/nt386.gui/wv.sym    <RELROOT>/binnt/wdw.sym

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> <DEVDIR>/wv/linux/linux386/wv.exe   <RELROOT>/binl/wd
    <CPCMD> <DEVDIR>/wv/linux/linux386/wv.sym   <RELROOT>/binl/wd.sym
    <CPCMD> <DEVDIR>/wv/dbg/*.dbg               <RELROOT>/binl/
    <CPCMD> <DEVDIR>/wv/ssl/*.prs               <RELROOT>/binl/

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> <DEVDIR>/wv/win/ntaxp/wv.exe        <RELROOT>/axpnt/wd.exe
    <CPCMD> <DEVDIR>/wv/win/ntaxp/wv.sym        <RELROOT>/axpnt/wd.sym
    <CPCMD> <DEVDIR>/wv/win/ntaxp.gui/wv.exe    <RELROOT>/axpnt/wdw.exe
    <CPCMD> <DEVDIR>/wv/win/ntaxp.gui/wv.sym    <RELROOT>/axpnt/wdw.sym

  [ IFDEF (os_qnx) <2*> ]
    <CPCMD> <DEVDIR>/wv/qnx/qnx386/wv.qnx       <RELROOT>/qnx/binq/wd
    <CPCMD> <DEVDIR>/wv/qnx/qnx386/wv.sym       <RELROOT>/qnx/sym/wd.sym
    <CPCMD> <DEVDIR>/wv/dbg/*.dbg               <RELROOT>/qnx/watcom/wd/
    <CPCMD> <DEVDIR>/wv/ssl/*.prs               <RELROOT>/qnx/watcom/wd/

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
