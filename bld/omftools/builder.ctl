# OMF tools Builder Control file
# ==============================

set PROJDIR=<CWD>
set PROJNAME=omftools

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BINTOOL> rel ]
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> rel ]
#========================
    <CPCMD> <OWOBJDIR>/objchg.exe   <OWBINDIR>/objchg<CMDEXT>
    <CPCMD> <OWOBJDIR>/objfind.exe  <OWBINDIR>/objfind<CMDEXT>
    <CPCMD> <OWOBJDIR>/objlist.exe  <OWBINDIR>/objlist<CMDEXT>
    <CPCMD> <OWOBJDIR>/objxdef.exe  <OWBINDIR>/objxdef<CMDEXT>
    <CPCMD> <OWOBJDIR>/objxref.exe  <OWBINDIR>/objxref<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/objchg<CMDEXT>
    rm -f <OWBINDIR>/objchg<CMDEXT>
    echo rm -f <OWBINDIR>/objfind<CMDEXT>
    rm -f <OWBINDIR>/objfind<CMDEXT>
    echo rm -f <OWBINDIR>/objlist<CMDEXT>
    rm -f <OWBINDIR>/objlist<CMDEXT>
    echo rm -f <OWBINDIR>/objxdef<CMDEXT>
    rm -f <OWBINDIR>/objxdef<CMDEXT>
    echo rm -f <OWBINDIR>/objxref<CMDEXT>
    rm -f <OWBINDIR>/objxref<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
    # Currently not built by default

[ BLOCK . . ]
#============
cdsay <PROJDIR>
