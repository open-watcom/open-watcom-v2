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
    <CCCMD> epm/os2386/epmlink.dll     <OWRELROOT>/binp/dll/
    <CCCMD> epm/os2386/epmlink.sym     <OWRELROOT>/binp/dll/
    <CCCMD> epm/os2386/wedit.lib       <OWRELROOT>/src/editdll/os2/
    <CCCMD> epm/os2386/wedit.dll       <OWRELROOT>/src/editdll/os2/

    <CCCMD> viw/wini86/weditviw.dll    <OWRELROOT>/binw/
    <CCCMD> viw/wini86/weditviw.sym    <OWRELROOT>/binw/
    <CCCMD> cw/wini86/weditcw.dll      <OWRELROOT>/binw/
    <CCCMD> viw/wini86/wedit.lib       <OWRELROOT>/src/editdll/win/
    <CCCMD> cw/wini86/wedit.dll        <OWRELROOT>/src/editdll/win/cw/
    <CCCMD> viw/wini86/wedit.dll       <OWRELROOT>/src/editdll/win/viw/

    <CCCMD> viw/nt386/weditviw.dll     <OWRELROOT>/binnt/
    <CCCMD> viw/nt386/weditviw.sym     <OWRELROOT>/binnt/
    <CCCMD> cw/nt386/weditcw.dll       <OWRELROOT>/binnt/
    <CCCMD> viw/nt386/wedit.lib        <OWRELROOT>/src/editdll/nt/
    <CCCMD> cw/nt386/wedit.dll         <OWRELROOT>/src/editdll/nt/cw/
    <CCCMD> viw/nt386/wedit.dll        <OWRELROOT>/src/editdll/nt/viw/

    <CCCMD> viw/ntaxp/weditviw.dll     <OWRELROOT>/axpnt/
    <CCCMD> viw/ntaxp/weditviw.sym     <OWRELROOT>/axpnt/

    <CPCMD> wedit.h                    <OWRELROOT>/src/editdll/
    <CPCMD> wedit.doc                  <OWRELROOT>/src/editdll/

    <CCCMD> viw/ntx64/weditviw.dll     <OWRELROOT>/binnt64/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
