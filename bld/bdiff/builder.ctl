# bpatch Builder Control file
# ===========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
#=======================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

    # Make BPATCH.QNX for Database guys. Easier than getting them set up
#    wmake -h bpatch.qnx

[ BLOCK <1> rel2 ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> rel2 cprel2 ]
#========================
  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> dos386/bdiff.exe     <OWRELROOT>/binw/bdiff.exe
    <CPCMD> dos386/bpatch.exe    <OWRELROOT>/binw/bpatch.exe
  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> os2386/bdiff.exe     <OWRELROOT>/binp/bdiff.exe
    <CPCMD> os2386/bpatch.exe    <OWRELROOT>/binp/bpatch.exe
  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/bdiff.exe      <OWRELROOT>/binnt/bdiff.exe
    <CPCMD> nt386/bpatch.exe     <OWRELROOT>/binnt/bpatch.exe

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
