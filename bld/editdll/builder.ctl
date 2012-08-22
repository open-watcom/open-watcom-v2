# EditDLL Builder Control file
# ============================

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
  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> epm/os2386/epmlink.dll     <RELROOT>/binp/dll/epmlink.dll
    <CPCMD> epm/os2386/epmlink.sym     <RELROOT>/binp/dll/epmlink.sym
    <CPCMD> epm/os2386/wedit.lib       <RELROOT>/src/editdll/os2/wedit.lib
    <CPCMD> epm/os2386/wedit.dll       <RELROOT>/src/editdll/os2/wedit.dll

  [ IFDEF (os_win "") <2*> ]

    <CPCMD> viw/wini86/weditviw.dll    <RELROOT>/binw/weditviw.dll
    <CPCMD> viw/wini86/weditviw.sym    <RELROOT>/binw/weditviw.sym
    <CPCMD> cw/wini86/weditcw.dll      <RELROOT>/binw/weditcw.dll
    <CPCMD> viw/wini86/wedit.lib       <RELROOT>/src/editdll/win/wedit.lib
    <CPCMD> cw/wini86/wedit.dll        <RELROOT>/src/editdll/win/cw/wedit.dll
    <CPCMD> viw/wini86/wedit.dll       <RELROOT>/src/editdll/win/viw/wedit.dll

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> viw/nt386/weditviw.dll     <RELROOT>/binnt/weditviw.dll
    <CPCMD> viw/nt386/weditviw.sym     <RELROOT>/binnt/weditviw.sym
    <CPCMD> cw/nt386/weditcw.dll       <RELROOT>/binnt/weditcw.dll
    <CPCMD> viw/nt386/wedit.lib        <RELROOT>/src/editdll/nt/wedit.lib
    <CPCMD> cw/nt386/wedit.dll         <RELROOT>/src/editdll/nt/cw/wedit.dll
    <CPCMD> viw/nt386/wedit.dll        <RELROOT>/src/editdll/nt/viw/wedit.dll

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> viw/ntaxp/weditviw.dll     <RELROOT>/axpnt/weditviw.dll
    <CPCMD> viw/ntaxp/weditviw.sym     <RELROOT>/axpnt/weditviw.sym
  [ ENDIF ]

    <CPCMD> wedit.h                    <RELROOT>/src/editdll/wedit.h
    <CPCMD> wedit.doc                  <RELROOT>/src/editdll/wedit.doc


[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
