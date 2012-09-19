# Documentation Builder Control file
# ==================================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel cprel ]
#======================
    <CPCMD> dos/*.ihp       <OWRELROOT>/binw/
    <CPCMD> win/*.hlp       <OWRELROOT>/binw/
    <CPCMD> os2/*.inf       <OWRELROOT>/binp/help/
    <CPCMD> os2/*.hlp       <OWRELROOT>/binp/help/
    <CCCMD> nt/*.hlp        <OWRELROOT>/binnt/
    <CCCMD> nt/*.cnt        <OWRELROOT>/binnt/
    <CCCMD> htmlhelp/*.chi  <OWRELROOT>/binnt/help/
    <CCCMD> htmlhelp/*.chm  <OWRELROOT>/binnt/help/
    <CCCMD> pdf/*.pdf       <OWRELROOT>/docs/
    <CPCMD> readme.txt      <OWRELROOT>/readme.txt
    <CPCMD> freadme.txt     <OWRELROOT>/freadme.txt
    <CPCMD> areadme.txt     <OWRELROOT>/areadme.txt
    <CPCMD> instlic.txt     <OWRELROOT>/instlic.txt

[ BLOCK . . ]
#============
cdsay <PROJDIR>
