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
    <CPCMD> <OWOBJDIR>/mkexezip.exe <OWBINDIR>/mkexezip<CMDEXT>
    <CPCMD> <OWOBJDIR>/uzip.exe <OWBINDIR>/uzip<CMDEXT>
    <CPCMD> <OWOBJDIR>/langdat.exe <OWBINDIR>/langdat<CMDEXT>
    <CPCMD> <OWOBJDIR>/mkinf.exe <OWBINDIR>/mkinf<CMDEXT>
 
[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/mkexezip<CMDEXT>
    rm -f <OWBINDIR>/mkexezip<CMDEXT>
    echo rm -f <OWBINDIR>/uzip<CMDEXT>
    rm -f <OWBINDIR>/uzip<CMDEXT>
    echo rm -f <OWBINDIR>/langdat<CMDEXT>
    rm -f <OWBINDIR>/langdat<CMDEXT>
    echo rm -f <OWBINDIR>/mkinf<CMDEXT>
    rm -f <OWBINDIR>/mkinf<CMDEXT>
 
[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
