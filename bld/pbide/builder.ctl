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
    <CPCMD> <SRCDIR>/pbide/fscopy/wfscopy.exe       <OWRELROOT>/binw/wfscopy.exe
  [ IFDEF (os_win "") <2*> ]
    <CPCMD> <SRCDIR>/pbide/run/wini86/run.exe       <OWRELROOT>/binw/watrun.exe
    <CPCMD> <SRCDIR>/pbide/dlldbg/wini86/dlldbg.exe <OWRELROOT>/binw/dlldbg.exe
    <CPCMD> <SRCDIR>/pbide/wig/wini86.dll/pbide.dll <OWRELROOT>/binw/pbide.dll
    <CPCMD> <SRCDIR>/pbide/cfg/wini86/pbide.cfg     <OWRELROOT>/binw/pbide.cfg
    <CPCMD> <SRCDIR>/pbide/cfg/wini86/pbidex.cfg    <OWRELROOT>/binw/pbidex.cfg
  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> <SRCDIR>/pbide/run/nt386/run.exe        <OWRELROOT>/binnt/watrun.exe
    <CPCMD> <SRCDIR>/pbide/dlldbg/nt386/dlldbg.exe  <OWRELROOT>/binnt/dlldbg.exe
    <CPCMD> <SRCDIR>/pbide/wig/nt386.dll/pbide.dll  <OWRELROOT>/binnt/pbide.dll
    <CPCMD> <SRCDIR>/pbide/dlldbg/pbwdnt.dbg        <OWRELROOT>/binnt/pbwdnt.dbg
    <CPCMD> <SRCDIR>/pbide/cfg/nt386/pbide.cfg      <OWRELROOT>/binnt/pbide.cfg
    <CPCMD> <SRCDIR>/pbide/cfg/nt386/pbidex.cfg     <OWRELROOT>/binnt/pbidex.cfg
    <CPCMD> <SRCDIR>/viper/cfg/idepbnt.cfg          <OWRELROOT>/binnt/idepbnt.cfg
  [ ENDIF ]
    <CPCMD> <SRCDIR>/pbide/dlldbg/pbend.dbg         <OWRELROOT>/binw/pbend.dbg
    <CPCMD> <SRCDIR>/pbide/dlldbg/pbstart.dbg       <OWRELROOT>/binw/pbstart.dbg
    <CPCMD> <SRCDIR>/pbide/dlldbg/pbwd.dbg          <OWRELROOT>/binw/pbwd.dbg
    <CPCMD> <SRCDIR>/pbide/pbdll.h                  <OWRELROOT>/h/pbdll.h
    <CPCMD> <SRCDIR>/viper/cfg/idepb.cfg            <OWRELROOT>/binw/idepb.cfg

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
