# FEMISC Builder Control file
# ===========================

set PROJNAME=femisc

set BINTOOL=0

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/optencod.exe  <OWBINDIR>/<OWOBJDIR>/optencod<CMDEXT>
    <CPCMD> <OWOBJDIR>/msgencod.exe  <OWBINDIR>/<OWOBJDIR>/msgencod<CMDEXT>
    <CPCMD> <OWOBJDIR>/findhash.exe  <OWBINDIR>/<OWOBJDIR>/findhash<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/<OWOBJDIR>/optencod<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/optencod<CMDEXT>
    echo rm -f <OWBINDIR>/<OWOBJDIR>/msgencod<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/msgencod<CMDEXT>
    echo rm -f <OWBINDIR>/<OWOBJDIR>/findhash<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/findhash<CMDEXT>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    # Currently not built by default

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
