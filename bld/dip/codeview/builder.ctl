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
    <CCCMD> <PROJDIR>/dos386/codeview.dip       <OWRELROOT>/binw/
#    <CCCMD> <PROJDIR>/dos386/codeview.sym       <OWRELROOT>/binw/codeview.dsy

    <CCCMD> <PROJDIR>/wini86/codeview.dll       <OWRELROOT>/binw/
#    <CCCMD> <PROJDIR>/wini86/codeview.sym       <OWRELROOT>/binw/

#    <CCCMD> <PROJDIR>/os2i86/codeview.dll       <OWRELROOT>/binp/dll/
#    <CCCMD> <PROJDIR>/os2i86/codeview.sym       <OWRELROOT>/binp/dll/
    <CCCMD> <PROJDIR>/os2386/codeview.d32       <OWRELROOT>/binp/
    <CCCMD> <PROJDIR>/os2386/codeview.sym       <OWRELROOT>/binp/

    <CCCMD> <PROJDIR>/nt386/codeview.dll        <OWRELROOT>/binnt/
    <CCCMD> <PROJDIR>/nt386/codeview.sym        <OWRELROOT>/binnt/

    <CCCMD> <PROJDIR>/linux386/codeview.dip     <OWRELROOT>/binl/
    <CCCMD> <PROJDIR>/linux386/codeview.sym     <OWRELROOT>/binl/

    <CCCMD> <PROJDIR>/ntaxp/codeview.dll        <OWRELROOT>/axpnt/
    <CCCMD> <PROJDIR>/ntaxp/codeview.sym        <OWRELROOT>/axpnt/

    <CCCMD> <PROJDIR>/qnx386/codeview.dip       <OWRELROOT>/qnx/watcom/wd/
    <CCCMD> <PROJDIR>/qnx386/codeview.sym       <OWRELROOT>/qnx/sym/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
