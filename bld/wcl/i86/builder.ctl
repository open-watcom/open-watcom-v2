# wcl Builder Control file
# ========================

set PROJDIR=<CWD>
set PROJNAME=wcl

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> boot ]
#=================
    <CPCMD> <OWOBJDIR>/bwcl.exe <OWBINDIR>/bwcl<CMDEXT>

[ BLOCK <1> bootclean ]
#======================
    echo rm -f <OWBINDIR>/bwcl<CMDEXT>
    rm -f <OWBINDIR>/bwcl<CMDEXT>

[ BLOCK <1> rel ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> <PROJDIR>/dosi86/wcl.exe        <OWRELROOT>/binw/wcl.exe
    <CCCMD> <PROJDIR>/dosi86/wcl.sym        <OWRELROOT>/binw/wcl.sym
    <CCCMD> <PROJDIR>/nt386/wcl.exe         <OWRELROOT>/binnt/wcl.exe
    <CCCMD> <PROJDIR>/nt386/wcl.sym         <OWRELROOT>/binnt/wcl.sym
    <CCCMD> <PROJDIR>/os2386/wcl.exe        <OWRELROOT>/binp/wcl.exe
    <CCCMD> <PROJDIR>/os2386/wcl.sym        <OWRELROOT>/binp/wcl.sym
    <CCCMD> <PROJDIR>/linux386/wcl.exe      <OWRELROOT>/binl/wcl
    <CCCMD> <PROJDIR>/linux386/wcl.sym      <OWRELROOT>/binl/wcl.sym
    <CCCMD> <PROJDIR>/ntaxp/wcl.exe         <OWRELROOT>/axpnt/wcl.exe
    <CCCMD> <PROJDIR>/ntaxp/wcl.sym         <OWRELROOT>/axpnt/wcl.sym

[ BLOCK . . ]
#============
cdsay <PROJDIR>
