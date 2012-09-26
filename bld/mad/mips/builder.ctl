# PowerPC MAD Builder Control file
# ================================

set PROJDIR=<CWD>
set PROJNAME=madmips

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
    <CCCMD> <PROJDIR>/dos386/madmips.mad     <OWRELROOT>/binw/
#    <CCCMD> <PROJDIR>/dos386/madmips.sym     <OWRELROOT>/binw/madmips.dsy

    <CCCMD> <PROJDIR>/wini86/madmips.dll     <OWRELROOT>/binw/
#    <CCCMD> <PROJDIR>/wini86/madmips.sym     <OWRELROOT>/binw/

#    <CCCMD> <PROJDIR>/os2i86/madmips.dll     <OWRELROOT>/binp/dll/
#    <CCCMD> <PROJDIR>/os2i86/madmips.sym     <OWRELROOT>/binp/dll/
    <CCCMD> <PROJDIR>/os2386/madmips.d32     <OWRELROOT>/binp/
    <CCCMD> <PROJDIR>/os2386/madmips.sym     <OWRELROOT>/binp/

    <CCCMD> <PROJDIR>/nt386/madmips.dll      <OWRELROOT>/binnt/
    <CCCMD> <PROJDIR>/nt386/madmips.sym      <OWRELROOT>/binnt/

    <CCCMD> <PROJDIR>/linux386/madmips.mad   <OWRELROOT>/binl/
    <CCCMD> <PROJDIR>/linux386/madmips.sym   <OWRELROOT>/binl/

    <CCCMD> <PROJDIR>/ntaxp/madmips.dll      <OWRELROOT>/axpnt/
    <CCCMD> <PROJDIR>/ntaxp/madmips.sym      <OWRELROOT>/axpnt/

    <CCCMD> <PROJDIR>/qnx386/madmips.mad     <OWRELROOT>/qnx/watcom/wd/
    <CCCMD> <PROJDIR>/qnx386/madmips.sym     <OWRELROOT>/qnx/sym/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
