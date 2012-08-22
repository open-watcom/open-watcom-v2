# cfe Builder Control file
# ==========================

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

[ BLOCK <1> rel2 cprel2 acprel2 ]
#================================
  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> 386/dos386/wcc386c.exe    <OWRELROOT>/binw/wcc386.exe
    <CPCMD> 386/dos386/wcc386c.sym    <OWRELROOT>/binw/wcc386.sym
    <CPCMD> 386/dos386/wcc38601.int   <OWRELROOT>/binw/wcc38601.int
    <CPCMD> i86/dos386/wcci86c.exe    <OWRELROOT>/binw/wcc.exe
    <CPCMD> i86/dos386/wcci86c.sym    <OWRELROOT>/binw/wcc.sym
    <CPCMD> i86/dos386/wcci8601.int   <OWRELROOT>/binw/wcci8601.int
#    <CPCMD> axp/dos386/wccaxpc.exe    <OWRELROOT>/binnt/wccaxp.exe

#  [ IFDEF (os_osi "") <2*> ]    #osi stuff
#    <CPCMD> 386/osi386/wcc386c.exe    <OWRELROOT>/binw/wcc386.exe
#    <CPCMD> 386/osi386/wcc386c.sym    <OWRELROOT>/binw/wcc386.sym
#    <CPCMD> 386/osi386/wcc38601.int   <OWRELROOT>/binw/wcc38601.int
#    <CPCMD> i86/osi386/wcci86c.exe    <OWRELROOT>/binw/wcc.exe
#    <CPCMD> i86/osi386/wcci86c.sym    <OWRELROOT>/binw/wcc.sym
#    <CPCMD> i86/osi386/wcci8601.int   <OWRELROOT>/binw/wcci8601.int
#    <CPCMD> axp/osi386/wccaxpc.exe    <OWRELROOT>/binw/wccaxp.exe
#    <CPCMD> ppc/osi386/wccppcc.exe    <OWRELROOT>/binw/wccppc.exe

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> 386/os2386/wcc386c.exe    <OWRELROOT>/binp/wcc386.exe
    <CPCMD> 386/os2386/wcc386c.sym    <OWRELROOT>/binp/wcc386.sym
    <CPCMD> 386/os2386/wccd386c.dll   <OWRELROOT>/binp/dll/wccd386.dll
    <CPCMD> 386/os2386/wccd386c.sym   <OWRELROOT>/binp/dll/wccd386.sym
    <CPCMD> 386/os2386/wcc38601.int   <OWRELROOT>/binp/dll/wcc38601.int
    <CPCMD> i86/os2386/wcci86c.exe    <OWRELROOT>/binp/wcc.exe
    <CPCMD> i86/os2386/wcci86c.sym    <OWRELROOT>/binp/wcc.sym
    <CPCMD> i86/os2386/wccdi86c.dll   <OWRELROOT>/binp/dll/wccd.dll
    <CPCMD> i86/os2386/wccdi86c.sym   <OWRELROOT>/binp/dll/wccd.sym
    <CPCMD> i86/os2386/wcci8601.int   <OWRELROOT>/binp/dll/wcci8601.int
    <CPCMD> axp/os2386/wccaxpc.exe    <OWRELROOT>/binp/wccaxp.exe
    <CPCMD> axp/os2386/wccaxpc.sym    <OWRELROOT>/binp/wccaxp.sym
    <CPCMD> axp/os2386/wccdaxpc.dll   <OWRELROOT>/binp/dll/wccdaxp.dll
    <CPCMD> axp/os2386/wccdaxpc.sym   <OWRELROOT>/binp/dll/wccdaxp.sym
    <CPCMD> axp/os2386/wccaxp01.int   <OWRELROOT>/binp/wccaxp01.int
    <CPCMD> ppc/os2386/wccppcc.exe    <OWRELROOT>/binp/wccppc.exe
    <CPCMD> ppc/os2386/wccppcc.sym    <OWRELROOT>/binp/wccppc.sym
    <CPCMD> ppc/os2386/wccdppcc.dll   <OWRELROOT>/binp/dll/wccdppc.dll
    <CPCMD> ppc/os2386/wccdppcc.sym   <OWRELROOT>/binp/dll/wccdppc.sym
    <CPCMD> ppc/os2386/wccppc01.int   <OWRELROOT>/binp/wccppc01.int
    <CPCMD> mps/os2386/wccmpsc.exe    <OWRELROOT>/binp/wccmps.exe
    <CPCMD> mps/os2386/wccmpsc.sym    <OWRELROOT>/binp/wccmps.sym
    <CPCMD> mps/os2386/wccdmpsc.dll   <OWRELROOT>/binp/dll/wccdmps.dll
    <CPCMD> mps/os2386/wccdmpsc.sym   <OWRELROOT>/binp/dll/wccdmps.sym
    <CPCMD> mps/os2386/wccmps01.int   <OWRELROOT>/binp/wccmps01.int

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> 386/nt386/wcc386c.exe     <OWRELROOT>/binnt/wcc386.exe
    <CPCMD> 386/nt386/wcc386c.sym     <OWRELROOT>/binnt/wcc386.sym
    <CPCMD> 386/nt386/wccd386c.dll    <OWRELROOT>/binnt/wccd386.dll
    <CPCMD> 386/nt386/wccd386c.sym    <OWRELROOT>/binnt/wccd386.sym
    <CPCMD> 386/nt386/wcc38601.int    <OWRELROOT>/binnt/wcc38601.int
    <CPCMD> 386/nt386.dll/wcc386c.exe  <OWRELROOT>/binnt/rtdll/wcc386.exe
    <CPCMD> 386/nt386.dll/wccd386c.dll <OWRELROOT>/binnt/rtdll/wccd386.dll
    <CPCMD> 386/nt386.dll/wccd386c.sym <OWRELROOT>/binnt/rtdll/wccd386.sym
    <CPCMD> 386/nt386.dll/wcc38601.int <OWRELROOT>/binnt/rtdll/wcc38601.int
    <CPCMD> i86/nt386/wcci86c.exe     <OWRELROOT>/binnt/wcc.exe
    <CPCMD> i86/nt386/wcci86c.sym     <OWRELROOT>/binnt/wcc.sym
    <CPCMD> i86/nt386/wccdi86c.dll    <OWRELROOT>/binnt/wccd.dll
    <CPCMD> i86/nt386/wccdi86c.sym    <OWRELROOT>/binnt/wccd.sym
    <CPCMD> i86/nt386/wcci8601.int    <OWRELROOT>/binnt/wcci8601.int
    <CPCMD> axp/nt386/wccaxpc.exe     <OWRELROOT>/binnt/wccaxp.exe
    <CPCMD> axp/nt386/wccaxpc.sym     <OWRELROOT>/binnt/wccaxp.sym
    <CPCMD> axp/nt386/wccdaxpc.dll    <OWRELROOT>/binnt/wccdaxp.dll
    <CPCMD> axp/nt386/wccdaxpc.sym    <OWRELROOT>/binnt/wccdaxp.sym
    <CPCMD> axp/nt386/wccaxp01.int    <OWRELROOT>/binnt/wccaxp01.int
    <CPCMD> ppc/nt386/wccppcc.exe     <OWRELROOT>/binnt/wccppc.exe
    <CPCMD> ppc/nt386/wccppcc.sym     <OWRELROOT>/binnt/wccppc.sym
    <CPCMD> ppc/nt386/wccdppcc.dll    <OWRELROOT>/binnt/wccdppc.dll
    <CPCMD> ppc/nt386/wccdppcc.sym    <OWRELROOT>/binnt/wccdppc.sym
    <CPCMD> ppc/nt386/wccppc01.int    <OWRELROOT>/binnt/wccppc01.int

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> 386/linux386/wcc386c.exe  <OWRELROOT>/binl/wcc386
    <CPCMD> 386/linux386/wcc386c.sym  <OWRELROOT>/binl/wcc386.sym
    <CPCMD> 386/linux386/wcc38601.int <OWRELROOT>/binl/wcc38601.int
    <CPCMD> i86/linux386/wcci86c.exe  <OWRELROOT>/binl/wcc
    <CPCMD> i86/linux386/wcci86c.sym  <OWRELROOT>/binl/wcc.sym
    <CPCMD> i86/linux386/wcci8601.int <OWRELROOT>/binl/wcci8601.int
    <CPCMD> axp/linux386/wccaxpc.exe  <OWRELROOT>/binl/wccaxp
    <CPCMD> axp/linux386/wccaxpc.sym  <OWRELROOT>/binl/wccaxp.sym
  [ ENDIF ]

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> axp/ntaxp/wccaxpc.exe     <OWRELROOT>/axpnt/wccaxp.exe
    <CPCMD> axp/ntaxp/wccaxpc.sym     <OWRELROOT>/axpnt/wccaxp.sym
    <CPCMD> axp/ntaxp/wccaxp01.int    <OWRELROOT>/axpnt/wccaxp01.int
    <CPCMD> 386/ntaxp/wcc386c.exe     <OWRELROOT>/axpnt/wcc386.exe
    <CPCMD> 386/ntaxp/wcc386c.sym     <OWRELROOT>/axpnt/wcc386.sym
    <CPCMD> 386/ntaxp/wcc38601.int    <OWRELROOT>/axpnt/wcc38601.int

  [ IFDEF (cpu_ppc) <2*> ]          # PPC target from 386 hosts
    <CPCMD> ppc/nt386/wccppcc.exe     <OWRELROOT>/binnt/wccppc.exe
    <CPCMD> ppc/nt386/wccppcc.sym     <OWRELROOT>/binnt/wccppc.sym
    <CPCMD> ppc/nt386/wccdppcc.dll    <OWRELROOT>/binnt/wccdppc.dll
    <CPCMD> ppc/nt386/wccdppcc.sym    <OWRELROOT>/binnt/wccdppc.sym
    <CPCMD> ppc/nt386/wccppc01.int    <OWRELROOT>/binnt/wccppc01.int
    <CPCMD> ppc/os2386/wccppcc.exe    <OWRELROOT>/binp/wccppc.exe
    <CPCMD> ppc/os2386/wccppcc.sym    <OWRELROOT>/binp/wccppc.sym
    <CPCMD> ppc/os2386/wccdppcc.dll   <OWRELROOT>/binp/dll/wccdppc.dll
    <CPCMD> ppc/os2386/wccdppcc.sym   <OWRELROOT>/binp/dll/wccdppc.sym
