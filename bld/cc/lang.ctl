# cfe Builder Control file
# ==========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/bat/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
#=======================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> rel2 cprel2 acprel2 ]
#================================
  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> dos386.386/wcc386c.exe    <RELROOT>/binw/wcc386.exe
    <CPCMD> dos386.386/wcc386c.sym    <RELROOT>/binw/wcc386.sym
    <CPCMD> dos386.386/wcc38601.int   <RELROOT>/binw/wcc38601.int
    <CPCMD> dos386.i86/wcci86c.exe    <RELROOT>/binw/wcc.exe
    <CPCMD> dos386.i86/wcci86c.sym    <RELROOT>/binw/wcc.sym
    <CPCMD> dos386.i86/wcci8601.int   <RELROOT>/binw/wcci8601.int
#    <CPCMD> dos386.axp/wccaxpc.exe    <RELROOT>/binnt/wccaxp.exe

#  [ IFDEF (os_osi "") <2*> ]    #osi stuff
#    <CPCMD> osi386.386/wcc386c.exe    <RELROOT>/binw/wcc386.exe
#    <CPCMD> osi386.386/wcc386c.sym    <RELROOT>/binw/wcc386.sym
#    <CPCMD> osi386.386/wcc38601.int   <RELROOT>/binw/wcc38601.int
#    <CPCMD> osi386.i86/wcci86c.exe    <RELROOT>/binw/wcc.exe
#    <CPCMD> osi386.i86/wcci86c.sym    <RELROOT>/binw/wcc.sym
#    <CPCMD> osi386.i86/wcci8601.int   <RELROOT>/binw/wcci8601.int
#    <CPCMD> osi386.axp/wccaxpc.exe    <RELROOT>/binw/wccaxp.exe
#    <CPCMD> osi386.ppc/wccppcc.exe    <RELROOT>/binw/wccppc.exe

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> os2386.386/wcc386c.exe    <RELROOT>/binp/wcc386.exe
    <CPCMD> os2386.386/wcc386c.sym    <RELROOT>/binp/wcc386.sym
    <CPCMD> os2386.386/wccd386c.dll   <RELROOT>/binp/dll/wccd386.dll
    <CPCMD> os2386.386/wccd386c.sym   <RELROOT>/binp/dll/wccd386.sym
    <CPCMD> os2386.386/wcc38601.int   <RELROOT>/binp/dll/wcc38601.int
    <CPCMD> os2386.i86/wcci86c.exe    <RELROOT>/binp/wcc.exe
    <CPCMD> os2386.i86/wcci86c.sym    <RELROOT>/binp/wcc.sym
    <CPCMD> os2386.i86/wccdi86c.dll   <RELROOT>/binp/dll/wccd.dll
    <CPCMD> os2386.i86/wccdi86c.sym   <RELROOT>/binp/dll/wccd.sym
    <CPCMD> os2386.i86/wcci8601.int   <RELROOT>/binp/dll/wcci8601.int
    <CPCMD> os2386.axp/wccaxpc.exe    <RELROOT>/binp/wccaxp.exe
    <CPCMD> os2386.axp/wccaxpc.sym    <RELROOT>/binp/wccaxp.sym
    <CPCMD> os2386.axp/wccdaxpc.dll   <RELROOT>/binp/dll/wccdaxp.dll
    <CPCMD> os2386.axp/wccdaxpc.sym   <RELROOT>/binp/dll/wccdaxp.sym
    <CPCMD> os2386.axp/wccaxp01.int   <RELROOT>/binp/wccaxp01.int
    <CPCMD> os2386.ppc/wccppcc.exe    <RELROOT>/binp/wccppc.exe
    <CPCMD> os2386.ppc/wccppcc.sym    <RELROOT>/binp/wccppc.sym
    <CPCMD> os2386.ppc/wccdppcc.dll   <RELROOT>/binp/dll/wccdppc.dll
    <CPCMD> os2386.ppc/wccdppcc.sym   <RELROOT>/binp/dll/wccdppc.sym
    <CPCMD> os2386.ppc/wccppc01.int   <RELROOT>/binp/wccppc01.int
    <CPCMD> os2386.mps/wccmpsc.exe    <RELROOT>/binp/wccmps.exe
    <CPCMD> os2386.mps/wccmpsc.sym    <RELROOT>/binp/wccmps.sym
    <CPCMD> os2386.mps/wccdmpsc.dll   <RELROOT>/binp/dll/wccdmps.dll
    <CPCMD> os2386.mps/wccdmpsc.sym   <RELROOT>/binp/dll/wccdmps.sym
    <CPCMD> os2386.mps/wccmps01.int   <RELROOT>/binp/wccmps01.int

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386.386/wcc386c.exe     <RELROOT>/binnt/wcc386.exe
    <CPCMD> nt386.386/wcc386c.sym     <RELROOT>/binnt/wcc386.sym
    <CPCMD> nt386.386/wccd386c.dll    <RELROOT>/binnt/wccd386.dll
    <CPCMD> nt386.386/wccd386c.sym    <RELROOT>/binnt/wccd386.sym
    <CPCMD> nt386.386/wcc38601.int    <RELROOT>/binnt/wcc38601.int
    <CPCMD> nt386dll.386/wcc386c.exe  <RELROOT>/binnt/rtdll/wcc386.exe
    <CPCMD> nt386dll.386/wccd386c.dll <RELROOT>/binnt/rtdll/wccd386.dll
    <CPCMD> nt386dll.386/wccd386c.sym <RELROOT>/binnt/rtdll/wccd386.sym
    <CPCMD> nt386dll.386/wcc38601.int <RELROOT>/binnt/rtdll/wcc38601.int
    <CPCMD> nt386.i86/wcci86c.exe     <RELROOT>/binnt/wcc.exe
    <CPCMD> nt386.i86/wcci86c.sym     <RELROOT>/binnt/wcc.sym
    <CPCMD> nt386.i86/wccdi86c.dll    <RELROOT>/binnt/wccd.dll
    <CPCMD> nt386.i86/wccdi86c.sym    <RELROOT>/binnt/wccd.sym
    <CPCMD> nt386.i86/wcci8601.int    <RELROOT>/binnt/wcci8601.int
    <CPCMD> nt386.axp/wccaxpc.exe     <RELROOT>/binnt/wccaxp.exe
    <CPCMD> nt386.axp/wccaxpc.sym     <RELROOT>/binnt/wccaxp.sym
    <CPCMD> nt386.axp/wccdaxpc.dll    <RELROOT>/binnt/wccdaxp.dll
    <CPCMD> nt386.axp/wccdaxpc.sym    <RELROOT>/binnt/wccdaxp.sym
    <CPCMD> nt386.axp/wccaxp01.int    <RELROOT>/binnt/wccaxp01.int
    <CPCMD> nt386.ppc/wccppcc.exe     <RELROOT>/binnt/wccppc.exe
    <CPCMD> nt386.ppc/wccppcc.sym     <RELROOT>/binnt/wccppc.sym
    <CPCMD> nt386.ppc/wccdppcc.dll    <RELROOT>/binnt/wccdppc.dll
    <CPCMD> nt386.ppc/wccdppcc.sym    <RELROOT>/binnt/wccdppc.sym
    <CPCMD> nt386.ppc/wccppc01.int    <RELROOT>/binnt/wccppc01.int

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> linux386.386/wcc386c.exe  <RELROOT>/binl/wcc386
    <CPCMD> linux386.386/wcc386c.sym  <RELROOT>/binl/wcc386.sym
    <CPCMD> linux386.386/wcc38601.int <RELROOT>/binl/wcc38601.int
    <CPCMD> linux386.i86/wcci86c.exe  <RELROOT>/binl/wcc
    <CPCMD> linux386.i86/wcci86c.sym  <RELROOT>/binl/wcc.sym
    <CPCMD> linux386.i86/wcci8601.int <RELROOT>/binl/wcci8601.int
    <CPCMD> linux386.axp/wccaxpc.exe  <RELROOT>/binl/wccaxp
    <CPCMD> linux386.axp/wccaxpc.sym  <RELROOT>/binl/wccaxp.sym
  [ ENDIF ]

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> ntaxp.axp/wccaxpc.exe     <RELROOT>/axpnt/wccaxp.exe
    <CPCMD> ntaxp.axp/wccaxpc.sym     <RELROOT>/axpnt/wccaxp.sym
    <CPCMD> ntaxp.axp/wccaxp01.int    <RELROOT>/axpnt/wccaxp01.int
    <CPCMD> ntaxp.386/wcc386c.exe     <RELROOT>/axpnt/wcc386.exe
    <CPCMD> ntaxp.386/wcc386c.sym     <RELROOT>/axpnt/wcc386.sym
    <CPCMD> ntaxp.386/wcc38601.int    <RELROOT>/axpnt/wcc38601.int

  [ IFDEF (cpu_ppc) <2*> ]          # PPC target from 386 hosts
    <CPCMD> nt386.ppc/wccppcc.exe     <RELROOT>/binnt/wccppc.exe
    <CPCMD> nt386.ppc/wccppcc.sym     <RELROOT>/binnt/wccppc.sym
    <CPCMD> nt386.ppc/wccdppcc.dll    <RELROOT>/binnt/wccdppc.dll
    <CPCMD> nt386.ppc/wccdppcc.sym    <RELROOT>/binnt/wccdppc.sym
    <CPCMD> nt386.ppc/wccppc01.int    <RELROOT>/binnt/wccppc01.int
    <CPCMD> os2386.ppc/wccppcc.exe    <RELROOT>/binp/wccppc.exe
    <CPCMD> os2386.ppc/wccppcc.sym    <RELROOT>/binp/wccppc.sym
    <CPCMD> os2386.ppc/wccdppcc.dll   <RELROOT>/binp/dll/wccdppc.dll
    <CPCMD> os2386.ppc/wccdppcc.sym   <RELROOT>/binp/dll/wccdppc.sym
