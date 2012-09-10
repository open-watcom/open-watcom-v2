# Graphlib Builder Control file
# =============================

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
    <CCCMD> fix/dosi86/graph.lib    <OWRELROOT>/lib286/dos/graph.lib
    <CCCMD> fix/dos386/graph.lib    <OWRELROOT>/lib386/dos/graph.lib
    <CCCMD> fix/os2i86/seginit.obj  <OWRELROOT>/lib286/os2/graphp.obj
    <CCCMD> fix/qnxi86/graph.lib    <OWRELROOT>/lib286/qnx/graph.lib
    <CCCMD> fix/qnx386/graph3r.lib  <OWRELROOT>/lib386/qnx/graph3r.lib
    <CCCMD> fix/qnx386/graph3s.lib  <OWRELROOT>/lib386/qnx/graph3s.lib

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
