# PowerPC MAD Builder Control file
# ================================

set PROJDIR=<CWD>
set PROJNAME=madppc

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
    <CCCMD> dos386/madppc.mad     <OWRELROOT>/binw/
#    <CCCMD> dos386/madppc.sym     <OWRELROOT>/binw/madppc.dsy

    <CCCMD> wini86/madppc.dll     <OWRELROOT>/binw/
#    <CCCMD> wini86/madppc.sym     <OWRELROOT>/binw/

#    <CCCMD> os2i86/madppc.dll     <OWRELROOT>/binp/dll/
#    <CCCMD> os2i86/madppc.sym     <OWRELROOT>/binp/dll/
    <CCCMD> os2386/madppc.d32     <OWRELROOT>/binp/
    <CCCMD> os2386/madppc.sym     <OWRELROOT>/binp/

    <CCCMD> nt386/madppc.dll      <OWRELROOT>/binnt/
    <CCCMD> nt386/madppc.sym      <OWRELROOT>/binnt/

    <CCCMD> linux386/madppc.mad   <OWRELROOT>/binl/
    <CCCMD> linux386/madppc.sym   <OWRELROOT>/binl/

    <CCCMD> ntaxp/madppc.dll      <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/madppc.sym      <OWRELROOT>/axpnt/

    <CCCMD> qnx386/madppc.mad     <OWRELROOT>/qnx/watcom/wd/
    <CCCMD> qnx386/madppc.sym     <OWRELROOT>/qnx/sym/

    <CCCMD> ntx64/madppc.dll      <OWRELROOT>/binnt64/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
