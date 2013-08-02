# IDE configuration Builder Control file
# ======================================

set PROJDIR=<CWD>
set PROJNAME=ide cfg

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CPCMD> ide.cfg           <OWRELROOT>/binw/
    <CPCMD> idedos.cfg        <OWRELROOT>/binw/
    <CPCMD> idedos32.cfg      <OWRELROOT>/binw/
    <CPCMD> idewin.cfg        <OWRELROOT>/binw/
    <CPCMD> ideos2.cfg        <OWRELROOT>/binw/
    <CPCMD> ideos232.cfg      <OWRELROOT>/binw/
    <CPCMD> idew32.cfg        <OWRELROOT>/binw/
    <CPCMD> idew386.cfg       <OWRELROOT>/binw/
#    <CPCMD> ideads.cfg        <OWRELROOT>/binw/
    <CPCMD> idenlm.cfg        <OWRELROOT>/binw/
#    <CPCMD> idemfc16.cfg      <OWRELROOT>/binw/
#    <CPCMD> idemfc32.cfg      <OWRELROOT>/binw/
#    <CPCMD> idemfca.cfg       <OWRELROOT>/binw/
    <CPCMD> ideaxp.cfg        <OWRELROOT>/binw/
    <CPCMD> idelnx.cfg        <OWRELROOT>/binw/
    <CPCMD> iderdos.cfg       <OWRELROOT>/binw/

    <CCCMD> ide.cfg           <OWRELROOT>/binl/
    <CCCMD> idedos.cfg        <OWRELROOT>/binl/
    <CCCMD> idedos32.cfg      <OWRELROOT>/binl/
    <CCCMD> idewin.cfg        <OWRELROOT>/binl/
    <CCCMD> ideos2.cfg        <OWRELROOT>/binl/
    <CCCMD> ideos232.cfg      <OWRELROOT>/binl/
    <CCCMD> idew32.cfg        <OWRELROOT>/binl/
    <CCCMD> idew386.cfg       <OWRELROOT>/binl/
    <CCCMD> idenlm.cfg        <OWRELROOT>/binl/
    <CPCMD> ideaxp.cfg        <OWRELROOT>/binl/
    <CCCMD> idelnx.cfg        <OWRELROOT>/binl/
    <CCCMD> iderdos.cfg       <OWRELROOT>/binl/

    <CCCMD> wini86/*.cfg      <OWRELROOT>/binw/
    <CCCMD> os2/*.cfg         <OWRELROOT>/binp/
    <CCCMD> nt386/*.cfg       <OWRELROOT>/binnt/
    <CCCMD> linux386/*.cfg    <OWRELROOT>/binl/

    <CCCMD> ntx64/*.cfg       <OWRELROOT>/binnt64/

    <CCCMD> ntaxp/*.cfg       <OWRELROOT>/axpnt/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
