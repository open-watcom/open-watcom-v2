# TODO: Restructure directories, redo makefiles. This project is a mess.
#       Parts of it seem obsolete (wfscopy).
#
# PBIDE Builder Control file
# ==========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel ]
#======================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel ]
#================
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> <OWSRCDIR>/pbide/fscopy/wfscopy.exe       <OWRELROOT>/binw/wfscopy.exe
  [ IFDEF (os_win "") <2*> ]
    <CPCMD> <OWSRCDIR>/pbide/run/wini86/run.exe       <OWRELROOT>/binw/watrun.exe
    <CPCMD> <OWSRCDIR>/pbide/dlldbg/wini86/dlldbg.exe <OWRELROOT>/binw/dlldbg.exe
    <CPCMD> <OWSRCDIR>/pbide/wig/wini86.dll/pbide.dll <OWRELROOT>/binw/pbide.dll
    <CPCMD> <OWSRCDIR>/pbide/cfg/wini86/pbide.cfg     <OWRELROOT>/binw/pbide.cfg
    <CPCMD> <OWSRCDIR>/pbide/cfg/wini86/pbidex.cfg    <OWRELROOT>/binw/pbidex.cfg
  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> <OWSRCDIR>/pbide/run/nt386/run.exe        <OWRELROOT>/binnt/watrun.exe
    <CPCMD> <OWSRCDIR>/pbide/dlldbg/nt386/dlldbg.exe  <OWRELROOT>/binnt/dlldbg.exe
    <CPCMD> <OWSRCDIR>/pbide/wig/nt386.dll/pbide.dll  <OWRELROOT>/binnt/pbide.dll
    <CPCMD> <OWSRCDIR>/pbide/dlldbg/pbwdnt.dbg        <OWRELROOT>/binnt/pbwdnt.dbg
    <CPCMD> <OWSRCDIR>/pbide/cfg/nt386/pbide.cfg      <OWRELROOT>/binnt/pbide.cfg
    <CPCMD> <OWSRCDIR>/pbide/cfg/nt386/pbidex.cfg     <OWRELROOT>/binnt/pbidex.cfg
    <CPCMD> <OWSRCDIR>/viper/cfg/idepbnt.cfg          <OWRELROOT>/binnt/idepbnt.cfg
  [ ENDIF ]
    <CPCMD> <OWSRCDIR>/pbide/dlldbg/pbend.dbg         <OWRELROOT>/binw/pbend.dbg
    <CPCMD> <OWSRCDIR>/pbide/dlldbg/pbstart.dbg       <OWRELROOT>/binw/pbstart.dbg
    <CPCMD> <OWSRCDIR>/pbide/dlldbg/pbwd.dbg          <OWRELROOT>/binw/pbwd.dbg
    <CPCMD> <OWSRCDIR>/pbide/pbdll.h                  <OWRELROOT>/h/pbdll.h
    <CPCMD> <OWSRCDIR>/viper/cfg/idepb.cfg            <OWRELROOT>/binw/idepb.cfg

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
