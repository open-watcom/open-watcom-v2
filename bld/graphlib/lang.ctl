# Graphlib Builder Control file
# =============================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    cdsay o.s
    wmake -h -i
    cdsay ..\o.3r
    wmake -h
    cdsay ..\o.3s
    wmake -h
    cdsay ..\o.nec
    wmake -h
    cdsay ..\o.n3r
    wmake -h
    cdsay ..\o.n3s
    wmake -h
    cdsay ..\o.qs
    wmake -h
    cdsay ..\o.q3r
    wmake -h
    cdsay ..\o.q3s
    wmake -h
    cdsay ..\..\pgchart\o.s
    wmake -h
    cdsay ..\o.3r
    wmake -h
    cdsay ..\o.3s
    wmake -h
    cdsay ..\..\graphlib\fix
    wmake -h
    cd <PROJDIR>

[ BLOCK <1> rel2 cprel2 ]
#========================
    <CPCMD> <devdir>\graphlib\fix\graph.lib    <relroot>\rel2\lib286\dos\graph.lib
    <CPCMD> <devdir>\graphlib\fix\graph386.lib <relroot>\rel2\lib386\dos\graph.lib
    <CPCMD> <devdir>\graphlib\fix\graphp.2bj   <relroot>\rel2\lib286\os2\graphp.obj
    <CPCMD> <devdir>\graphlib\fix\graph98.lib  <relroot>\rel2\lib286\dos\graph98.lib
    <CPCMD> <devdir>\graphlib\fix\graph983.lib <relroot>\rel2\lib386\dos\graph98.lib
    <CPCMD> <devdir>\graphlib\fix\graphq.lib   <relroot>\rel2\lib286\qnx\graph.lib
    <CPCMD> <devdir>\graphlib\fix\graphq3r.lib  <relroot>\rel2\lib386\qnx\graph3r.lib
    <CPCMD> <devdir>\graphlib\fix\graphq3s.lib  <relroot>\rel2\lib386\qnx\graph3s.lib

[ BLOCK <1> clean ]
#==================
    cdsay o.s
    wmake -h -i clean
    cdsay ..\o.3r
    wmake -h clean
    cdsay ..\o.3s
    wmake -h clean
    cdsay ..\o.nec
    wmake -h clean
    cdsay ..\o.n3r
    wmake -h clean
    cdsay ..\o.n3s
    wmake -h clean
    cdsay ..\o.qs
    wmake -h clean
    cdsay ..\o.q3r
    wmake -h clean
    cdsay ..\o.q3s
    wmake -h clean
    cdsay ..\..\pgchart\o.s
    wmake -h clean
    cdsay ..\o.3r
    wmake -h clean
    cdsay ..\o.3s
    wmake -h clean
    cdsay ..\..\graphlib\fix
    wmake -h clean
    cd <PROJDIR>
