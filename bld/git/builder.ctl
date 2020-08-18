# WATCOM GIT filter driver Builder Control file
# ==============================================

set PROJNAME=git

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/cpyright.exe <OWBINDIR>/cpyright<CMDEXT>
    <CPCMD> <OWOBJDIR>/sjisutf8.exe <OWBINDIR>/sjisutf8<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/cpyright<CMDEXT>
    rm -f <OWBINDIR>/cpyright<CMDEXT>
    echo rm -f <OWBINDIR>/sjisutf8<CMDEXT>
    rm -f <OWBINDIR>/sjisutf8<CMDEXT>

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
