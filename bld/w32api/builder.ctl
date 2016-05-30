# w32api Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=w32api

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> readme.txt                    <OWRELROOT>/readme.w32

    <CCCMD> nt/*.h                        <OWRELROOT>/h/nt/
    <CCCMD> nt/*.rh                       <OWRELROOT>/h/nt/
    <CCCMD> nt/GL/*.h                     <OWRELROOT>/h/nt/GL/
    <CCCMD> nt/ddk/*.h                    <OWRELROOT>/h/nt/ddk/
    <CCCMD> nt/directx/*.h                <OWRELROOT>/h/nt/directx/

    <CCCMD> nt386/*.lib                   <OWRELROOT>/lib386/nt/
    <CCCMD> nt386/ddk/*.lib               <OWRELROOT>/lib386/nt/ddk/
    <CCCMD> nt386/directx/*.lib           <OWRELROOT>/lib386/nt/directx/

#    <CCCMD> ntaxp/*.lib                   <OWRELROOT>/libaxp/nt/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
