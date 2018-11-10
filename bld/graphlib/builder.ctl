# Graphlib Builder Control file
# =============================

set PROJNAME=graphlib

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> fix/dosi86/graph.lib    <OWRELROOT>/lib286/dos/graph.lib
    <CCCMD> fix/dosi86/graph2.lib   <OWRELROOT>/lib286/dos/graph2.lib
    <CCCMD> fix/dos386/graph.lib    <OWRELROOT>/lib386/dos/graph.lib
    <CCCMD> fix/dos386/graph2.lib   <OWRELROOT>/lib386/dos/graph2.lib
    <CCCMD> fix/qnxi86/graph.lib    <OWRELROOT>/lib286/qnx/graph.lib
    <CCCMD> fix/qnxi86/graph2.lib   <OWRELROOT>/lib286/qnx/graph2.lib
    <CCCMD> fix/qnx386/graph3r.lib  <OWRELROOT>/lib386/qnx/graph3r.lib
    <CCCMD> fix/qnx386/graph23r.lib <OWRELROOT>/lib386/qnx/graph23r.lib
    <CCCMD> fix/qnx386/graph3s.lib  <OWRELROOT>/lib386/qnx/graph3s.lib
    <CCCMD> fix/qnx386/graph23s.lib <OWRELROOT>/lib386/qnx/graph23s.lib

    <CCCMD> os2i86/seginit.obj      <OWRELROOT>/lib286/os2/graphp.obj

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
