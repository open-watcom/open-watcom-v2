# Documentation Builder Control file
# ==================================

set PROJNAME=docs

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

cdsay <PROJDIR>

[ BLOCK .<OWGUINOBUILD> .1 ]
    set PROJPMAKE=guitool .not .and

[ BLOCK <BLDRULE> docs build rel ]
#=================================
    pmake -d build <PROJPMAKE> <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <BLDRULE> docsclean clean ]
#==================================
    pmake -d build <PROJPMAKE> <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK <BLDRULE> docs rel ]
#===========================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> docs rel cprel ]
#=================================
    <CCCMD> dos/*.ihp       <OWRELROOT>/binw/
    <CCCMD> win/*.hlp       <OWRELROOT>/binw/
    <CCCMD> os2/*.inf       <OWRELROOT>/binp/help/
    <CCCMD> os2/*.hlp       <OWRELROOT>/binp/help/
    <CCCMD> nt/*.hlp        <OWRELROOT>/binnt/
    <CCCMD> nt/*.cnt        <OWRELROOT>/binnt/
    <CCCMD> htmlhelp/*.chm  <OWRELROOT>/binnt/help/
    <CCCMD> pdf/*.pdf       <OWRELROOT>/docs/

[ BLOCK . . ]
    set PROJPMAKE=

[ INCLUDE <OWROOT>/build/epilog.ctl ]
