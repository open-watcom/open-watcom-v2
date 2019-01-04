# Documentation Builder Control file
# ==================================

set PROJNAME=docs

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

cdsay <PROJDIR>

set PMAKEKEY=txt

[ BLOCK .<OWTXTDOCBUILD> . ]
    set PMAKEKEY=build

[ BLOCK .<OWGUINOBUILD> .1 ]
    set PROJPMAKE=guitool .not .and

[ BLOCK <BLDRULE> docs build rel ]
#=================================
    pmake -d <PMAKEKEY> <PROJPMAKE> <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <BLDRULE> doctrav ]
#==========================
    cd pdf
    pmake -d all <2> <3> <4> <5> <6> <7> <8> <9> -h docset=doctrav
    cd ..
    cd html
    pmake -d all <2> <3> <4> <5> <6> <7> <8> <9> -h docset=doctrav
    cd ..

[ BLOCK <BLDRULE> docsclean clean ]
#==================================
    pmake -d <PMAKEKEY> <PROJPMAKE> <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK <BLDRULE> docs rel doctrav ]
#===================================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> docs rel cprel ]
#=================================
    [ IFDEF <PMAKEKEY> build ]
        <CPCMD> dos/*.ihp       <OWRELROOT>/binw/
        <CPCMD> win/*.hlp       <OWRELROOT>/binw/
        <CPCMD> os2/*.inf       <OWRELROOT>/binp/help/
        <CPCMD> os2/*.hlp       <OWRELROOT>/binp/help/
        <CCCMD> nt/*.hlp        <OWRELROOT>/binnt/
        <CCCMD> nt/*.cnt        <OWRELROOT>/binnt/
        <CCCMD> htmlhelp/*.chm  <OWRELROOT>/binnt/help/
        <CCCMD> pdf/*.pdf       <OWRELROOT>/docs/
    [ ENDIF ]
    <CCCMD> txt/*.txt       <OWRELROOT>/
    <CPCMD> areadme.txt     <OWRELROOT>/areadme.txt

[ BLOCK <BLDRULE> cpdoctrav ]
#============================
    [ IFDEF <PMAKEKEY> build ]
        <CCCMD> pdf/*.pdf       <OWRELROOT>/docs/
        <CCCMD> html/*.htm      <OWRELROOT>/docs/
        <CCCMD> html/*.bmp      <OWRELROOT>/docs/
    [ ENDIF ]
    <CCCMD> txt/*.txt       <OWRELROOT>/
    <CPCMD> areadme.txt     <OWRELROOT>/areadme.txt

[ BLOCK . . ]
    set PROJPMAKE=
    set PMAKEKEY=

[ INCLUDE <OWROOT>/build/epilog.ctl ]
