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
    <CCCMD> dos386/madmips.mad     <OWRELROOT>/binw/
#    <CCCMD> dos386/madmips.sym     <OWRELROOT>/binw/madmips.dsy

    <CCCMD> wini86/madmips.dll     <OWRELROOT>/binw/
#    <CCCMD> wini86/madmips.sym     <OWRELROOT>/binw/

#    <CCCMD> os2i86/madmips.dll     <OWRELROOT>/binp/dll/
#    <CCCMD> os2i86/madmips.sym     <OWRELROOT>/binp/dll/
    <CCCMD> os2386/madmips.d32     <OWRELROOT>/binp/
    <CCCMD> os2386/madmips.sym     <OWRELROOT>/binp/

    <CCCMD> nt386/madmips.dll      <OWRELROOT>/binnt/
    <CCCMD> nt386/madmips.sym      <OWRELROOT>/binnt/

    <CCCMD> linux386/madmips.mad   <OWRELROOT>/binl/
    <CCCMD> linux386/madmips.sym   <OWRELROOT>/binl/

    <CCCMD> ntaxp/madmips.dll      <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/madmips.sym      <OWRELROOT>/axpnt/

    <CCCMD> qnx386/madmips.mad     <OWRELROOT>/qnx/watcom/wd/
    <CCCMD> qnx386/madmips.sym     <OWRELROOT>/qnx/sym/

    <CCCMD> ntx64/madmips.dll      <OWRELROOT>/binnt64/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
