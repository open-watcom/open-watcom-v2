# OMF tools Builder Control file
# ==============================

set PROJDIR=<CWD>
set PROJNAME=omftools

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> boot ]
    cdsay <PROJDIR>

[ BLOCK <1> boot ]
#=================
    <CPCMD> <PROJDIR>/<OWOBJDIR>/objchg.exe   <OWBINDIR>/objchg<CMDEXT>
    <CPCMD> <PROJDIR>/<OWOBJDIR>/objfind.exe  <OWBINDIR>/objfind<CMDEXT>
    <CPCMD> <PROJDIR>/<OWOBJDIR>/objlist.exe  <OWBINDIR>/objlist<CMDEXT>
    <CPCMD> <PROJDIR>/<OWOBJDIR>/objxdef.exe  <OWBINDIR>/objxdef<CMDEXT>
    <CPCMD> <PROJDIR>/<OWOBJDIR>/objxref.exe  <OWBINDIR>/objxref<CMDEXT>

[ BLOCK <1> bootclean ]
#======================
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

[ BLOCK . . ]
#============
cdsay <PROJDIR>
