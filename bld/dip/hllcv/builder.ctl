# IBM HLL/CodeView DIP Builder Control file
# =========================================

set PROJDIR=<CWD>
set PROJNAME=hllcv

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ BLOCK .<WATCOMBOOT>. .1. ]
    [ INCLUDE <OWROOT>/build/deflib.ctl ]

[ BLOCK .<WATCOMBOOT>. .. ]
    [ INCLUDE <OWROOT>/build/defdylib.ctl ]

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> <PROJDIR>/dos386/hllcv.dip      <OWRELROOT>/binw/
#    <CCCMD> <PROJDIR>/dos386/hllcv.sym      <OWRELROOT>/binw/hllcv.dsy

    <CCCMD> <PROJDIR>/wini86/hllcv.dll      <OWRELROOT>/binw/
#    <CCCMD> <PROJDIR>/wini86/hllcv.sym      <OWRELROOT>/binw/

#    <CCCMD> <PROJDIR>/os2i86/hllcv.dll      <OWRELROOT>/binp/dll/
#    <CCCMD> <PROJDIR>/os2i86/hllcv.sym      <OWRELROOT>/binp/dll/
    <CCCMD> <PROJDIR>/os2386/hllcv.d32      <OWRELROOT>/binp/
    <CCCMD> <PROJDIR>/os2386/hllcv.sym      <OWRELROOT>/binp/

    <CCCMD> <PROJDIR>/nt386/hllcv.dll       <OWRELROOT>/binnt/
    <CCCMD> <PROJDIR>/nt386/hllcv.sym       <OWRELROOT>/binnt/

    <CCCMD> <PROJDIR>/linux386/hllcv.dip    <OWRELROOT>/binl/
    <CCCMD> <PROJDIR>/linux386/hllcv.sym    <OWRELROOT>/binl/

    <CCCMD> <PROJDIR>/ntaxp/hllcv.dll       <OWRELROOT>/axpnt/
    <CCCMD> <PROJDIR>/ntaxp/hllcv.sym       <OWRELROOT>/axpnt/

    <CCCMD> <PROJDIR>/qnx386/hllcv.dip      <OWRELROOT>/qnx/watcom/wd/
    <CCCMD> <PROJDIR>/qnx386/hllcv.sym      <OWRELROOT>/qnx/sym/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
