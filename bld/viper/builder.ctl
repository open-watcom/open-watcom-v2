# VIPER Builder Control file
# ==========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel cprel ]
#======================
    <CPCMD> viper.doc            <OWRELROOT>/ide.doc
    <CPCMD> cfg/ide.cfg          <OWRELROOT>/binw/
    <CPCMD> cfg/idedos.cfg       <OWRELROOT>/binw/
    <CPCMD> cfg/idedos32.cfg     <OWRELROOT>/binw/
    <CPCMD> cfg/idewin.cfg       <OWRELROOT>/binw/
    <CPCMD> cfg/ideos2.cfg       <OWRELROOT>/binw/
    <CPCMD> cfg/ideos232.cfg     <OWRELROOT>/binw/
    <CPCMD> cfg/idew32.cfg       <OWRELROOT>/binw/
    <CPCMD> cfg/idew386.cfg      <OWRELROOT>/binw/
#    <CPCMD> cfg/ideads.cfg       <OWRELROOT>/binw/
    <CPCMD> cfg/idenlm.cfg       <OWRELROOT>/binw/
#    <CPCMD> cfg/idemfc16.cfg     <OWRELROOT>/binw/
#    <CPCMD> cfg/idemfc32.cfg     <OWRELROOT>/binw/
#    <CPCMD> cfg/idemfca.cfg      <OWRELROOT>/binw/
    <CPCMD> cfg/ideaxp.cfg       <OWRELROOT>/binw/
    <CPCMD> cfg/idelnx.cfg       <OWRELROOT>/binw/
    <CPCMD> cfg/iderdos.cfg       <OWRELROOT>/binw/

  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> bviper/dos386/ide2make.exe <OWRELROOT>/binw/
    <CPCMD> cfg/wini86/idex.cfg  <OWRELROOT>/binw/

  [ IFDEF (os_win "") <2*> ]
    <CPCMD> viper/wini86/ide.exe <OWRELROOT>/binw/
    <CPCMD> cfg/wini86/idex.cfg  <OWRELROOT>/binw/
    <CPCMD> viper/win/wsrv.pif   <OWRELROOT>/binw/
    <CPCMD> viper/win/wd.pif     <OWRELROOT>/binw/

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> viper/os2386/ide.exe <OWRELROOT>/binp/
    <CPCMD> cfg/os2386/idex.cfg  <OWRELROOT>/binp/
    <CPCMD> bviper/os2386/ide2make.exe <OWRELROOT>/binp/

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> viper/nt386/ide.exe  <OWRELROOT>/binnt/
    <CPCMD> cfg/nt386/idex.cfg   <OWRELROOT>/binnt/
    <CPCMD> bviper/nt386/ide2make.exe <OWRELROOT>/binnt/

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> bviper/linux386/ide2make.exe <OWRELROOT>/binl/ide2make
    <CPCMD> cfg/ide.cfg          <OWRELROOT>/binl/
    <CPCMD> cfg/linux386/idex.cfg <OWRELROOT>/binl/
    <CPCMD> cfg/idedos.cfg       <OWRELROOT>/binl/
    <CPCMD> cfg/idedos32.cfg     <OWRELROOT>/binl/
    <CPCMD> cfg/idewin.cfg       <OWRELROOT>/binl/
    <CPCMD> cfg/ideos2.cfg       <OWRELROOT>/binl/
    <CPCMD> cfg/ideos232.cfg     <OWRELROOT>/binl/
    <CPCMD> cfg/idew32.cfg       <OWRELROOT>/binl/
    <CPCMD> cfg/idew386.cfg      <OWRELROOT>/binl/
    <CPCMD> cfg/idenlm.cfg       <OWRELROOT>/binl/
    <CPCMD> cfg/idelnx.cfg       <OWRELROOT>/binl/
    <CPCMD> cfg/iderdos.cfg      <OWRELROOT>/binl/

  [ IFDEF (cpu_axp) <2*> ] 
    <CPCMD> viper/ntaxp/ide.exe  <OWRELROOT>/axpnt/
    <CPCMD> cfg/ntaxp/idex.cfg   <OWRELROOT>/axpnt/
    <CPCMD> bviper/ntaxp/ide2make.exe <OWRELROOT>/axpnt/

[ BLOCK . . ]

cdsay <PROJDIR>/bviper

[ INCLUDE prereq.ctl ]

[ BLOCK . . ]
#============
cdsay <PROJDIR>
