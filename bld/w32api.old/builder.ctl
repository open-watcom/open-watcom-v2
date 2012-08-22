# w32api Builder Control file
# ===========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
#=======================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> rel2 cprel2 acprel2 ]
#================================
#NT stuff
    <CPCMD> README.w32api                 <RELROOT>/readme.w32
  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt/include/*.h                <RELROOT>/h/nt/
    <CPCMD> nt/include/GL/*.h             <RELROOT>/h/nt/GL/
    <CPCMD> nt/include/ddk/*.h            <RELROOT>/h/nt/ddk/
    <CPCMD> nt/include/directx/*.h        <RELROOT>/h/nt/directx/

    <CPCMD> nt386/base/*.lib              <RELROOT>/lib386/nt/
    <CPCMD> nt386/ddk/*.lib               <RELROOT>/lib386/nt/ddk/
    <CPCMD> nt386/directx/*.lib           <RELROOT>/lib386/nt/directx/

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> ntaxp/base/*.lib              <RELROOT>/libaxp/nt/

[ BLOCK <1> clean ]
#==================
    pmake -d all <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
