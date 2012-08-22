# Documentation Builder Control file
# ==================================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
#=======================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> rel2 cprel2 ]
#========================
    <CPCMD> dos/*.ihp       <RELROOT>/binw/
    <CPCMD> win/*.hlp       <RELROOT>/binw/
    <CPCMD> os2/*.inf       <RELROOT>/binp/help/
    <CPCMD> os2/*.hlp       <RELROOT>/binp/help/
    <CCCMD> nt/*.hlp        <RELROOT>/binnt/
    <CCCMD> nt/*.cnt        <RELROOT>/binnt/
    <CCCMD> htmlhelp/*.chi  <RELROOT>/binnt/help/
    <CCCMD> htmlhelp/*.chm  <RELROOT>/binnt/help/
    <CCCMD> pdf/*.pdf       <RELROOT>/docs/
    <CPCMD> readme.txt      <RELROOT>/readme.txt
    <CPCMD> freadme.txt     <RELROOT>/freadme.txt
    <CPCMD> areadme.txt     <RELROOT>/areadme.txt
    <CPCMD> instlic.txt     <RELROOT>/instlic.txt

[ BLOCK <1> clean ]
#==================
    pmake -d all <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
