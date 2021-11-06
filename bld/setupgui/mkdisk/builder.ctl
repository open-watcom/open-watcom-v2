# mkdisk Builder Control file
# ===========================
 
set PROJNAME=mkdisk

set BINTOOL=0

set PROJDIR=<CWD>
 
[ INCLUDE <OWROOT>/build/prolog.ctl ]
 
[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/mkexezip.exe <OWBINDIR>/<OWOBJDIR>/mkexezip<CMDEXT>
    <CPCMD> <OWOBJDIR>/uzip.exe <OWBINDIR>/<OWOBJDIR>/uzip<CMDEXT>
    <CPCMD> <OWOBJDIR>/langdat.exe <OWBINDIR>/<OWOBJDIR>/langdat<CMDEXT>
    <CPCMD> <OWOBJDIR>/mkinf.exe <OWBINDIR>/<OWOBJDIR>/mkinf<CMDEXT>
 
[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/<OWOBJDIR>/mkexezip<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/mkexezip<CMDEXT>
    echo rm -f <OWBINDIR>/<OWOBJDIR>/uzip<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/uzip<CMDEXT>
    echo rm -f <OWBINDIR>/<OWOBJDIR>/langdat<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/langdat<CMDEXT>
    echo rm -f <OWBINDIR>/<OWOBJDIR>/mkinf<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/mkinf<CMDEXT>
 
[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
