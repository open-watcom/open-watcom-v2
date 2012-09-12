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

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> <PROJDIR>/dos386/export.dip     <OWRELROOT>/binw/
#    <CCCMD> <PROJDIR>/dos386/export.sym     <OWRELROOT>/binw/export.dsy

    <CCCMD> <PROJDIR>/wini86/export.dll     <OWRELROOT>/binw/
#    <CCCMD> <PROJDIR>/wini86/export.sym     <OWRELROOT>/binw/

#    <CCCMD> <PROJDIR>/os2i86/export.dll     <OWRELROOT>/binp/dll/
#    <CCCMD> <PROJDIR>/os2i86/export.sym     <OWRELROOT>/binp/dll/
    <CCCMD> <PROJDIR>/os2386/export.d32     <OWRELROOT>/binp/
    <CCCMD> <PROJDIR>/os2386/export.sym     <OWRELROOT>/binp/

    <CCCMD> <PROJDIR>/nt386/export.dll      <OWRELROOT>/binnt/
    <CCCMD> <PROJDIR>/nt386/export.sym      <OWRELROOT>/binnt/

    <CCCMD> <PROJDIR>/linux386/export.dip   <OWRELROOT>/binl/
    <CCCMD> <PROJDIR>/linux386/export.sym   <OWRELROOT>/binl/

    <CCCMD> <PROJDIR>/ntaxp/export.dll      <OWRELROOT>/axpnt/
    <CCCMD> <PROJDIR>/ntaxp/export.sym      <OWRELROOT>/axpnt/

    <CCCMD> <PROJDIR>/qnx386/export.dip     <OWRELROOT>/qnx/watcom/wd/
    <CCCMD> <PROJDIR>/qnx386/export.sym     <OWRELROOT>/qnx/sym/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
