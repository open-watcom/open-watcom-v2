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
        <CCCMD> pdf/*.pdf           <OWRELROOT>/webdocs/
        <CCCMD> html/*.bmp          <OWRELROOT>/webdocs/
        <CCCMD> html/c_readme.htm   <OWRELROOT>/webdocs/c_readme.html
        <CCCMD> html/cgdoc.htm      <OWRELROOT>/webdocs/cgdoc.html
        <CCCMD> html/cguide.htm     <OWRELROOT>/webdocs/cguide.html
        <CCCMD> html/clib.htm       <OWRELROOT>/webdocs/clib.html
        <CCCMD> html/clr.htm        <OWRELROOT>/webdocs/clr.html
        <CCCMD> html/cpplib.htm     <OWRELROOT>/webdocs/cpplib.html
        <CCCMD> html/ctools.htm     <OWRELROOT>/webdocs/ctools.html
        <CCCMD> html/cw.htm         <OWRELROOT>/webdocs/cw.html
        <CCCMD> html/devguide.htm   <OWRELROOT>/webdocs/devguide.html
        <CCCMD> html/dwdoc.htm      <OWRELROOT>/webdocs/dwdoc.html
        <CCCMD> html/f_readme.htm   <OWRELROOT>/webdocs/f_readme.html
        <CCCMD> html/f77graph.htm   <OWRELROOT>/webdocs/f77graph.html
        <CCCMD> html/f77lr.htm      <OWRELROOT>/webdocs/f77lr.html
        <CCCMD> html/fpguide.htm    <OWRELROOT>/webdocs/fpguide.html
        <CCCMD> html/ftools.htm     <OWRELROOT>/webdocs/ftools.html
        <CCCMD> html/fuguide.htm    <OWRELROOT>/webdocs/fuguide.html
        <CCCMD> html/guitool.htm    <OWRELROOT>/webdocs/guitool.html
        <CCCMD> html/lguide.htm     <OWRELROOT>/webdocs/lguide.html
        <CCCMD> html/owstl.htm      <OWRELROOT>/webdocs/owstl.html
        <CCCMD> html/pguide.htm     <OWRELROOT>/webdocs/pguide.html
        <CCCMD> html/vi.htm         <OWRELROOT>/webdocs/vi.html
        <CCCMD> html/wd.htm         <OWRELROOT>/webdocs/wd.html
        <CCCMD> html/wddoc.htm      <OWRELROOT>/webdocs/wddoc.html
        <CCCMD> html/wgmlref.htm    <OWRELROOT>/webdocs/wgmlref.html
        <CCCMD> html/wipfc.htm      <OWRELROOT>/webdocs/wipfc.html
    [ ENDIF ]
    <CCCMD> txt/*.txt       <OWRELROOT>/
    <CPCMD> areadme.txt     <OWRELROOT>/areadme.txt

[ BLOCK . . ]
    set PROJPMAKE=
    set PMAKEKEY=

[ INCLUDE <OWROOT>/build/epilog.ctl ]
