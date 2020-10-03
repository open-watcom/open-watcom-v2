# F77 errsrc Builder Control file
# ================================

set PROJNAME=errsrc

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/errsrc.exe  <OWBINDIR>/errsrc<CMDEXT>
    <CPCMD> <OWOBJDIR>/errdoc.exe  <OWBINDIR>/errdoc<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/errsrc<CMDEXT>
    rm -f <OWBINDIR>/errsrc<CMDEXT>
    echo rm -f <OWBINDIR>/errdoc<CMDEXT>
    rm -f <OWBINDIR>/errdoc<CMDEXT>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    # Currently not built by default

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
