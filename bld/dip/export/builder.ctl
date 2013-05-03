# Export symbols DIP Builder Control file
# =======================================

set PROJDIR=<CWD>
set PROJNAME=export

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
    <CCCMD> dos386/export.dip     <OWRELROOT>/binw/
#    <CCCMD> dos386/export.sym     <OWRELROOT>/binw/export.dsy

    <CCCMD> wini86/export.dll     <OWRELROOT>/binw/
#    <CCCMD> wini86/export.sym     <OWRELROOT>/binw/

#    <CCCMD> os2i86/export.dll     <OWRELROOT>/binp/dll/
#    <CCCMD> os2i86/export.sym     <OWRELROOT>/binp/dll/
    <CCCMD> os2386/export.d32     <OWRELROOT>/binp/
    <CCCMD> os2386/export.sym     <OWRELROOT>/binp/

    <CCCMD> nt386/export.dll      <OWRELROOT>/binnt/
    <CCCMD> nt386/export.sym      <OWRELROOT>/binnt/

    <CCCMD> linux386/export.dip   <OWRELROOT>/binl/
    <CCCMD> linux386/export.sym   <OWRELROOT>/binl/

    <CCCMD> ntaxp/export.dll      <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/export.sym      <OWRELROOT>/axpnt/

    <CCCMD> qnx386/export.dip     <OWRELROOT>/qnx/watcom/wd/
    <CCCMD> qnx386/export.sym     <OWRELROOT>/qnx/sym/

    <CCCMD> ntx64/export.dll      <OWRELROOT>/binnt64/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
