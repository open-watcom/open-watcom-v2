# Readme Builder Control file
# ==========================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ BLOCK <1> rel2 cprel2 ]
#========================
cdsay <PROJDIR>
<CPCMD> readme.110 <relroot>\rel2\readme.txt
<CPCMD> freadme.110 <relroot>\rel2\freadme.txt
<CPCMD> jreadme.110 <relroot>\rel2\jreadme.txt
<CPCMD> jfreadme.110 <relroot>\rel2\jfreadme.txt
