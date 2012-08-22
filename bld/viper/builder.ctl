# VIPER Builder Control file
# ==========================

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
    <CPCMD> viper.doc            <RELROOT>/ide.doc
    <CPCMD> cfg/ide.cfg          <RELROOT>/binw/
    <CPCMD> cfg/idedos.cfg       <RELROOT>/binw/
    <CPCMD> cfg/idedos32.cfg     <RELROOT>/binw/
    <CPCMD> cfg/idewin.cfg       <RELROOT>/binw/
    <CPCMD> cfg/ideos2.cfg       <RELROOT>/binw/
    <CPCMD> cfg/ideos232.cfg     <RELROOT>/binw/
    <CPCMD> cfg/idew32.cfg       <RELROOT>/binw/
    <CPCMD> cfg/idew386.cfg      <RELROOT>/binw/
#    <CPCMD> cfg/ideads.cfg       <RELROOT>/binw/
    <CPCMD> cfg/idenlm.cfg       <RELROOT>/binw/
#    <CPCMD> cfg/idemfc16.cfg     <RELROOT>/binw/
#    <CPCMD> cfg/idemfc32.cfg     <RELROOT>/binw/
#    <CPCMD> cfg/idemfca.cfg      <RELROOT>/binw/
    <CPCMD> cfg/ideaxp.cfg       <RELROOT>/binw/
    <CPCMD> cfg/idelnx.cfg       <RELROOT>/binw/
    <CPCMD> cfg/iderdos.cfg       <RELROOT>/binw/

  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> bviper/dos386/ide2make.exe <RELROOT>/binw/
    <CPCMD> cfg/wini86/idex.cfg  <RELROOT>/binw/

  [ IFDEF (os_win "") <2*> ]
    <CPCMD> viper/wini86/ide.exe <RELROOT>/binw/
    <CPCMD> cfg/wini86/idex.cfg  <RELROOT>/binw/
    <CPCMD> viper/win/wsrv.pif   <RELROOT>/binw/
    <CPCMD> viper/win/wd.pif     <RELROOT>/binw/

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> viper/os2386/ide.exe <RELROOT>/binp/
    <CPCMD> cfg/os2386/idex.cfg  <RELROOT>/binp/
    <CPCMD> bviper/os2386/ide2make.exe <RELROOT>/binp/

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> viper/nt386/ide.exe  <RELROOT>/binnt/
    <CPCMD> cfg/nt386/idex.cfg   <RELROOT>/binnt/
    <CPCMD> bviper/nt386/ide2make.exe <RELROOT>/binnt/

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> bviper/linux386/ide2make.exe <RELROOT>/binl/ide2make
    <CPCMD> cfg/ide.cfg          <RELROOT>/binl/
    <CPCMD> cfg/linux386/idex.cfg <RELROOT>/binl/
    <CPCMD> cfg/idedos.cfg       <RELROOT>/binl/
    <CPCMD> cfg/idedos32.cfg     <RELROOT>/binl/
    <CPCMD> cfg/idewin.cfg       <RELROOT>/binl/
    <CPCMD> cfg/ideos2.cfg       <RELROOT>/binl/
    <CPCMD> cfg/ideos232.cfg     <RELROOT>/binl/
    <CPCMD> cfg/idew32.cfg       <RELROOT>/binl/
    <CPCMD> cfg/idew386.cfg      <RELROOT>/binl/
    <CPCMD> cfg/idenlm.cfg       <RELROOT>/binl/
    <CPCMD> cfg/idelnx.cfg       <RELROOT>/binl/
    <CPCMD> cfg/iderdos.cfg      <RELROOT>/binl/

  [ IFDEF (cpu_axp) <2*> ] 
    <CPCMD> viper/ntaxp/ide.exe  <RELROOT>/axpnt/
    <CPCMD> cfg/ntaxp/idex.cfg   <RELROOT>/axpnt/
    <CPCMD> bviper/ntaxp/ide2make.exe <RELROOT>/axpnt/

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]

cdsay <PROJDIR>/bviper

[ INCLUDE prereq.ctl ]

[ BLOCK . . ]
#============

cdsay <PROJDIR>
