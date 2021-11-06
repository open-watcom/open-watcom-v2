# OMF tools Builder Control file
# ==============================

set PROJNAME=omftools

set BINTOOL=0

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/objchg.exe   <OWBINDIR>/<OWOBJDIR>/objchg<CMDEXT>
    <CPCMD> <OWOBJDIR>/objfind.exe  <OWBINDIR>/<OWOBJDIR>/objfind<CMDEXT>
    <CPCMD> <OWOBJDIR>/objlist.exe  <OWBINDIR>/<OWOBJDIR>/objlist<CMDEXT>
    <CPCMD> <OWOBJDIR>/objxdef.exe  <OWBINDIR>/<OWOBJDIR>/objxdef<CMDEXT>
    <CPCMD> <OWOBJDIR>/objxref.exe  <OWBINDIR>/<OWOBJDIR>/objxref<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/<OWOBJDIR>/objchg<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/objchg<CMDEXT>
    echo rm -f <OWBINDIR>/<OWOBJDIR>/objfind<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/objfind<CMDEXT>
    echo rm -f <OWBINDIR>/<OWOBJDIR>/objlist<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/objlist<CMDEXT>
    echo rm -f <OWBINDIR>/<OWOBJDIR>/objxdef<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/objxdef<CMDEXT>
    echo rm -f <OWBINDIR>/<OWOBJDIR>/objxref<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/objxref<CMDEXT>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    # Currently not built by default

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
