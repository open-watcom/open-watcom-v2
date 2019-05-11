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
        <CCCMD> pdf/*.pdf           <OWRELROOT>/docs/
        <CCCMD> html/*.bmp          <OWRELROOT>/docs/
        <CCCMD> html/c_readme.htm   <OWRELROOT>/docs/c_readme.html
        <CCCMD> html/cgdoc.htm      <OWRELROOT>/docs/cgdoc.html
        <CCCMD> html/cguide.htm     <OWRELROOT>/docs/cguide.html
        <CCCMD> html/clib.htm       <OWRELROOT>/docs/clib.html
        <CCCMD> html/clr.htm        <OWRELROOT>/docs/clr.html
        <CCCMD> html/cpplib.htm     <OWRELROOT>/docs/cpplib.html
        <CCCMD> html/ctools.htm     <OWRELROOT>/docs/ctools.html
        <CCCMD> html/cw.htm         <OWRELROOT>/docs/cw.html
        <CCCMD> html/devguide.htm   <OWRELROOT>/docs/devguide.html
        <CCCMD> html/dwdoc.htm      <OWRELROOT>/docs/dwdoc.html
        <CCCMD> html/f_readme.htm   <OWRELROOT>/docs/f_readme.html
        <CCCMD> html/f77graph.htm   <OWRELROOT>/docs/f77graph.html
        <CCCMD> html/f77lr.htm      <OWRELROOT>/docs/f77lr.html
        <CCCMD> html/fpguide.htm    <OWRELROOT>/docs/fpguide.html
        <CCCMD> html/ftools.htm     <OWRELROOT>/docs/ftools.html
        <CCCMD> html/fuguide.htm    <OWRELROOT>/docs/fuguide.html
        <CCCMD> html/guitool.htm    <OWRELROOT>/docs/guitool.html
        <CCCMD> html/lguide.htm     <OWRELROOT>/docs/lguide.html
        <CCCMD> html/owstl.htm      <OWRELROOT>/docs/owstl.html
        <CCCMD> html/pguide.htm     <OWRELROOT>/docs/pguide.html
        <CCCMD> html/vi.htm         <OWRELROOT>/docs/vi.html
        <CCCMD> html/wd.htm         <OWRELROOT>/docs/wd.html
        <CCCMD> html/wddoc.htm      <OWRELROOT>/docs/wddoc.html
        <CCCMD> html/wipfc.htm      <OWRELROOT>/docs/wipfc.html
    [ ENDIF ]
    <CCCMD> txt/*.txt       <OWRELROOT>/
    <CPCMD> areadme.txt     <OWRELROOT>/areadme.txt

[ BLOCK . . ]
    set PROJPMAKE=
    set PMAKEKEY=

[ INCLUDE <OWROOT>/build/epilog.ctl ]
