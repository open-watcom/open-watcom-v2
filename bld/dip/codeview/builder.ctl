# CodeView DIP Builder Control file
# =================================

set PROJDIR=<CWD>
set PROJNAME=codeview

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
    <CCCMD> dos386/codeview.dip       <OWRELROOT>/binw/
#    <CCCMD> dos386/codeview.sym       <OWRELROOT>/binw/codeview.dsy

    <CCCMD> wini86/codeview.dll       <OWRELROOT>/binw/
#    <CCCMD> wini86/codeview.sym       <OWRELROOT>/binw/

#    <CCCMD> os2i86/codeview.dll       <OWRELROOT>/binp/dll/
#    <CCCMD> os2i86/codeview.sym       <OWRELROOT>/binp/dll/
    <CCCMD> os2386/codeview.d32       <OWRELROOT>/binp/
    <CCCMD> os2386/codeview.sym       <OWRELROOT>/binp/

    <CCCMD> nt386/codeview.dll        <OWRELROOT>/binnt/
    <CCCMD> nt386/codeview.sym        <OWRELROOT>/binnt/

    <CCCMD> linux386/codeview.dip     <OWRELROOT>/binl/
    <CCCMD> linux386/codeview.sym     <OWRELROOT>/binl/

    <CCCMD> ntaxp/codeview.dll        <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/codeview.sym        <OWRELROOT>/axpnt/

    <CCCMD> qnx386/codeview.dip       <OWRELROOT>/qnx/watcom/wd/
    <CCCMD> qnx386/codeview.sym       <OWRELROOT>/qnx/sym/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
