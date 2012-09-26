# setupgui Builder Control file
# =============================
 
set PROJDIR=<CWD>
 
[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]
 
[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> build rel ]
    cdsay <PROJDIR>

[ BLOCK <1> build rel cprel ]
#============================
    <CPCMD> mkdisk/mkexezip.exe <OWBINDIR>/mkexezip<CMDEXT>
    <CPCMD> mkdisk/uzip.exe <OWBINDIR>/uzip<CMDEXT>
    <CPCMD> mkdisk/langdat.exe <OWBINDIR>/langdat<CMDEXT>
    <CPCMD> mkdisk/mkinf.exe <OWBINDIR>/mkinf<CMDEXT>
 
[ BLOCK <1> clean ]
#==================
    rm -f <OWBINDIR>/mkexezip<CMDEXT>
    rm -f <OWBINDIR>/uzip<CMDEXT>
    rm -f <OWBINDIR>/langdat<CMDEXT>
    rm -f <OWBINDIR>/mkinf<CMDEXT>
 
[ BLOCK . . ]
#============
cdsay <PROJDIR>
