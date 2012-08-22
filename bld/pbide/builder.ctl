# TODO: Restructure directories, redo makefiles. This project is a mess.
#       Parts of it seem obsolete (wfscopy).
#
# PBIDE Builder Control file
# ==========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
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
  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> <DEVDIR>/pbide/fscopy/wfscopy.exe       <OWRELROOT>/binw/wfscopy.exe
  [ IFDEF (os_win "") <2*> ]
    <CPCMD> <DEVDIR>/pbide/run/wini86/run.exe       <OWRELROOT>/binw/watrun.exe
    <CPCMD> <DEVDIR>/pbide/dlldbg/wini86/dlldbg.exe <OWRELROOT>/binw/dlldbg.exe
    <CPCMD> <DEVDIR>/pbide/wig/wini86.dll/pbide.dll <OWRELROOT>/binw/pbide.dll
    <CPCMD> <DEVDIR>/pbide/cfg/wini86/pbide.cfg     <OWRELROOT>/binw/pbide.cfg
    <CPCMD> <DEVDIR>/pbide/cfg/wini86/pbidex.cfg    <OWRELROOT>/binw/pbidex.cfg
  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> <DEVDIR>/pbide/run/nt386/run.exe        <OWRELROOT>/binnt/watrun.exe
    <CPCMD> <DEVDIR>/pbide/dlldbg/nt386/dlldbg.exe  <OWRELROOT>/binnt/dlldbg.exe
    <CPCMD> <DEVDIR>/pbide/wig/nt386.dll/pbide.dll  <OWRELROOT>/binnt/pbide.dll
    <CPCMD> <DEVDIR>/pbide/dlldbg/pbwdnt.dbg        <OWRELROOT>/binnt/pbwdnt.dbg
    <CPCMD> <DEVDIR>/pbide/cfg/nt386/pbide.cfg      <OWRELROOT>/binnt/pbide.cfg
    <CPCMD> <DEVDIR>/pbide/cfg/nt386/pbidex.cfg     <OWRELROOT>/binnt/pbidex.cfg
    <CPCMD> <DEVDIR>/viper/cfg/idepbnt.cfg          <OWRELROOT>/binnt/idepbnt.cfg
  [ ENDIF ]
    <CPCMD> <DEVDIR>/pbide/dlldbg/pbend.dbg         <OWRELROOT>/binw/pbend.dbg
    <CPCMD> <DEVDIR>/pbide/dlldbg/pbstart.dbg       <OWRELROOT>/binw/pbstart.dbg
    <CPCMD> <DEVDIR>/pbide/dlldbg/pbwd.dbg          <OWRELROOT>/binw/pbwd.dbg
    <CPCMD> <DEVDIR>/pbide/pbdll.h                  <OWRELROOT>/h/pbdll.h
    <CPCMD> <DEVDIR>/viper/cfg/idepb.cfg            <OWRELROOT>/binw/idepb.cfg

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
