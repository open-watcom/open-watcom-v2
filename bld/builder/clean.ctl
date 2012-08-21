# Build tools cleaning Control File
# =================================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> clean ]
#==================
    rm -f <OWBINDIR>/langdat<CMDEXT>
    rm -f <OWBINDIR>/mksetup<CMDEXT>
    rm -f <OWBINDIR>/cdsay<CMDEXT>
    rm -f <OWBINDIR>/sweep<CMDEXT>
    rm -f <OWBINDIR>/wsplice<CMDEXT>
    rm -f <OWBINDIR>/comstrip<CMDEXT>
    rm -f <OWBINDIR>/wgrep<CMDEXT>
    rm -f <OWBINDIR>/wtouch<CMDEXT>
    rm -f <OWBINDIR>/aliasgen<CMDEXT>
    rm -f <OWBINDIR>/genverrc<CMDEXT>
    rm -f <OWBINDIR>/wmake<CMDEXT>
# POSIX tools
    rm -f <OWBINDIR>/diff.exe
    rm -f <OWBINDIR>/rm.exe
    rm -f <OWBINDIR>/awk.exe
    rm -f <OWBINDIR>/sed.exe

[ BLOCK . . ]
#============

