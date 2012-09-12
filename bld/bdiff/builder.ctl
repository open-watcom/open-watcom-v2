# bpatch Builder Control file
# ===========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

cdsay .

    # Make BPATCH.QNX for Database guys. Easier than getting them set up
#    wmake -h bpatch.qnx

[ BLOCK <1> rel cprel ]
#======================
  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> dos386/bdiff.exe     <OWRELROOT>/binw/bdiff.exe
    <CPCMD> dos386/bpatch.exe    <OWRELROOT>/binw/bpatch.exe
  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> os2386/bdiff.exe     <OWRELROOT>/binp/bdiff.exe
    <CPCMD> os2386/bpatch.exe    <OWRELROOT>/binp/bpatch.exe
  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/bdiff.exe      <OWRELROOT>/binnt/bdiff.exe
    <CPCMD> nt386/bpatch.exe     <OWRELROOT>/binnt/bpatch.exe

[ BLOCK . . ]
#============
cdsay <PROJDIR>
