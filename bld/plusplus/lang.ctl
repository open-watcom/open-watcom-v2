# PLUS Builder Control file
# =========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/bat/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
#=======================
    pmake -d buildwpp <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> rel2 cprel2 acprel2 ]
#================================

#
# 386 NT hosted compilers
#
  [ IFDEF (os_nt "") <2*> ]
#  i86 target
    <CPCMD> <PROJDIR>/nt386.i86/wcppi86.exe   <RELROOT>/binnt/wpp.exe
    <CPCMD> <PROJDIR>/nt386.i86/wcppi86.sym   <RELROOT>/binnt/wpp.sym
    <CPCMD> <PROJDIR>/nt386.i86/wppdi86.dll   <RELROOT>/binnt/wppdi86.dll
    <CPCMD> <PROJDIR>/nt386.i86/wppdi86.sym   <RELROOT>/binnt/wppdi86.sym
    <CPCMD> <PROJDIR>/nt386.i86/wppi8601.int  <RELROOT>/binnt/wppi8601.int
#  386 target
    <CPCMD> <PROJDIR>/nt386.386/wcpp386.exe   <RELROOT>/binnt/wpp386.exe
    <CPCMD> <PROJDIR>/nt386.386/wcpp386.sym   <RELROOT>/binnt/wpp386.sym
    <CPCMD> <PROJDIR>/nt386.386/wppd386.dll   <RELROOT>/binnt/wppd386.dll
    <CPCMD> <PROJDIR>/nt386.386/wppd386.sym   <RELROOT>/binnt/wppd386.sym
    <CPCMD> <PROJDIR>/nt386.386/wpp38601.int  <RELROOT>/binnt/wpp38601.int
#  AXP target
    <CPCMD> <PROJDIR>/nt386.axp/wcppaxp.exe   <RELROOT>/binnt/wppaxp.exe
    <CPCMD> <PROJDIR>/nt386.axp/wcppaxp.sym   <RELROOT>/binnt/wppaxp.sym
    <CPCMD> <PROJDIR>/nt386.axp/wppdaxp.dll   <RELROOT>/binnt/wppdaxp.dll
    <CPCMD> <PROJDIR>/nt386.axp/wppdaxp.sym   <RELROOT>/binnt/wppdaxp.sym
    <CPCMD> <PROJDIR>/nt386.axp/wppaxp01.int  <RELROOT>/binnt/wppaxp01.int

#  Optima 386 target (with -br switch)
    <CPCMD> <PROJDIR>/nt386dll.386/wcpp386.exe   <RELROOT>/binnt/rtdll/wpp386.exe
    <CPCMD> <PROJDIR>/nt386dll.386/wcpp386.sym   <RELROOT>/binnt/rtdll/wpp386.sym
    <CPCMD> <PROJDIR>/nt386dll.386/wppd386.dll   <RELROOT>/binnt/rtdll/wppd386.dll
    <CPCMD> <PROJDIR>/nt386dll.386/wppd386.sym   <RELROOT>/binnt/rtdll/wppd386.sym
    <CPCMD> <PROJDIR>/nt386dll.386/wpp38601.int  <RELROOT>/binnt/rtdll/wpp38601.int

#
# 386 OS/2 hosted compilers
#
  [ IFDEF (os_os2 "") <2*> ]
#  i86 target
    <CPCMD> <PROJDIR>/os2386.i86/wcppi86.exe   <RELROOT>/binp/wpp.exe
    <CPCMD> <PROJDIR>/os2386.i86/wcppi86.sym   <RELROOT>/binp/wpp.sym
    <CPCMD> <PROJDIR>/os2386.i86/wppdi86.dll   <RELROOT>/binp/dll/wppdi86.dll
    <CPCMD> <PROJDIR>/os2386.i86/wppdi86.sym   <RELROOT>/binp/dll/wppdi86.sym
    <CPCMD> <PROJDIR>/os2386.i86/wppi8601.int  <RELROOT>/binp/dll/wppi8601.int
#  386 target
    <CPCMD> <PROJDIR>/os2386.386/wcpp386.exe   <RELROOT>/binp/wpp386.exe
    <CPCMD> <PROJDIR>/os2386.386/wcpp386.sym   <RELROOT>/binp/wpp386.sym
    <CPCMD> <PROJDIR>/os2386.386/wppd386.dll   <RELROOT>/binp/dll/wppd386.dll
    <CPCMD> <PROJDIR>/os2386.386/wppd386.sym   <RELROOT>/binp/dll/wppd386.sym
    <CPCMD> <PROJDIR>/os2386.386/wpp38601.int  <RELROOT>/binp/dll/wpp38601.int
