# bmp2eps Builder Control file
# ============================

set PROJDIR=<CWD>
set PROJNAME=bmp2eps

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/binrule.ctl ]

[ BLOCK <1> build rel ]
    cdsay <PROJDIR>

[ BLOCK <1> build rel ]
#======================
    <CPCMD> <OWOBJDIR>/bmp2eps.exe <OWBINDIR>/bmp2eps<CMDEXT>

[ BLOCK <1> clean ]
#==================
    echo rm -f <OWBINDIR>/bmp2eps<CMDEXT>
    rm -f <OWBINDIR>/bmp2eps<CMDEXT>

[ BLOCK . . ]
#============
cdsay <PROJDIR>
