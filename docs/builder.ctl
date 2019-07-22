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

[ BLOCK <BLDRULE> webdocs ]
#==========================
    pmake -d webdocs <2> <3> <4> <5> <6> <7> <8> <9> -h docset=webdocs

[ BLOCK <BLDRULE> docsclean clean ]
#==================================
    pmake -d build <PROJPMAKE> <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK <BLDRULE> docs rel webdocs ]
#===================================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> docs rel cprel ]
#=================================
    <CPCMD> dos/*.ihp       <OWRELROOT>/binw/
    <CPCMD> win/*.hlp       <OWRELROOT>/binw/
    <CPCMD> os2/*.inf       <OWRELROOT>/binp/help/
    <CPCMD> os2/*.hlp       <OWRELROOT>/binp/help/
    <CCCMD> nt/*.hlp        <OWRELROOT>/binnt/
    <CCCMD> nt/*.cnt        <OWRELROOT>/binnt/
    <CCCMD> htmlhelp/*.chm  <OWRELROOT>/binnt/help/
    <CCCMD> pdf/*.pdf       <OWRELROOT>/docs/

[ BLOCK <BLDRULE> cpwebdocs ]
#============================
    <CCCMD> pdf/*.pdf           <OWRELROOT>/
    <CCCMD> html/*.bmp          <OWRELROOT>/
    <CCCMD> html/c_readme.htm   <OWRELROOT>/c_readme.html
    <CCCMD> html/cgdoc.htm      <OWRELROOT>/cgdoc.html
    <CCCMD> html/cguide.htm     <OWRELROOT>/cguide.html
    <CCCMD> html/clib.htm       <OWRELROOT>/clib.html
    <CCCMD> html/clr.htm        <OWRELROOT>/clr.html
    <CCCMD> html/cpplib.htm     <OWRELROOT>/cpplib.html
    <CCCMD> html/ctools.htm     <OWRELROOT>/ctools.html
    <CCCMD> html/cw.htm         <OWRELROOT>/cw.html
    <CCCMD> html/devguide.htm   <OWRELROOT>/devguide.html
    <CCCMD> html/dwdoc.htm      <OWRELROOT>/dwdoc.html
    <CCCMD> html/f_readme.htm   <OWRELROOT>/f_readme.html
    <CCCMD> html/f77graph.htm   <OWRELROOT>/f77graph.html
    <CCCMD> html/f77lr.htm      <OWRELROOT>/f77lr.html
    <CCCMD> html/fpguide.htm    <OWRELROOT>/fpguide.html
    <CCCMD> html/ftools.htm     <OWRELROOT>/ftools.html
    <CCCMD> html/fuguide.htm    <OWRELROOT>/fuguide.html
    <CCCMD> html/guitool.htm    <OWRELROOT>/guitool.html
    <CCCMD> html/lguide.htm     <OWRELROOT>/lguide.html
    <CCCMD> html/owstl.htm      <OWRELROOT>/owstl.html
    <CCCMD> html/pguide.htm     <OWRELROOT>/pguide.html
    <CCCMD> html/vi.htm         <OWRELROOT>/vi.html
    <CCCMD> html/wd.htm         <OWRELROOT>/wd.html
    <CCCMD> html/wddoc.htm      <OWRELROOT>/wddoc.html
    <CCCMD> html/wgmlref.htm    <OWRELROOT>/wgmlref.html
    <CCCMD> html/wipfc.htm      <OWRELROOT>/wipfc.html

[ BLOCK . . ]
    set PROJPMAKE=

[ INCLUDE <OWROOT>/build/epilog.ctl ]
