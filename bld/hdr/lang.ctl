# H Builder Control file
# ======================

set PROJDIR=<CWD>
set reldir=<relroot>\rel2

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> nlang ]
    set reldir=<dwatcom>
    set CPCMD=copy

[ BLOCK <1> rel2 cprel2 acprel2 nlang ]
#======================================
    <CPCMD> <devdir>\hdr\dos\*.h* <reldir>\h\
    <CPCMD> <devdir>\hdr\dos\*.cnv <reldir>\h\
    <CPCMD> <devdir>\hdr\dos\sys\*.h <reldir>\h\sys\
    <CPCMD> <devdir>\hdr\dos\win\*.h <reldir>\h\win\
    <CPCMD> <devdir>\hdr\qnx\*.h* <reldir>\qh\
    <CPCMD> <devdir>\hdr\qnx\sys\*.h <reldir>\qh\sys\
#rem<CPCMD> <devdir>\hdr\penpoint\*.h* <reldir>\ph\
#rem<CPCMD> <devdir>\hdr\penpoint\sys\*.h <reldir>\ph\sys\
#rem<CPCMD> <devdir>\watcom\nt_h\*.h* <reldir>\h\nt\
#rem<CPCMD> <devdir>\watcom\nt_h\*.dlg <reldir>\h\nt\

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean
