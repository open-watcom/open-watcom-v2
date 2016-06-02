# TODO: Restructure directories, redo makefiles. This project is a mess.
#       Parts of it seem obsolete (wfscopy).
#
# PBIDE Builder Control file
# ==========================

set PROJNAME=pbide

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
#================
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> fscopy/wfscopy.exe                  <OWRELROOT>/binw/wfscopy.exe
    <CCCMD> run/wini86/run.exe                  <OWRELROOT>/binw/watrun.exe
    <CCCMD> dlldbg/wini86/dlldbg.exe            <OWRELROOT>/binw/dlldbg.exe
    <CCCMD> wig/wini86.dll/pbide.dll            <OWRELROOT>/binw/pbide.dll
    <CCCMD> cfg/wini86/pbide.cfg                <OWRELROOT>/binw/pbide.cfg
    <CCCMD> cfg/wini86/pbidex.cfg               <OWRELROOT>/binw/pbidex.cfg
    <CCCMD> run/nt386/run.exe                   <OWRELROOT>/binnt/watrun.exe
    <CCCMD> dlldbg/nt386/dlldbg.exe             <OWRELROOT>/binnt/dlldbg.exe
    <CCCMD> wig/nt386.dll/pbide.dll             <OWRELROOT>/binnt/pbide.dll
    <CCCMD> dlldbg/pbwdnt.dbg                   <OWRELROOT>/binnt/pbwdnt.dbg
    <CCCMD> cfg/nt386/pbide.cfg                 <OWRELROOT>/binnt/pbide.cfg
    <CCCMD> cfg/nt386/pbidex.cfg                <OWRELROOT>/binnt/pbidex.cfg
    <CCCMD> <OWSRCDIR>/ide/cfg/idepbnt.cfg      <OWRELROOT>/binnt/idepbnt.cfg

    <CPCMD> dlldbg/pbend.dbg                    <OWRELROOT>/binw/pbend.dbg
    <CPCMD> dlldbg/pbstart.dbg                  <OWRELROOT>/binw/pbstart.dbg
    <CPCMD> dlldbg/pbwd.dbg                     <OWRELROOT>/binw/pbwd.dbg
    <CPCMD> pbdll.h                             <OWRELROOT>/h/pbdll.h
    <CPCMD> <OWSRCDIR>/ide/cfg/idepb.cfg        <OWRELROOT>/binw/idepb.cfg

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
