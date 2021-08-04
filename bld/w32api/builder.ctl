# w32api Builder Control file
# ===========================

set PROJNAME=w32api

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> readme.txt                    <OWRELROOT>/readme.w32

    <CCCMD> nt/h/*                        <OWRELROOT>/h/nt/
    <CCCMD> nt/h/GL/*                     <OWRELROOT>/h/nt/GL/
    <CCCMD> nt/h/ddk/*                    <OWRELROOT>/h/nt/ddk/
    <CCCMD> nt/h/directx/*                <OWRELROOT>/h/nt/directx/

    <CCCMD> nt386/lib/*                   <OWRELROOT>/lib386/nt/
    <CCCMD> nt386/lib/ddk/*               <OWRELROOT>/lib386/nt/ddk/
    <CCCMD> nt386/lib/directx/*           <OWRELROOT>/lib386/nt/directx/

#    <CCCMD> ntaxp/lib/*                   <OWRELROOT>/libaxp/nt/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
