# EditDLL Builder Control file
# ============================

set PROJDIR=<CWD>
set PROJNAME=editdll

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> epm/os2386/epmlink.dll     <OWRELROOT>/binp/dll/epmlink.dll
    <CCCMD> epm/os2386/epmlink.sym     <OWRELROOT>/binp/dll/epmlink.sym
    <CCCMD> epm/os2386/wedit.lib       <OWRELROOT>/src/editdll/os2/wedit.lib
    <CCCMD> epm/os2386/wedit.dll       <OWRELROOT>/src/editdll/os2/wedit.dll

    <CCCMD> viw/wini86/weditviw.dll    <OWRELROOT>/binw/weditviw.dll
    <CCCMD> viw/wini86/weditviw.sym    <OWRELROOT>/binw/weditviw.sym
    <CCCMD> cw/wini86/weditcw.dll      <OWRELROOT>/binw/weditcw.dll
    <CCCMD> viw/wini86/wedit.lib       <OWRELROOT>/src/editdll/win/wedit.lib
    <CCCMD> cw/wini86/wedit.dll        <OWRELROOT>/src/editdll/win/cw/wedit.dll
    <CCCMD> viw/wini86/wedit.dll       <OWRELROOT>/src/editdll/win/viw/wedit.dll

    <CCCMD> viw/nt386/weditviw.dll     <OWRELROOT>/binnt/weditviw.dll
    <CCCMD> viw/nt386/weditviw.sym     <OWRELROOT>/binnt/weditviw.sym
    <CCCMD> cw/nt386/weditcw.dll       <OWRELROOT>/binnt/weditcw.dll
    <CCCMD> viw/nt386/wedit.lib        <OWRELROOT>/src/editdll/nt/wedit.lib
    <CCCMD> cw/nt386/wedit.dll         <OWRELROOT>/src/editdll/nt/cw/wedit.dll
    <CCCMD> viw/nt386/wedit.dll        <OWRELROOT>/src/editdll/nt/viw/wedit.dll

    <CCCMD> viw/ntaxp/weditviw.dll     <OWRELROOT>/axpnt/weditviw.dll
    <CCCMD> viw/ntaxp/weditviw.sym     <OWRELROOT>/axpnt/weditviw.sym

    <CPCMD> wedit.h                    <OWRELROOT>/src/editdll/wedit.h
    <CPCMD> wedit.doc                  <OWRELROOT>/src/editdll/wedit.doc

[ BLOCK . . ]
#============
cdsay <PROJDIR>
