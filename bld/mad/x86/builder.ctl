# x86 MAD Builder Control file
# ============================

set PROJDIR=<CWD>
set PROJNAME=madx86

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ BLOCK .<WATCOMBOOT>. .1. ]
    [ INCLUDE <OWROOT>/build/deflib.ctl ]

[ BLOCK .<WATCOMBOOT>. .. ]
    [ INCLUDE <OWROOT>/build/defdylib.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/madx86.mad     <OWRELROOT>/binw/
#    <CCCMD> dos386/madx86.sym     <OWRELROOT>/binw/madx86.dsy

    <CCCMD> wini86/madx86.dll     <OWRELROOT>/binw/
#    <CCCMD> wini86/madx86.sym     <OWRELROOT>/binw/

#    <CCCMD> os2i86/madx86.dll     <OWRELROOT>/binp/dll/
#    <CCCMD> os2i86/madx86.sym     <OWRELROOT>/binp/dll/
    <CCCMD> os2386/madx86.d32     <OWRELROOT>/binp/
    <CCCMD> os2386/madx86.sym     <OWRELROOT>/binp/

    <CCCMD> nt386/madx86.dll      <OWRELROOT>/binnt/
    <CCCMD> nt386/madx86.sym      <OWRELROOT>/binnt/

    <CCCMD> linux386/madx86.mad   <OWRELROOT>/binl/
    <CCCMD> linux386/madx86.sym   <OWRELROOT>/binl/

    <CCCMD> ntaxp/madx86.dll      <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/madx86.sym      <OWRELROOT>/axpnt/

    <CCCMD> qnx386/madx86.mad     <OWRELROOT>/qnx/watcom/wd/
    <CCCMD> qnx386/madx86.sym     <OWRELROOT>/qnx/sym/

    <CCCMD> rdos386/madx86.dll    <OWRELROOT>/rdos/
    <CCCMD> rdos386/madx86.sym    <OWRELROOT>/rdos/

    <CCCMD> ntx64/madx86.dll      <OWRELROOT>/binnt64/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
