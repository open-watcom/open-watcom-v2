# setupgui Builder Control file
# =============================
 
set PROJDIR=<CWD>
 
[ INCLUDE <OWROOT>/bat/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]
 
cdsay .
 
[ BLOCK <1> build rel2 ]
#=======================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h
 
[ BLOCK <1> rel2 ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> build rel2 cprel2 ]
#==============================
    <CPCMD> <PROJDIR>/mkdisk/mkexezip.exe <OWBINDIR>/mkexezip<CMDEXT>
    <CPCMD> <PROJDIR>/mkdisk/uzip.exe <OWBINDIR>/uzip<CMDEXT>
    <CPCMD> <PROJDIR>/mkdisk/langdat.exe <OWBINDIR>/langdat<CMDEXT>
    <CPCMD> <PROJDIR>/mkdisk/mkinf.exe <OWBINDIR>/mkinf<CMDEXT>
 
[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean
    rm -f <OWBINDIR>/mkexezip<CMDEXT>
    rm -f <OWBINDIR>/uzip<CMDEXT>
    rm -f <OWBINDIR>/langdat<CMDEXT>
    rm -f <OWBINDIR>/mkinf<CMDEXT>
 
[ BLOCK . . ]
#============
 
cdsay <PROJDIR>
