# w32api Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=w32api

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

cdsay .

[ BLOCK <1> rel cprel ]
#======================
    <CPCMD> readme.txt                    <OWRELROOT>/readme.w32

    <CPCMD> nt/*.h                        <OWRELROOT>/h/nt/
    <CPCMD> nt/*.rh                       <OWRELROOT>/h/nt/
    <CPCMD> nt/GL/*.h                     <OWRELROOT>/h/nt/GL/
    <CPCMD> nt/ddk/*.h                    <OWRELROOT>/h/nt/ddk/
    <CPCMD> nt/directx/*.h                <OWRELROOT>/h/nt/directx/

    <CPCMD> nt386/*.lib                   <OWRELROOT>/lib386/nt/
    <CPCMD> nt386/ddk/*.lib               <OWRELROOT>/lib386/nt/ddk/
    <CPCMD> nt386/directx/*.lib           <OWRELROOT>/lib386/nt/directx/

#    <CPCMD> ntaxp/*.lib                   <OWRELROOT>/libaxp/nt/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
