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
    <CCCMD> <PROJDIR>/dos386/madppc.mad     <OWRELROOT>/binw/
#    <CCCMD> <PROJDIR>/dos386/madppc.sym     <OWRELROOT>/binw/madppc.dsy

    <CCCMD> <PROJDIR>/wini86/madppc.dll     <OWRELROOT>/binw/
#    <CCCMD> <PROJDIR>/wini86/madppc.sym     <OWRELROOT>/binw/

#    <CCCMD> <PROJDIR>/os2i86/madppc.dll     <OWRELROOT>/binp/dll/
#    <CCCMD> <PROJDIR>/os2i86/madppc.sym     <OWRELROOT>/binp/dll/
    <CCCMD> <PROJDIR>/os2386/madppc.d32     <OWRELROOT>/binp/
    <CCCMD> <PROJDIR>/os2386/madppc.sym     <OWRELROOT>/binp/

    <CCCMD> <PROJDIR>/nt386/madppc.dll      <OWRELROOT>/binnt/
    <CCCMD> <PROJDIR>/nt386/madppc.sym      <OWRELROOT>/binnt/

    <CCCMD> <PROJDIR>/linux386/madppc.mad   <OWRELROOT>/binl/
    <CCCMD> <PROJDIR>/linux386/madppc.sym   <OWRELROOT>/binl/

    <CCCMD> <PROJDIR>/ntaxp/madppc.dll      <OWRELROOT>/axpnt/
    <CCCMD> <PROJDIR>/ntaxp/madppc.sym      <OWRELROOT>/axpnt/

    <CCCMD> <PROJDIR>/qnx386/madppc.mad     <OWRELROOT>/qnx/watcom/wd/
    <CCCMD> <PROJDIR>/qnx386/madppc.sym     <OWRELROOT>/qnx/sym/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