#  386 target
    <CPCMD> <PROJDIR>/os2386.axp/wcppaxp.exe   <RELROOT>/binp/wppaxp.exe
    <CPCMD> <PROJDIR>/os2386.axp/wcppaxp.sym   <RELROOT>/binp/wppaxp.sym
    <CPCMD> <PROJDIR>/os2386.axp/wppdaxp.dll   <RELROOT>/binp/dll/wppdaxp.dll
    <CPCMD> <PROJDIR>/os2386.axp/wppdaxp.sym   <RELROOT>/binp/dll/wppdaxp.sym
    <CPCMD> <PROJDIR>/os2386.axp/wppaxp01.int  <RELROOT>/binp/dll/wppaxp01.int
  [ ENDIF ]
#
# 386 OSI hosted compilers
#
#  i86 target
#    <CPCMD> <PROJDIR>/osi386.i86/wcppi86.exe   <RELROOT>/binw/wpp.exe
#    <CPCMD> <PROJDIR>/osi386.i86/wcppi86.sym   <RELROOT>/binw/wpp.sym
#    <CPCMD> <PROJDIR>/osi386.i86/wppi8601.int  <RELROOT>/binw/wppi8601.int
#  386 target
#    <CPCMD> <PROJDIR>/osi386.386/wcpp386.exe   <RELROOT>/binw/wpp386.exe
#    <CPCMD> <PROJDIR>/osi386.386/wcpp386.sym   <RELROOT>/binw/wpp386.sym
#    <CPCMD> <PROJDIR>/osi386.386/wpp38601.int  <RELROOT>/binw/wpp38601.int

#
# 386 DOS hosted compilers
#
  [ IFDEF (os_dos "") <2*> ]
#  i86 target
    <CPCMD> <PROJDIR>/dos386.i86/wcppi86.exe   <RELROOT>/binw/wpp.exe
    <CPCMD> <PROJDIR>/dos386.i86/wcppi86.sym   <RELROOT>/binw/wpp.sym
    <CPCMD> <PROJDIR>/dos386.i86/wppi8601.int  <RELROOT>/binw/wppi8601.int
#  386 target
    <CPCMD> <PROJDIR>/dos386.386/wcpp386.exe   <RELROOT>/binw/wpp386.exe
    <CPCMD> <PROJDIR>/dos386.386/wcpp386.sym   <RELROOT>/binw/wpp386.sym
    <CPCMD> <PROJDIR>/dos386.386/wpp38601.int  <RELROOT>/binw/wpp38601.int
#
# 386 QNX hosted compilers
#
  [ IFDEF (os_qnx) <2*> ]
#  i86 target
    <CPCMD> <PROJDIR>/qnx386.i86/wcppi86.exe   <RELROOT>/qnx/binq/wpp.
    <CPCMD> <PROJDIR>/qnx386.i86/wcppi86.sym   <RELROOT>/qnx/sym/wpp.sym
    <CPCMD> <PROJDIR>/qnx386.i86/wppi8601.int  <RELROOT>/qnx/sym/wppi8601.int
#  386 target
    <CPCMD> <PROJDIR>/qnx386.386/wcpp386.exe   <RELROOT>/qnx/binq/wpp386.
    <CPCMD> <PROJDIR>/qnx386.386/wcpp386.sym   <RELROOT>/qnx/sym/wpp386.sym
    <CPCMD> <PROJDIR>/qnx386.386/wpp38601.int  <RELROOT>/qnx/sym/wpp38601.int

#
# 386 Linux hosted compilers
#
  [ IFDEF (os_linux "") <2*> ]
#  i86 target
    <CPCMD> <PROJDIR>/linux386.i86/wcppi86.exe   <RELROOT>/binl/wpp
    <CPCMD> <PROJDIR>/linux386.i86/wcppi86.sym   <RELROOT>/binl/wpp.sym
    <CPCMD> <PROJDIR>/linux386.i86/wppi8601.int  <RELROOT>/binl/wppi8601.int
#  386 target
    <CPCMD> <PROJDIR>/linux386.386/wcpp386.exe   <RELROOT>/binl/wpp386
    <CPCMD> <PROJDIR>/linux386.386/wcpp386.sym   <RELROOT>/binl/wpp386.sym
    <CPCMD> <PROJDIR>/linux386.386/wpp38601.int  <RELROOT>/binl/wpp38601.int

#
# AXP NT hosted compilers
#
  [ IFDEF (cpu_axp) <2*> ]
#  AXP target
    <CPCMD> <PROJDIR>/ntaxp.axp/wcppaxp.exe   <RELROOT>/axpnt/wppaxp.exe
    <CPCMD> <PROJDIR>/ntaxp.axp/wcppaxp.sym   <RELROOT>/axpnt/wppaxp.sym
#  386 target
    <CPCMD> <PROJDIR>/ntaxp.386/wcpp386.exe   <RELROOT>/axpnt/wpp386.exe
    <CPCMD> <PROJDIR>/ntaxp.386/wcpp386.sym   <RELROOT>/axpnt/wpp386.sym

[ BLOCK <1> clean ]
#==================
    pmake -d buildwpp <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
