# WRES Builder Control file
# =========================

set PROJDIR=<CWD>
set PROJNAME=wres library

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/deflib.ctl ]

[ BLOCK <1> rel cprel ]
#======================
# NT, OS2 32-bit version
    <CCCMD> flat386/mf_r/wres.lib   <OWRELROOT>/lib386/wresf.lib
    <CCCMD> flat386/mf_s/wres.lib   <OWRELROOT>/lib386/wresfs.lib
# OSI 32-bit version
#    <CCCMD> small386/ms_r/wres.lib  <OWRELROOT>/lib386/osi/wresf.lib
#    <CCCMD> small386/ms_s/wres.lib  <OWRELROOT>/lib386/osi/wresfs.lib
# DOS 32-bit version
    <CCCMD> small386/ms_r/wres.lib  <OWRELROOT>/lib386/dos/wresf.lib
    <CCCMD> small386/ms_s/wres.lib  <OWRELROOT>/lib386/dos/wresfs.lib
# AXP version
#    <CCCMD> ntaxp/_s/wres.lib       <OWRELROOT>/libaxp/
# LINUX version
    <CCCMD> linux386/mf_r/wres.lib  <OWRELROOT>/lib386/linux/wresf.lib
    <CCCMD> linux386/mf_s/wres.lib  <OWRELROOT>/lib386/linux/wresfs.lib
# QNX version
#    <CCCMD> qnx386/ms_r/wres.lib    <OWRELROOT>/lib386/qnx/wresf.lib
#    <CCCMD> qnx386/ms_s/wres.lib    <OWRELROOT>/lib386/qnx/wresfs.lib

# DOS 16-bit version
#    <CCCMD> dosi86/mm/wres.lib      <OWRELROOT>/lib286/wresm.lib
#    <CCCMD> dosi86/ml/wres.lib      <OWRELROOT>/lib286/wresl.lib
#    <CCCMD> dosi86/ms/wres.lib      <OWRELROOT>/lib286/wress.lib
#    <CCCMD> dosi86/mc/wres.lib      <OWRELROOT>/lib286/wresc.lib
#    <CCCMD> dosi86/mh/wres.lib      <OWRELROOT>/lib286/wresh.lib
# OS2 16-bit version
#    <CCCMD> os2i86/ml/wres.lib      <OWRELROOT>/lib286/os2/wresl.lib

[ BLOCK . . ]
#============
cdsay <PROJDIR>
