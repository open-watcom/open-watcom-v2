# EditDLL Builder Control file
# ============================

set PROJDIR=<CWD>
set PROJNAME=editdll

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel cprel ]
#======================
  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> epm/os2386/epmlink.dll     <OWRELROOT>/binp/dll/epmlink.dll
    <CPCMD> epm/os2386/epmlink.sym     <OWRELROOT>/binp/dll/epmlink.sym
    <CPCMD> epm/os2386/wedit.lib       <OWRELROOT>/src/editdll/os2/wedit.lib
    <CPCMD> epm/os2386/wedit.dll       <OWRELROOT>/src/editdll/os2/wedit.dll

  [ IFDEF (os_win "") <2*> ]

    <CPCMD> viw/wini86/weditviw.dll    <OWRELROOT>/binw/weditviw.dll
    <CPCMD> viw/wini86/weditviw.sym    <OWRELROOT>/binw/weditviw.sym
    <CPCMD> cw/wini86/weditcw.dll      <OWRELROOT>/binw/weditcw.dll
    <CPCMD> viw/wini86/wedit.lib       <OWRELROOT>/src/editdll/win/wedit.lib
    <CPCMD> cw/wini86/wedit.dll        <OWRELROOT>/src/editdll/win/cw/wedit.dll
    <CPCMD> viw/wini86/wedit.dll       <OWRELROOT>/src/editdll/win/viw/wedit.dll

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> viw/nt386/weditviw.dll     <OWRELROOT>/binnt/weditviw.dll
    <CPCMD> viw/nt386/weditviw.sym     <OWRELROOT>/binnt/weditviw.sym
    <CPCMD> cw/nt386/weditcw.dll       <OWRELROOT>/binnt/weditcw.dll
    <CPCMD> viw/nt386/wedit.lib        <OWRELROOT>/src/editdll/nt/wedit.lib
    <CPCMD> cw/nt386/wedit.dll         <OWRELROOT>/src/editdll/nt/cw/wedit.dll
    <CPCMD> viw/nt386/wedit.dll        <OWRELROOT>/src/editdll/nt/viw/wedit.dll

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> viw/ntaxp/weditviw.dll     <OWRELROOT>/axpnt/weditviw.dll
    <CPCMD> viw/ntaxp/weditviw.sym     <OWRELROOT>/axpnt/weditviw.sym
  [ ENDIF ]

    <CPCMD> wedit.h                    <OWRELROOT>/src/editdll/wedit.h
    <CPCMD> wedit.doc                  <OWRELROOT>/src/editdll/wedit.doc


[ BLOCK . . ]
#============
cdsay <PROJDIR>
