# mkdisk Builder Control file
# ===========================
 
set PROJNAME=mkdisk

set BINTOOL=0

set PROJDIR=<CWD>
 
[ INCLUDE "<OWROOT>/build/prolog.ctl" ]
 
[ INCLUDE "<OWROOT>/build/defrule.ctl" ]

[ BLOCK <BINTOOL> build ]
#========================
    cdsay "<PROJDIR>"
    <CPCMD> <OWOBJDIR>/mkexezip.exe "<OWROOT>/build/<OWOBJDIR>/mkexezip<CMDEXT>"
    <CPCMD> <OWOBJDIR>/uzip.exe "<OWROOT>/build/<OWOBJDIR>/uzip<CMDEXT>"
    <CPCMD> <OWOBJDIR>/langdat.exe "<OWROOT>/build/<OWOBJDIR>/langdat<CMDEXT>"
    <CPCMD> <OWOBJDIR>/mkinf.exe "<OWROOT>/build/<OWOBJDIR>/mkinf<CMDEXT>"
 
[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/mkexezip<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/mkexezip<CMDEXT>"
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/uzip<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/uzip<CMDEXT>"
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/langdat<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/langdat<CMDEXT>"
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/mkinf<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/mkinf<CMDEXT>"
 
[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
