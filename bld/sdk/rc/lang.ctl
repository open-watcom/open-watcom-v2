# RC Builder Control file
# =======================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/bat/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
#=======================
    cdsay <PROJDIR>/wres
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h
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
  <CPCMD> <PROJDIR>/rc/*.uni                    <RELROOT>/binw/
  <CPCMD> <PROJDIR>/rc/*.uni                    <RELROOT>/binl/

  [ IFDEF (os_osi) <2*> ]
#    <CPCMD> <PROJDIR>/rc/osi386/wrce.exe        <RELROOT>/binw/wrc.exe

  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> <PROJDIR>/rc/dos386/wrce.exe        <RELROOT>/binw/wrc.exe
    <CPCMD> <PROJDIR>/rc/dos386/wrce.sym        <RELROOT>/binw/wrc.sym

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> <PROJDIR>/rc/os2386.dll/wrce.exe    <RELROOT>/binp/wrc.exe
    <CPCMD> <PROJDIR>/rc/os2386.dll/wrce.sym    <RELROOT>/binp/wrc.sym
    <CPCMD> <PROJDIR>/rc/os2386.dll/wrcde.dll   <RELROOT>/binp/dll/wrcd.dll
    <CPCMD> <PROJDIR>/rc/os2386.dll/wrcde.sym   <RELROOT>/binp/dll/wrcd.sym
    <CPCMD> <PROJDIR>/exedmp/os2386/exedmp.exe  <RELROOT>/binp/exedmp.exe

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> <PROJDIR>/rc/nt386.dll/wrce.exe     <RELROOT>/binnt/wrc.exe
    <CPCMD> <PROJDIR>/rc/nt386.dll/wrce.sym     <RELROOT>/binnt/wrc.sym
    <CPCMD> <PROJDIR>/rc/nt386.dll/wrcde.dll    <RELROOT>/binnt/wrcd.dll
    <CPCMD> <PROJDIR>/rc/nt386.dll/wrcde.sym    <RELROOT>/binnt/wrcd.sym
    <CPCMD> <PROJDIR>/rc/nt386rt.dll/wrce.exe   <RELROOT>/binnt/rtdll/wrc.exe
    <CPCMD> <PROJDIR>/rc/nt386rt.dll/wrce.sym   <RELROOT>/binnt/rtdll/wrc.sym
    <CPCMD> <PROJDIR>/rc/nt386rt.dll/wrcde.dll  <RELROOT>/binnt/rtdll/wrcd.dll
    <CPCMD> <PROJDIR>/rc/nt386rt.dll/wrcde.sym  <RELROOT>/binnt/rtdll/wrcd.sym
    <CPCMD> <PROJDIR>/mkcdpg/nt386/mkcdpg.exe   <RELROOT>/binnt/mkcdpg.exe
    <CPCMD> <PROJDIR>/exedmp/nt386/exedmp.exe   <RELROOT>/binnt/exedmp.exe

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> <PROJDIR>/rc/ntaxp/wrce.exe         <RELROOT>/axpnt/wrc.exe
    <CPCMD> <PROJDIR>/mkcdpg/ntaxp/mkcdpg.exe   <RELROOT>/axpnt/mkcdpg.exe

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> <PROJDIR>/rc/linux386/wrce.exe      <RELROOT>/binl/wrc
    <CPCMD> <PROJDIR>/rc/linux386/wrce.sym      <RELROOT>/binl/wrc.sym

[ BLOCK <1> clean ]
#==================
    cdsay <PROJDIR>/wres
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean
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
