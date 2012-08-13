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
    <CPCMD> 386/dos386/wcc386c.exe    <RELROOT>/binw/wcc386.exe
    <CPCMD> 386/dos386/wcc386c.sym    <RELROOT>/binw/wcc386.sym
    <CPCMD> 386/dos386/wcc38601.int   <RELROOT>/binw/wcc38601.int
    <CPCMD> i86/dos386/wcci86c.exe    <RELROOT>/binw/wcc.exe
    <CPCMD> i86/dos386/wcci86c.sym    <RELROOT>/binw/wcc.sym
    <CPCMD> i86/dos386/wcci8601.int   <RELROOT>/binw/wcci8601.int
#    <CPCMD> axp/dos386/wccaxpc.exe    <RELROOT>/binnt/wccaxp.exe

#  [ IFDEF (os_osi "") <2*> ]    #osi stuff
#    <CPCMD> 386/osi386/wcc386c.exe    <RELROOT>/binw/wcc386.exe
#    <CPCMD> 386/osi386/wcc386c.sym    <RELROOT>/binw/wcc386.sym
#    <CPCMD> 386/osi386/wcc38601.int   <RELROOT>/binw/wcc38601.int
#    <CPCMD> i86/osi386/wcci86c.exe    <RELROOT>/binw/wcc.exe
#    <CPCMD> i86/osi386/wcci86c.sym    <RELROOT>/binw/wcc.sym
#    <CPCMD> i86/osi386/wcci8601.int   <RELROOT>/binw/wcci8601.int
#    <CPCMD> axp/osi386/wccaxpc.exe    <RELROOT>/binw/wccaxp.exe
#    <CPCMD> ppc/osi386/wccppcc.exe    <RELROOT>/binw/wccppc.exe

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> 386/os2386/wcc386c.exe    <RELROOT>/binp/wcc386.exe
    <CPCMD> 386/os2386/wcc386c.sym    <RELROOT>/binp/wcc386.sym
    <CPCMD> 386/os2386/wccd386c.dll   <RELROOT>/binp/dll/wccd386.dll
    <CPCMD> 386/os2386/wccd386c.sym   <RELROOT>/binp/dll/wccd386.sym
    <CPCMD> 386/os2386/wcc38601.int   <RELROOT>/binp/dll/wcc38601.int
    <CPCMD> i86/os2386/wcci86c.exe    <RELROOT>/binp/wcc.exe
    <CPCMD> i86/os2386/wcci86c.sym    <RELROOT>/binp/wcc.sym
    <CPCMD> i86/os2386/wccdi86c.dll   <RELROOT>/binp/dll/wccd.dll
    <CPCMD> i86/os2386/wccdi86c.sym   <RELROOT>/binp/dll/wccd.sym
    <CPCMD> i86/os2386/wcci8601.int   <RELROOT>/binp/dll/wcci8601.int
    <CPCMD> axp/os2386/wccaxpc.exe    <RELROOT>/binp/wccaxp.exe
    <CPCMD> axp/os2386/wccaxpc.sym    <RELROOT>/binp/wccaxp.sym
    <CPCMD> axp/os2386/wccdaxpc.dll   <RELROOT>/binp/dll/wccdaxp.dll
    <CPCMD> axp/os2386/wccdaxpc.sym   <RELROOT>/binp/dll/wccdaxp.sym
    <CPCMD> axp/os2386/wccaxp01.int   <RELROOT>/binp/wccaxp01.int
    <CPCMD> ppc/os2386/wccppcc.exe    <RELROOT>/binp/wccppc.exe
    <CPCMD> ppc/os2386/wccppcc.sym    <RELROOT>/binp/wccppc.sym
    <CPCMD> ppc/os2386/wccdppcc.dll   <RELROOT>/binp/dll/wccdppc.dll
    <CPCMD> ppc/os2386/wccdppcc.sym   <RELROOT>/binp/dll/wccdppc.sym
    <CPCMD> ppc/os2386/wccppc01.int   <RELROOT>/binp/wccppc01.int
    <CPCMD> mps/os2386/wccmpsc.exe    <RELROOT>/binp/wccmps.exe
    <CPCMD> mps/os2386/wccmpsc.sym    <RELROOT>/binp/wccmps.sym
    <CPCMD> mps/os2386/wccdmpsc.dll   <RELROOT>/binp/dll/wccdmps.dll
    <CPCMD> mps/os2386/wccdmpsc.sym   <RELROOT>/binp/dll/wccdmps.sym
    <CPCMD> mps/os2386/wccmps01.int   <RELROOT>/binp/wccmps01.int

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> 386/nt386/wcc386c.exe     <RELROOT>/binnt/wcc386.exe
    <CPCMD> 386/nt386/wcc386c.sym     <RELROOT>/binnt/wcc386.sym
    <CPCMD> 386/nt386/wccd386c.dll    <RELROOT>/binnt/wccd386.dll
    <CPCMD> 386/nt386/wccd386c.sym    <RELROOT>/binnt/wccd386.sym
    <CPCMD> 386/nt386/wcc38601.int    <RELROOT>/binnt/wcc38601.int
    <CPCMD> 386/nt386.dll/wcc386c.exe  <RELROOT>/binnt/rtdll/wcc386.exe
    <CPCMD> 386/nt386.dll/wccd386c.dll <RELROOT>/binnt/rtdll/wccd386.dll
    <CPCMD> 386/nt386.dll/wccd386c.sym <RELROOT>/binnt/rtdll/wccd386.sym
    <CPCMD> 386/nt386.dll/wcc38601.int <RELROOT>/binnt/rtdll/wcc38601.int
    <CPCMD> i86/nt386/wcci86c.exe     <RELROOT>/binnt/wcc.exe
    <CPCMD> i86/nt386/wcci86c.sym     <RELROOT>/binnt/wcc.sym
    <CPCMD> i86/nt386/wccdi86c.dll    <RELROOT>/binnt/wccd.dll
    <CPCMD> i86/nt386/wccdi86c.sym    <RELROOT>/binnt/wccd.sym
    <CPCMD> i86/nt386/wcci8601.int    <RELROOT>/binnt/wcci8601.int
    <CPCMD> axp/nt386/wccaxpc.exe     <RELROOT>/binnt/wccaxp.exe
    <CPCMD> axp/nt386/wccaxpc.sym     <RELROOT>/binnt/wccaxp.sym
    <CPCMD> axp/nt386/wccdaxpc.dll    <RELROOT>/binnt/wccdaxp.dll
    <CPCMD> axp/nt386/wccdaxpc.sym    <RELROOT>/binnt/wccdaxp.sym
    <CPCMD> axp/nt386/wccaxp01.int    <RELROOT>/binnt/wccaxp01.int
    <CPCMD> ppc/nt386/wccppcc.exe     <RELROOT>/binnt/wccppc.exe
    <CPCMD> ppc/nt386/wccppcc.sym     <RELROOT>/binnt/wccppc.sym
    <CPCMD> ppc/nt386/wccdppcc.dll    <RELROOT>/binnt/wccdppc.dll
    <CPCMD> ppc/nt386/wccdppcc.sym    <RELROOT>/binnt/wccdppc.sym
    <CPCMD> ppc/nt386/wccppc01.int    <RELROOT>/binnt/wccppc01.int

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> 386/linux386/wcc386c.exe  <RELROOT>/binl/wcc386
    <CPCMD> 386/linux386/wcc386c.sym  <RELROOT>/binl/wcc386.sym
    <CPCMD> 386/linux386/wcc38601.int <RELROOT>/binl/wcc38601.int
    <CPCMD> i86/linux386/wcci86c.exe  <RELROOT>/binl/wcc
    <CPCMD> i86/linux386/wcci86c.sym  <RELROOT>/binl/wcc.sym
    <CPCMD> i86/linux386/wcci8601.int <RELROOT>/binl/wcci8601.int
    <CPCMD> axp/linux386/wccaxpc.exe  <RELROOT>/binl/wccaxp
    <CPCMD> axp/linux386/wccaxpc.sym  <RELROOT>/binl/wccaxp.sym
  [ ENDIF ]

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> axp/ntaxp/wccaxpc.exe     <RELROOT>/axpnt/wccaxp.exe
    <CPCMD> axp/ntaxp/wccaxpc.sym     <RELROOT>/axpnt/wccaxp.sym
    <CPCMD> axp/ntaxp/wccaxp01.int    <RELROOT>/axpnt/wccaxp01.int
    <CPCMD> 386/ntaxp/wcc386c.exe     <RELROOT>/axpnt/wcc386.exe
    <CPCMD> 386/ntaxp/wcc386c.sym     <RELROOT>/axpnt/wcc386.sym
    <CPCMD> 386/ntaxp/wcc38601.int    <RELROOT>/axpnt/wcc38601.int

  [ IFDEF (cpu_ppc) <2*> ]          # PPC target from 386 hosts
    <CPCMD> ppc/nt386/wccppcc.exe     <RELROOT>/binnt/wccppc.exe
    <CPCMD> ppc/nt386/wccppcc.sym     <RELROOT>/binnt/wccppc.sym
    <CPCMD> ppc/nt386/wccdppcc.dll    <RELROOT>/binnt/wccdppc.dll
    <CPCMD> ppc/nt386/wccdppcc.sym    <RELROOT>/binnt/wccdppc.sym
    <CPCMD> ppc/nt386/wccppc01.int    <RELROOT>/binnt/wccppc01.int
    <CPCMD> ppc/os2386/wccppcc.exe    <RELROOT>/binp/wccppc.exe
    <CPCMD> ppc/os2386/wccppcc.sym    <RELROOT>/binp/wccppc.sym
    <CPCMD> ppc/os2386/wccdppcc.dll   <RELROOT>/binp/dll/wccdppc.dll
    <CPCMD> ppc/os2386/wccdppcc.sym   <RELROOT>/binp/dll/wccdppc.sym