#    <CPCMD> ntppc.ppc/wccppcc.exe     <RELROOT>/ppcnt/wccppc.exe
#    <CPCMD> ntppc.ppc/wccppcc.sym     <RELROOT>/ppcnt/wccppc.sym
#    <CPCMD> ntppc.ppc/wccdppcc.dll    <RELROOT>/ppcnt/wccdppc.dll
#    <CPCMD> ntppc.ppc/wccdppcc.sym    <RELROOT>/ppcnt/wccdppc.sym
#    <CPCMD> ntppc.ppc/wccppc01.int    <RELROOT>/ppcnt/wccppc01.int
  [ ENDIF ]
  
  # MIPS target from 386 hosts
    <CCCMD> nt386.mps/wccmpsc.exe     <RELROOT>/binnt/wccmps.exe
    <CCCMD> nt386.mps/wccmpsc.sym     <RELROOT>/binnt/wccmps.sym
    <CCCMD> nt386.mps/wccdmpsc.dll    <RELROOT>/binnt/wccdmps.dll
    <CCCMD> nt386.mps/wccdmpsc.sym    <RELROOT>/binnt/wccdmps.sym
    <CCCMD> nt386.mps/wccmps01.int    <RELROOT>/binnt/wccmps01.int
    <CCCMD> os2386.mps/wccmpsc.exe    <RELROOT>/binp/wccmps.exe
    <CCCMD> os2386.mps/wccmpsc.sym    <RELROOT>/binp/wccmps.sym
    <CCCMD> os2386.mps/wccdmpsc.dll   <RELROOT>/binp/dll/wccdmps.dll
    <CCCMD> os2386.mps/wccdmpsc.sym   <RELROOT>/binp/dll/wccdmps.sym

  [ IFDEF (os_qnx) <2*> ]
    <CPCMD> qnx386.i86/wcci86c.exe    <RELROOT>/qnx/binq/wcc.
    <CPCMD> qnx386.i86/wcci86c.sym    <RELROOT>/qnx/sym/wcc.sym
    <CPCMD> qnx386.386/wcc386c.exe    <RELROOT>/qnx/binq/wcc386.
    <CPCMD> qnx386.386/wcc386c.sym    <RELROOT>/qnx/sym/wcc.sym

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
