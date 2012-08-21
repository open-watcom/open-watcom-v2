# Browser Builder Control file
# ============================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
#=======================
    # pmake priorities are used to build:
    # 1) dlgprs/o
    # 2) gen
    # 3) everywhere else.
    #
    # gen is dependent on dlgprs/o
    # the os_dos dlgprs/o and gen are dependent on windows.h and not selected.
    # brg/* are independent of dlgprs/o and gen
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h


[ BLOCK <1> rel2 ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> rel2 cprel2 ]
#========================
  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> brg/dos386/wbrg.exe    <RELROOT>/binw/wbrg.exe

  [ IFDEF (os_win "") <2*> ]
    <CPCMD> wini86/wbrw.exe        <RELROOT>/binw/wbrw.exe

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> os2386/wbrw.exe        <RELROOT>/binp/wbrw.exe
    <CPCMD> brg/os2386/wbrg.exe    <RELROOT>/binp/wbrg.exe

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/wbrw.exe         <RELROOT>/binnt/wbrw.exe
    <CPCMD> brg/nt386/wbrg.exe     <RELROOT>/binnt/wbrg.exe

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> axpnt/wbrw.exe         <RELROOT>/axpnt/wbrw.exe
    <CPCMD> brg/ntaxp/wbrg.exe     <RELROOT>/axpnt/wbrg.exe

[ BLOCK <1> clean ]
#==================
   pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