#    <CPCMD> ppc/ntppc/wccppcc.exe     <OWRELROOT>/ppcnt/wccppc.exe
#    <CPCMD> ppc/ntppc/wccppcc.sym     <OWRELROOT>/ppcnt/wccppc.sym
#    <CPCMD> ppc/ntppc/wccdppcc.dll    <OWRELROOT>/ppcnt/wccdppc.dll
#    <CPCMD> ppc/ntppc/wccdppcc.sym    <OWRELROOT>/ppcnt/wccdppc.sym
#    <CPCMD> ppc/ntppc/wccppc01.int    <OWRELROOT>/ppcnt/wccppc01.int
  [ ENDIF ]
  
  # MIPS target from 386 hosts
    <CCCMD> mps/nt386/wccmpsc.exe     <OWRELROOT>/binnt/wccmps.exe
    <CCCMD> mps/nt386/wccmpsc.sym     <OWRELROOT>/binnt/wccmps.sym
    <CCCMD> mps/nt386/wccdmpsc.dll    <OWRELROOT>/binnt/wccdmps.dll
    <CCCMD> mps/nt386/wccdmpsc.sym    <OWRELROOT>/binnt/wccdmps.sym
    <CCCMD> mps/nt386/wccmps01.int    <OWRELROOT>/binnt/wccmps01.int
    <CCCMD> mps/os2386/wccmpsc.exe    <OWRELROOT>/binp/wccmps.exe
    <CCCMD> mps/os2386/wccmpsc.sym    <OWRELROOT>/binp/wccmps.sym
    <CCCMD> mps/os2386/wccdmpsc.dll   <OWRELROOT>/binp/dll/wccdmps.dll
    <CCCMD> mps/os2386/wccdmpsc.sym   <OWRELROOT>/binp/dll/wccdmps.sym

  [ IFDEF (os_qnx) <2*> ]
    <CPCMD> i86/qnx386/wcci86c.exe    <OWRELROOT>/qnx/binq/wcc.
    <CPCMD> i86/qnx386/wcci86c.sym    <OWRELROOT>/qnx/sym/wcc.sym
    <CPCMD> 386/qnx386/wcc386c.exe    <OWRELROOT>/qnx/binq/wcc386.
    <CPCMD> 386/qnx386/wcc386c.sym    <OWRELROOT>/qnx/sym/wcc.sym

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