#    <CPCMD> ppc/ntppc/wccppcc.exe     <RELROOT>/ppcnt/wccppc.exe
#    <CPCMD> ppc/ntppc/wccppcc.sym     <RELROOT>/ppcnt/wccppc.sym
#    <CPCMD> ppc/ntppc/wccdppcc.dll    <RELROOT>/ppcnt/wccdppc.dll
#    <CPCMD> ppc/ntppc/wccdppcc.sym    <RELROOT>/ppcnt/wccdppc.sym
#    <CPCMD> ppc/ntppc/wccppc01.int    <RELROOT>/ppcnt/wccppc01.int
  [ ENDIF ]
  
  # MIPS target from 386 hosts
    <CCCMD> mps/nt386/wccmpsc.exe     <RELROOT>/binnt/wccmps.exe
    <CCCMD> mps/nt386/wccmpsc.sym     <RELROOT>/binnt/wccmps.sym
    <CCCMD> mps/nt386/wccdmpsc.dll    <RELROOT>/binnt/wccdmps.dll
    <CCCMD> mps/nt386/wccdmpsc.sym    <RELROOT>/binnt/wccdmps.sym
    <CCCMD> mps/nt386/wccmps01.int    <RELROOT>/binnt/wccmps01.int
    <CCCMD> mps/os2386/wccmpsc.exe    <RELROOT>/binp/wccmps.exe
    <CCCMD> mps/os2386/wccmpsc.sym    <RELROOT>/binp/wccmps.sym
    <CCCMD> mps/os2386/wccdmpsc.dll   <RELROOT>/binp/dll/wccdmps.dll
    <CCCMD> mps/os2386/wccdmpsc.sym   <RELROOT>/binp/dll/wccdmps.sym

  [ IFDEF (os_qnx) <2*> ]
    <CPCMD> i86/qnx386/wcci86c.exe    <RELROOT>/qnx/binq/wcc.
    <CPCMD> i86/qnx386/wcci86c.sym    <RELROOT>/qnx/sym/wcc.sym
    <CPCMD> 386/qnx386/wcc386c.exe    <RELROOT>/qnx/binq/wcc386.
    <CPCMD> 386/qnx386/wcc386c.sym    <RELROOT>/qnx/sym/wcc.sym

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
