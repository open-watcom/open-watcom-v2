# Graphlib Builder Control file
# =============================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/bat/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
#=======================
    cd <PROJDIR>/../pgchart
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h
    cdsay <PROJDIR>
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> rel2 cprel2 ]
#========================
    <CCCMD> <DEVDIR>/graphlib/fix/dosi86/graph.lib    <RELROOT>/lib286/dos/graph.lib
    <CCCMD> <DEVDIR>/graphlib/fix/dos386/graph.lib    <RELROOT>/lib386/dos/graph.lib
    <CCCMD> <DEVDIR>/graphlib/fix/os2i86/seginit.obj  <RELROOT>/lib286/os2/graphp.obj
    <CCCMD> <DEVDIR>/graphlib/fix/qnxi86/graph.lib    <RELROOT>/lib286/qnx/graph.lib
    <CCCMD> <DEVDIR>/graphlib/fix/qnx386/graph3r.lib  <RELROOT>/lib386/qnx/graph3r.lib
    <CCCMD> <DEVDIR>/graphlib/fix/qnx386/graph3s.lib  <RELROOT>/lib386/qnx/graph3s.lib

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean
    cd <PROJDIR>/../pgchart
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean
    cdsay <PROJDIR>/fix
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
