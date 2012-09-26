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
    <CPCMD> ide.cfg          <OWRELROOT>/binw/
    <CPCMD> idedos.cfg       <OWRELROOT>/binw/
    <CPCMD> idedos32.cfg     <OWRELROOT>/binw/
    <CPCMD> idewin.cfg       <OWRELROOT>/binw/
    <CPCMD> ideos2.cfg       <OWRELROOT>/binw/
    <CPCMD> ideos232.cfg     <OWRELROOT>/binw/
    <CPCMD> idew32.cfg       <OWRELROOT>/binw/
    <CPCMD> idew386.cfg      <OWRELROOT>/binw/
#    <CPCMD> ideads.cfg       <OWRELROOT>/binw/
    <CPCMD> idenlm.cfg       <OWRELROOT>/binw/
#    <CPCMD> idemfc16.cfg     <OWRELROOT>/binw/
#    <CPCMD> idemfc32.cfg     <OWRELROOT>/binw/
#    <CPCMD> idemfca.cfg      <OWRELROOT>/binw/
    <CPCMD> ideaxp.cfg       <OWRELROOT>/binw/
    <CPCMD> idelnx.cfg       <OWRELROOT>/binw/
    <CPCMD> iderdos.cfg       <OWRELROOT>/binw/

  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> wini86/idex.cfg  <OWRELROOT>/binw/

  [ IFDEF (os_win "") <2*> ]
    <CPCMD> wini86/idex.cfg  <OWRELROOT>/binw/

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> os2386/idex.cfg  <OWRELROOT>/binp/

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/idex.cfg   <OWRELROOT>/binnt/

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> ide.cfg          <OWRELROOT>/binl/
    <CPCMD> linux386/idex.cfg <OWRELROOT>/binl/
    <CPCMD> idedos.cfg       <OWRELROOT>/binl/
    <CPCMD> idedos32.cfg     <OWRELROOT>/binl/
    <CPCMD> idewin.cfg       <OWRELROOT>/binl/
    <CPCMD> ideos2.cfg       <OWRELROOT>/binl/
    <CPCMD> ideos232.cfg     <OWRELROOT>/binl/
    <CPCMD> idew32.cfg       <OWRELROOT>/binl/
    <CPCMD> idew386.cfg      <OWRELROOT>/binl/
    <CPCMD> idenlm.cfg       <OWRELROOT>/binl/
    <CPCMD> idelnx.cfg       <OWRELROOT>/binl/
    <CPCMD> iderdos.cfg      <OWRELROOT>/binl/

  [ IFDEF (cpu_axp) <2*> ] 
    <CPCMD> ntaxp/idex.cfg   <OWRELROOT>/axpnt/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
