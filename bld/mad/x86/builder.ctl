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
    <CCCMD> <PROJDIR>/dos386/madx86.mad     <OWRELROOT>/binw/
#    <CCCMD> <PROJDIR>/dos386/madx86.sym     <OWRELROOT>/binw/madx86.dsy

    <CCCMD> <PROJDIR>/wini86/madx86.dll     <OWRELROOT>/binw/
#    <CCCMD> <PROJDIR>/wini86/madx86.sym     <OWRELROOT>/binw/

#    <CCCMD> <PROJDIR>/os2i86/madx86.dll     <OWRELROOT>/binp/dll/
#    <CCCMD> <PROJDIR>/os2i86/madx86.sym     <OWRELROOT>/binp/dll/
    <CCCMD> <PROJDIR>/os2386/madx86.d32     <OWRELROOT>/binp/
    <CCCMD> <PROJDIR>/os2386/madx86.sym     <OWRELROOT>/binp/

    <CCCMD> <PROJDIR>/nt386/madx86.dll      <OWRELROOT>/binnt/
    <CCCMD> <PROJDIR>/nt386/madx86.sym      <OWRELROOT>/binnt/

    <CCCMD> <PROJDIR>/linux386/madx86.mad   <OWRELROOT>/binl/
    <CCCMD> <PROJDIR>/linux386/madx86.sym   <OWRELROOT>/binl/

    <CCCMD> <PROJDIR>/ntaxp/madx86.dll      <OWRELROOT>/axpnt/
    <CCCMD> <PROJDIR>/ntaxp/madx86.sym      <OWRELROOT>/axpnt/

    <CCCMD> <PROJDIR>/qnx386/madx86.mad     <OWRELROOT>/qnx/watcom/wd/
    <CCCMD> <PROJDIR>/qnx386/madx86.sym     <OWRELROOT>/qnx/sym/

    <CCCMD> <PROJDIR>/rdos386/madx86.dll    <OWRELROOT>/rdos/
    <CCCMD> <PROJDIR>/rdos386/madx86.sym    <OWRELROOT>/rdos/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
