# VIPER Builder Control file
# ==========================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 cprel2 ]
#========================
    <CPCMD> getviper.bat <relroot>\rel2\getviper.bat
    <CPCMD> viper.doc <relroot>\rel2\ide.doc
    <CPCMD> viper\ide.cfg <relroot>\rel2\binw\ide.cfg
    <CPCMD> viper\idedos.cfg <relroot>\rel2\binw\idedos.cfg
    <CPCMD> viper\idedos32.cfg <relroot>\rel2\binw\idedos32.cfg
    <CPCMD> viper\idewin.cfg <relroot>\rel2\binw\idewin.cfg
    <CPCMD> viper\ideos2.cfg <relroot>\rel2\binw\ideos2.cfg
    <CPCMD> viper\ideos232.cfg <relroot>\rel2\binw\ideos232.cfg
    <CPCMD> viper\idew32.cfg <relroot>\rel2\binw\idew32.cfg
    <CPCMD> viper\idew386.cfg <relroot>\rel2\binw\idew386.cfg
    <CPCMD> viper\ideads.cfg <relroot>\rel2\binw\ideads.cfg
    <CPCMD> viper\idenlm.cfg <relroot>\rel2\binw\idenlm.cfg
    <CPCMD> viper\idemfc16.cfg <relroot>\rel2\binw\idemfc16.cfg
    <CPCMD> viper\idemfc32.cfg <relroot>\rel2\binw\idemfc32.cfg
    <CPCMD> viper\idemfca.cfg <relroot>\rel2\binw\idemfca.cfg
#    <CPCMD> viper\ideaxp.cfg <relroot>\rel2\binw\ideaxp.cfg
    <CPCMD> viper\win\ide.exe <relroot>\rel2\binw\ide.exe
    <CPCMD> viper\win\idex.cfg <relroot>\rel2\binw\idex.cfg
    <CPCMD> viper\win\wsrv.pif <relroot>\rel2\binw\wsrv.pif
    <CPCMD> viper\win\wd.pif <relroot>\rel2\binw\wd.pif
    <CPCMD> viper\os2\ide.exe <relroot>\rel2\binp\ide.exe
    <CPCMD> viper\os2\idex.cfg <relroot>\rel2\binp\idex.cfg
    <CPCMD> viper\nt\ide.exe <relroot>\rel2\binnt\ide.exe
    <CPCMD> viper\nt\idex.cfg <relroot>\rel2\binnt\idex.cfg
#    <CPCMD> viper\axp\ide.exe <relroot>\rel2\axpnt\ide.exe
#    <CPCMD> viper\axp\idex.cfg <relroot>\rel2\axpnt\idex.cfg
    <CPCMD> vpdll\dll\vpdll.dll <relroot>\rel2\binw\vpdll.dll
    <CPCMD> vpdll\dll.nt\vpdll.dll <relroot>\rel2\binnt\vpdll.dll

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean
