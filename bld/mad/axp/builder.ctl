# Alpha AXP MAD Builder Control file
# ==================================

set PROJDIR=<CWD>
set PROJNAME=madaxp

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
    <CCCMD> dos386/madaxp.mad     <OWRELROOT>/binw/
#    <CCCMD> dos386/madaxp.sym     <OWRELROOT>/binw/madaxp.dsy

    <CCCMD> wini86/madaxp.dll     <OWRELROOT>/binw/
#    <CCCMD> wini86/madaxp.sym     <OWRELROOT>/binw/

#    <CCCMD> os2i86/madaxp.dll     <OWRELROOT>/binp/dll/
#    <CCCMD> os2i86/madaxp.sym     <OWRELROOT>/binp/dll/
    <CCCMD> os2386/madaxp.d32     <OWRELROOT>/binp/
    <CCCMD> os2386/madaxp.sym     <OWRELROOT>/binp/

    <CCCMD> nt386/madaxp.dll      <OWRELROOT>/binnt/
    <CCCMD> nt386/madaxp.sym      <OWRELROOT>/binnt/

    <CCCMD> linux386/madaxp.mad   <OWRELROOT>/binl/
    <CCCMD> linux386/madaxp.sym   <OWRELROOT>/binl/

    <CCCMD> ntaxp/madaxp.dll      <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/madaxp.sym      <OWRELROOT>/axpnt/

    <CCCMD> qnx386/madaxp.mad     <OWRELROOT>/qnx/watcom/wd/
    <CCCMD> qnx386/madaxp.sym     <OWRELROOT>/qnx/sym/

    <CCCMD> ntx64/madaxp.dll      <OWRELROOT>/binnt64/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
