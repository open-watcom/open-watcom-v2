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
    <CCCMD> <PROJDIR>/dos386/madaxp.mad     <OWRELROOT>/binw/
#    <CCCMD> <PROJDIR>/dos386/madaxp.sym     <OWRELROOT>/binw/madaxp.dsy

    <CCCMD> <PROJDIR>/wini86/madaxp.dll     <OWRELROOT>/binw/
#    <CCCMD> <PROJDIR>/wini86/madaxp.sym     <OWRELROOT>/binw/

#    <CCCMD> <PROJDIR>/os2i86/madaxp.dll     <OWRELROOT>/binp/dll/
#    <CCCMD> <PROJDIR>/os2i86/madaxp.sym     <OWRELROOT>/binp/dll/
    <CCCMD> <PROJDIR>/os2386/madaxp.d32     <OWRELROOT>/binp/
    <CCCMD> <PROJDIR>/os2386/madaxp.sym     <OWRELROOT>/binp/

    <CCCMD> <PROJDIR>/nt386/madaxp.dll      <OWRELROOT>/binnt/
    <CCCMD> <PROJDIR>/nt386/madaxp.sym      <OWRELROOT>/binnt/

    <CCCMD> <PROJDIR>/linux386/madaxp.mad   <OWRELROOT>/binl/
    <CCCMD> <PROJDIR>/linux386/madaxp.sym   <OWRELROOT>/binl/

    <CCCMD> <PROJDIR>/ntaxp/madaxp.dll      <OWRELROOT>/axpnt/
    <CCCMD> <PROJDIR>/ntaxp/madaxp.sym      <OWRELROOT>/axpnt/

    <CCCMD> <PROJDIR>/qnx386/madaxp.mad     <OWRELROOT>/qnx/watcom/wd/
    <CCCMD> <PROJDIR>/qnx386/madaxp.sym     <OWRELROOT>/qnx/sym/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
