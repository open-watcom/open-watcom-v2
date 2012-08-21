# MAD Builder Control file
# ========================

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
# x86
  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> x86/dos386/madx86.mad   <RELROOT>/binw/
#    <CPCMD> axp/dos386/madaxp.mad   <RELROOT>/binw/
#    <CPCMD> ppc/dos386/madppc.mad   <RELROOT>/binw/

  [ IFDEF (os_win "") <2*> ]
    <CPCMD> x86/wini86/madx86.dll   <RELROOT>/binw/
    <CPCMD> axp/wini86/madaxp.dll   <RELROOT>/binw/
    <CPCMD> ppc/wini86/madppc.dll   <RELROOT>/binw/
    <CPCMD> mips/wini86/madmips.dll <RELROOT>/binw/

  [ IFDEF (os_os2 "") <2*> ]
#    <CPCMD> x86/os2i86/madx86.dll   <RELROOT>/binp/dll/
#    <CPCMD> axp/os2i86/madaxp.dll   <RELROOT>/binp/dll/
#    <CPCMD> ppc/os2i86/madppc.dll   <RELROOT>/binp/dll/
    <CPCMD> x86/os2386/madx86.d32   <RELROOT>/binp/
    <CPCMD> x86/os2386/madx86.sym   <RELROOT>/binp/
    <CPCMD> axp/os2386/madaxp.d32   <RELROOT>/binp/
    <CPCMD> axp/os2386/madaxp.sym   <RELROOT>/binp/
    <CPCMD> ppc/os2386/madppc.d32   <RELROOT>/binp/
    <CPCMD> ppc/os2386/madppc.sym   <RELROOT>/binp/
    <CPCMD> mips/os2386/madmips.d32 <RELROOT>/binp/
    <CPCMD> mips/os2386/madmips.sym <RELROOT>/binp/

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> x86/nt386/madx86.dll    <RELROOT>/binnt/
    <CPCMD> x86/nt386/madx86.sym    <RELROOT>/binnt/
    <CPCMD> axp/nt386/madaxp.dll    <RELROOT>/binnt/
    <CPCMD> axp/nt386/madaxp.sym    <RELROOT>/binnt/
    <CPCMD> ppc/nt386/madppc.dll    <RELROOT>/binnt/
    <CPCMD> ppc/nt386/madppc.sym    <RELROOT>/binnt/
    <CPCMD> mips/nt386/madmips.dll  <RELROOT>/binnt/
    <CPCMD> mips/nt386/madmips.sym  <RELROOT>/binnt/

  [ IFDEF (cpu_axp "") <2*> ]
    <CCCMD> x86/ntaxp/madx86.dll    <RELROOT>/axpnt/
    <CCCMD> axp/ntaxp/madaxp.dll    <RELROOT>/axpnt/
    <CCCMD> ppc/ntaxp/madppc.dll    <RELROOT>/axpnt/
    <CCCMD> mips/ntaxp/madmips.dll  <RELROOT>/axpnt/

  [ IFDEF (os_qnx) <2*> ]
    <CPCMD> x86/qnx386/madx86.mad   <RELROOT>/qnx/watcom/wd/
#    <CPCMD> axp/qnx386/madaxp.mad   <RELROOT>/qnx/watcom/wd/
#    <CPCMD> ppc/qnx386/madppc.mad   <RELROOT>/qnx/watcom/wd/

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> x86/linux386/madx86.mad    <RELROOT>/binl/
    <CPCMD> x86/linux386/madx86.sym    <RELROOT>/binl/
    <CPCMD> axp/linux386/madaxp.mad    <RELROOT>/binl/
    <CPCMD> axp/linux386/madaxp.sym    <RELROOT>/binl/
    <CPCMD> ppc/linux386/madppc.mad    <RELROOT>/binl/
    <CPCMD> ppc/linux386/madppc.sym    <RELROOT>/binl/
    <CPCMD> mips/linux386/madmips.mad  <RELROOT>/binl/
    <CPCMD> mips/linux386/madmips.sym  <RELROOT>/binl/

  [ IFDEF (os_rdos "") <2*> ]
    <CPCMD> x86/rdos386/madx86.dll    <RELROOT>/rdos/
    <CPCMD> x86/rdos386/madx86.sym    <RELROOT>/rdos/


[ BLOCK <1> clean ]
#==================
     pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
