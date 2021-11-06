# WATCOM GIT filter driver Builder Control file
# ==============================================

set PROJNAME=git

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/cpyright.exe <OWBINDIR>/<OWOBJDIR>/cpyright<CMDEXT>
    <CPCMD> <OWOBJDIR>/cvtutf8.exe  <OWBINDIR>/<OWOBJDIR>/cvtutf8<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/<OWOBJDIR>/cpyright<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/cpyright<CMDEXT>
    echo rm -f <OWBINDIR>/<OWOBJDIR>/cvtutf8<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/cvtutf8<CMDEXT>

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
