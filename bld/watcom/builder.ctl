# WATCOM CRTL extension libraries Builder Control file
# ====================================================

set PROJNAME=watcom

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

cdsay <PROJDIR>

[ BLOCK <BLDRULE> boot ]
#=================
    mkdir <OWOBJDIR>
    cdsay <OWOBJDIR>
    wmake -h -f ../binmake bootstrap=1
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> bootclean ]
#============================
    echo rm -rf <OWOBJDIR>
    rm -rf <OWOBJDIR>

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
