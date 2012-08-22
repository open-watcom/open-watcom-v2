# WRES Builder Control file
# =========================

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

[ BLOCK <1> rel2 cprel2 ]
#========================

# NT, OS2 32-bit version
    <CCCMD> flat386/mf_r/wres.lib  <RELROOT>/lib386/wresf.lib
    <CCCMD> flat386/mf_s/wres.lib  <RELROOT>/lib386/wresfs.lib
# OSI 32-bit version
#    <CCCMD> small386/ms_r/wres.lib <RELROOT>/lib386/osi/wresf.lib
#    <CCCMD> small386/ms_s/wres.lib <RELROOT>/lib386/osi/wresfs.lib
# DOS 32-bit version
    <CCCMD> small386/ms_r/wres.lib <RELROOT>/lib386/dos/wresf.lib
    <CCCMD> small386/ms_s/wres.lib <RELROOT>/lib386/dos/wresfs.lib
# AXP version
#    <CCCMD> ntaxp/_s/wres.lib   <RELROOT>/libaxp/
# LINUX version
    <CCCMD> linux386/mf_r/wres.lib <RELROOT>/lib386/linux/wresf.lib
    <CCCMD> linux386/mf_s/wres.lib <RELROOT>/lib386/linux/wresfs.lib
# QNX version
#    <CCCMD> qnx386/ms_r/wres.lib <RELROOT>/lib386/qnx/wresf.lib
#    <CCCMD> qnx386/ms_s/wres.lib <RELROOT>/lib386/qnx/wresfs.lib

# DOS 16-bit version
#    <CCCMD> dosi86/mm/wres.lib    <RELROOT>/lib286/wresm.lib
#    <CCCMD> dosi86/ml/wres.lib    <RELROOT>/lib286/wresl.lib
#    <CCCMD> dosi86/ms/wres.lib   <RELROOT>/lib286/wress.lib
#    <CCCMD> dosi86/mc/wres.lib   <RELROOT>/lib286/wresc.lib
#    <CCCMD> dosi86/mh/wres.lib   <RELROOT>/lib286/wresh.lib
# OS2 16-bit version
#    <CCCMD> os2i86/ml/wres.lib    <RELROOT>/lib286/os2/wresl.lib

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
