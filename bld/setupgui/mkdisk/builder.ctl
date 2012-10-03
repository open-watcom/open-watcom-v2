# mkdisk Builder Control file
# ===========================
 
set PROJDIR=<CWD>
set PROJNAME=mkdisk
 
[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]
 
[ INCLUDE <OWROOT>/build/binrule.ctl ]

[ BLOCK <BINTOOL> rel ]
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> rel ]
#======================
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
#============
cdsay <PROJDIR>
