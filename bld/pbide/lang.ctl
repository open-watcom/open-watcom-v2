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
    <CPCMD> <DEVDIR>/pbide/fscopy/wfscopy.exe       <RELROOT>/binw/wfscopy.exe
  [ IFDEF (os_win "") <2*> ]
    <CPCMD> <DEVDIR>/pbide/run/wini86/run.exe       <RELROOT>/binw/watrun.exe
    <CPCMD> <DEVDIR>/pbide/dlldbg/wini86/dlldbg.exe <RELROOT>/binw/dlldbg.exe
    <CPCMD> <DEVDIR>/pbide/wig/wini86.dll/pbide.dll <RELROOT>/binw/pbide.dll
    <CPCMD> <DEVDIR>/pbide/cfg/wini86/pbide.cfg     <RELROOT>/binw/pbide.cfg
    <CPCMD> <DEVDIR>/pbide/cfg/wini86/pbidex.cfg    <RELROOT>/binw/pbidex.cfg
  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> <DEVDIR>/pbide/run/nt386/run.exe        <RELROOT>/binnt/watrun.exe
    <CPCMD> <DEVDIR>/pbide/dlldbg/nt386/dlldbg.exe  <RELROOT>/binnt/dlldbg.exe
    <CPCMD> <DEVDIR>/pbide/wig/nt386.dll/pbide.dll  <RELROOT>/binnt/pbide.dll
    <CPCMD> <DEVDIR>/pbide/dlldbg/pbwdnt.dbg        <RELROOT>/binnt/pbwdnt.dbg
    <CPCMD> <DEVDIR>/pbide/cfg/nt386/pbide.cfg      <RELROOT>/binnt/pbide.cfg
    <CPCMD> <DEVDIR>/pbide/cfg/nt386/pbidex.cfg     <RELROOT>/binnt/pbidex.cfg
    <CPCMD> <DEVDIR>/viper/cfg/idepbnt.cfg          <RELROOT>/binnt/idepbnt.cfg
  [ ENDIF ]
    <CPCMD> <DEVDIR>/pbide/dlldbg/pbend.dbg         <RELROOT>/binw/pbend.dbg
    <CPCMD> <DEVDIR>/pbide/dlldbg/pbstart.dbg       <RELROOT>/binw/pbstart.dbg
    <CPCMD> <DEVDIR>/pbide/dlldbg/pbwd.dbg          <RELROOT>/binw/pbwd.dbg
    <CPCMD> <DEVDIR>/pbide/pbdll.h                  <RELROOT>/h/pbdll.h
    <CPCMD> <DEVDIR>/viper/cfg/idepb.cfg            <RELROOT>/binw/idepb.cfg

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
