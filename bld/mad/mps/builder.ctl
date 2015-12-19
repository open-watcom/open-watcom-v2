# PowerPC MAD Builder Control file
# ================================

set PROJDIR=<CWD>
set PROJNAME=madmps

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
    <CCCMD> dos386/mad*.mad     <OWRELROOT>/binw/
#    <CCCMD> dos386/madmps.sym   <OWRELROOT>/binw/madmps.dsy

    <CCCMD> wini86/mad*.dll     <OWRELROOT>/binw/
#    <CCCMD> wini86/mad*.sym     <OWRELROOT>/binw/

#    <CCCMD> os2i86/mad*.dll     <OWRELROOT>/binp/dll/
#    <CCCMD> os2i86/mad*.sym     <OWRELROOT>/binp/dll/
    <CCCMD> os2386/mad*.d32     <OWRELROOT>/binp/
    <CCCMD> os2386/mad*.sym     <OWRELROOT>/binp/

    <CCCMD> nt386/mad*.dll      <OWRELROOT>/binnt/
    <CCCMD> nt386/mad*.sym      <OWRELROOT>/binnt/

    <CCCMD> linux386/mad*.mad   <OWRELROOT>/binl/
    <CCCMD> linux386/mad*.sym   <OWRELROOT>/binl/

    <CCCMD> ntaxp/mad*.dll      <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/mad*.sym      <OWRELROOT>/axpnt/

    <CCCMD> qnx386/mad*.mad     <OWRELROOT>/qnx/watcom/wd/
    <CCCMD> qnx386/mad*.sym     <OWRELROOT>/qnx/sym/

    <CCCMD> ntx64/mad*.dll      <OWRELROOT>/binnt64/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
