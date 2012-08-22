# RC Builder Control file
# =======================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
#=======================
    cdsay <PROJDIR>/mkcdpg
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h
    cdsay <PROJDIR>/exedmp
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h
    cdsay <PROJDIR>/rc
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h
    cdsay <PROJDIR>/restest
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h
    cdsay <PROJDIR>/wresdmp
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> rel2 cprel2 ]
#========================
  <CPCMD> rc/*.uni                    <OWRELROOT>/binw/
  <CPCMD> rc/*.uni                    <OWRELROOT>/binl/

  [ IFDEF (os_osi) <2*> ]
#    <CPCMD> rc/osi386/wrce.exe        <OWRELROOT>/binw/wrc.exe

  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> rc/dos386/wrce.exe        <OWRELROOT>/binw/wrc.exe
    <CPCMD> rc/dos386/wrce.sym        <OWRELROOT>/binw/wrc.sym

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> rc/os2386.dll/wrce.exe    <OWRELROOT>/binp/wrc.exe
    <CPCMD> rc/os2386.dll/wrce.sym    <OWRELROOT>/binp/wrc.sym
    <CPCMD> rc/os2386.dll/wrcde.dll   <OWRELROOT>/binp/dll/wrcd.dll
    <CPCMD> rc/os2386.dll/wrcde.sym   <OWRELROOT>/binp/dll/wrcd.sym
    <CPCMD> exedmp/os2386/exedmp.exe  <OWRELROOT>/binp/exedmp.exe

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> rc/nt386.dll/wrce.exe     <OWRELROOT>/binnt/wrc.exe
    <CPCMD> rc/nt386.dll/wrce.sym     <OWRELROOT>/binnt/wrc.sym
    <CPCMD> rc/nt386.dll/wrcde.dll    <OWRELROOT>/binnt/wrcd.dll
    <CPCMD> rc/nt386.dll/wrcde.sym    <OWRELROOT>/binnt/wrcd.sym
    <CPCMD> rc/nt386rt.dll/wrce.exe   <OWRELROOT>/binnt/rtdll/wrc.exe
    <CPCMD> rc/nt386rt.dll/wrce.sym   <OWRELROOT>/binnt/rtdll/wrc.sym
    <CPCMD> rc/nt386rt.dll/wrcde.dll  <OWRELROOT>/binnt/rtdll/wrcd.dll
    <CPCMD> rc/nt386rt.dll/wrcde.sym  <OWRELROOT>/binnt/rtdll/wrcd.sym
    <CPCMD> mkcdpg/nt386/mkcdpg.exe   <OWRELROOT>/binnt/mkcdpg.exe
    <CPCMD> exedmp/nt386/exedmp.exe   <OWRELROOT>/binnt/exedmp.exe

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> rc/ntaxp/wrce.exe         <OWRELROOT>/axpnt/wrc.exe
    <CPCMD> mkcdpg/ntaxp/mkcdpg.exe   <OWRELROOT>/axpnt/mkcdpg.exe

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> rc/linux386/wrce.exe      <OWRELROOT>/binl/wrc
    <CPCMD> rc/linux386/wrce.sym      <OWRELROOT>/binl/wrc.sym

[ BLOCK <1> clean ]
#==================
    cdsay <PROJDIR>/mkcdpg
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean
    cdsay <PROJDIR>/exedmp
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean
    cdsay <PROJDIR>/rc
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean
    cdsay <PROJDIR>/restest
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean
    cdsay <PROJDIR>/wresdmp
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
