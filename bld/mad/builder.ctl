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
    <CPCMD> x86/dos386/madx86.mad   <OWRELROOT>/binw/
#    <CPCMD> axp/dos386/madaxp.mad   <OWRELROOT>/binw/
#    <CPCMD> ppc/dos386/madppc.mad   <OWRELROOT>/binw/

  [ IFDEF (os_win "") <2*> ]
    <CPCMD> x86/wini86/madx86.dll   <OWRELROOT>/binw/
    <CPCMD> axp/wini86/madaxp.dll   <OWRELROOT>/binw/
    <CPCMD> ppc/wini86/madppc.dll   <OWRELROOT>/binw/
    <CPCMD> mips/wini86/madmips.dll <OWRELROOT>/binw/

  [ IFDEF (os_os2 "") <2*> ]
#    <CPCMD> x86/os2i86/madx86.dll   <OWRELROOT>/binp/dll/
#    <CPCMD> axp/os2i86/madaxp.dll   <OWRELROOT>/binp/dll/
#    <CPCMD> ppc/os2i86/madppc.dll   <OWRELROOT>/binp/dll/
    <CPCMD> x86/os2386/madx86.d32   <OWRELROOT>/binp/
    <CPCMD> x86/os2386/madx86.sym   <OWRELROOT>/binp/
    <CPCMD> axp/os2386/madaxp.d32   <OWRELROOT>/binp/
    <CPCMD> axp/os2386/madaxp.sym   <OWRELROOT>/binp/
    <CPCMD> ppc/os2386/madppc.d32   <OWRELROOT>/binp/
    <CPCMD> ppc/os2386/madppc.sym   <OWRELROOT>/binp/
    <CPCMD> mips/os2386/madmips.d32 <OWRELROOT>/binp/
    <CPCMD> mips/os2386/madmips.sym <OWRELROOT>/binp/

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> x86/nt386/madx86.dll    <OWRELROOT>/binnt/
    <CPCMD> x86/nt386/madx86.sym    <OWRELROOT>/binnt/
    <CPCMD> axp/nt386/madaxp.dll    <OWRELROOT>/binnt/
    <CPCMD> axp/nt386/madaxp.sym    <OWRELROOT>/binnt/
    <CPCMD> ppc/nt386/madppc.dll    <OWRELROOT>/binnt/
    <CPCMD> ppc/nt386/madppc.sym    <OWRELROOT>/binnt/
    <CPCMD> mips/nt386/madmips.dll  <OWRELROOT>/binnt/
    <CPCMD> mips/nt386/madmips.sym  <OWRELROOT>/binnt/

  [ IFDEF (cpu_axp "") <2*> ]
    <CCCMD> x86/ntaxp/madx86.dll    <OWRELROOT>/axpnt/
    <CCCMD> axp/ntaxp/madaxp.dll    <OWRELROOT>/axpnt/
    <CCCMD> ppc/ntaxp/madppc.dll    <OWRELROOT>/axpnt/
    <CCCMD> mips/ntaxp/madmips.dll  <OWRELROOT>/axpnt/

  [ IFDEF (os_qnx) <2*> ]
    <CPCMD> x86/qnx386/madx86.mad   <OWRELROOT>/qnx/watcom/wd/
#    <CPCMD> axp/qnx386/madaxp.mad   <OWRELROOT>/qnx/watcom/wd/
#    <CPCMD> ppc/qnx386/madppc.mad   <OWRELROOT>/qnx/watcom/wd/

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> x86/linux386/madx86.mad    <OWRELROOT>/binl/
    <CPCMD> x86/linux386/madx86.sym    <OWRELROOT>/binl/
    <CPCMD> axp/linux386/madaxp.mad    <OWRELROOT>/binl/
    <CPCMD> axp/linux386/madaxp.sym    <OWRELROOT>/binl/
    <CPCMD> ppc/linux386/madppc.mad    <OWRELROOT>/binl/
    <CPCMD> ppc/linux386/madppc.sym    <OWRELROOT>/binl/
    <CPCMD> mips/linux386/madmips.mad  <OWRELROOT>/binl/
    <CPCMD> mips/linux386/madmips.sym  <OWRELROOT>/binl/

  [ IFDEF (os_rdos "") <2*> ]
    <CPCMD> x86/rdos386/madx86.dll    <OWRELROOT>/rdos/
    <CPCMD> x86/rdos386/madx86.sym    <OWRELROOT>/rdos/


[ BLOCK <1> clean ]
#==================
     pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
