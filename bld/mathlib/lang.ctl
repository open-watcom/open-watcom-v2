# MATHLIB Builder Control file
# ============================
set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 cprel2 acprel2 ]
#================================
    <CPCMD> <devdir>\mathlib\library\msdos.286\mc\mathc.lib <relroot>\rel2\lib286\mathc.lib
    <CPCMD> <devdir>\mathlib\library\msdos.286\mh\mathh.lib <relroot>\rel2\lib286\mathh.lib
    <CPCMD> <devdir>\mathlib\library\msdos.286\ml\mathl.lib <relroot>\rel2\lib286\mathl.lib
    <CPCMD> <devdir>\mathlib\library\msdos.286\mm\mathm.lib <relroot>\rel2\lib286\mathm.lib
    <CPCMD> <devdir>\mathlib\library\msdos.286\ms\maths.lib <relroot>\rel2\lib286\maths.lib
    <CPCMD> <devdir>\mathlib\library\msdos.287\mc\mathc.lib <relroot>\rel2\lib286\math87c.lib
    <CPCMD> <devdir>\mathlib\library\msdos.287\mh\mathh.lib <relroot>\rel2\lib286\math87h.lib
    <CPCMD> <devdir>\mathlib\library\msdos.287\ml\mathl.lib <relroot>\rel2\lib286\math87l.lib
    <CPCMD> <devdir>\mathlib\library\msdos.287\mm\mathm.lib <relroot>\rel2\lib286\math87m.lib
    <CPCMD> <devdir>\mathlib\library\msdos.287\ms\maths.lib <relroot>\rel2\lib286\math87s.lib
    <CPCMD> <devdir>\mathlib\library\msdos.386\ms_r\maths.lib <relroot>\rel2\lib386\math3r.lib
    <CPCMD> <devdir>\mathlib\library\msdos.386\ms_s\maths.lib <relroot>\rel2\lib386\math3s.lib
    <CPCMD> <devdir>\mathlib\library\msdos.387\ms_r\maths.lib <relroot>\rel2\lib386\math387r.lib
    <CPCMD> <devdir>\mathlib\library\msdos.387\ms_s\maths.lib <relroot>\rel2\lib386\math387s.lib
    <CPCMD> <devdir>\mathlib\library\windows.286\mc\mathc.lib <relroot>\rel2\lib286\win\mathc.lib
    <CPCMD> <devdir>\mathlib\library\windows.286\ml\mathl.lib <relroot>\rel2\lib286\win\mathl.lib
    <CPCMD> <devdir>\mathlib\library\windows.287\mc\mathc.lib <relroot>\rel2\lib286\win\math87c.lib
    <CPCMD> <devdir>\mathlib\library\windows.287\ml\mathl.lib <relroot>\rel2\lib286\win\math87l.lib
#   <CPCMD> <devdir>\mathlib\library\winnt.axp\_s\math.lib <relroot>\rel2\libaxp\math.lib

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean
