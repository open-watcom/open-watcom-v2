# TODO: Restructure directories, redo makefiles. This project is a mess.
#       Parts of it seem obsolete (wfscopy).
#
# PBIDE Builder Control file
# ==========================

set PROJDIR=<CWD>
set PROJNAME=pbide

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> fscopy/wfscopy.exe                  <OWRELROOT>/binw/wfscopy.exe
  [ IFDEF (os_win "") <2*> ]
    <CPCMD> run/wini86/run.exe                  <OWRELROOT>/binw/watrun.exe
    <CPCMD> dlldbg/wini86/dlldbg.exe            <OWRELROOT>/binw/dlldbg.exe
    <CPCMD> wig/wini86.dll/pbide.dll            <OWRELROOT>/binw/pbide.dll
    <CPCMD> cfg/wini86/pbide.cfg                <OWRELROOT>/binw/pbide.cfg
    <CPCMD> cfg/wini86/pbidex.cfg               <OWRELROOT>/binw/pbidex.cfg
  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> run/nt386/run.exe                   <OWRELROOT>/binnt/watrun.exe
    <CPCMD> dlldbg/nt386/dlldbg.exe             <OWRELROOT>/binnt/dlldbg.exe
    <CPCMD> wig/nt386.dll/pbide.dll             <OWRELROOT>/binnt/pbide.dll
    <CPCMD> dlldbg/pbwdnt.dbg                   <OWRELROOT>/binnt/pbwdnt.dbg
    <CPCMD> cfg/nt386/pbide.cfg                 <OWRELROOT>/binnt/pbide.cfg
    <CPCMD> cfg/nt386/pbidex.cfg                <OWRELROOT>/binnt/pbidex.cfg
    <CPCMD> <OWSRCDIR>/ide/cfg/idepbnt.cfg      <OWRELROOT>/binnt/idepbnt.cfg
  [ ENDIF ]
    <CPCMD> dlldbg/pbend.dbg                    <OWRELROOT>/binw/pbend.dbg
    <CPCMD> dlldbg/pbstart.dbg                  <OWRELROOT>/binw/pbstart.dbg
    <CPCMD> dlldbg/pbwd.dbg                     <OWRELROOT>/binw/pbwd.dbg
    <CPCMD> pbdll.h                             <OWRELROOT>/h/pbdll.h
    <CPCMD> <OWSRCDIR>/ide/cfg/idepb.cfg        <OWRELROOT>/binw/idepb.cfg

[ BLOCK . . ]
#============
cdsay <PROJDIR>
