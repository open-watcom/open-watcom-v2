# ONLINE Builder Control file
# ===========================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> rel2 cprel2 acprel2 ]
#================================
    <CPCMD> <PROJDIR>\hlp\win\*.hlp 	<relroot>\rel2\binw\
    <CPCMD> <PROJDIR>\hlp\win95\*.hlp 	<relroot>\rel2\binnt\
    <CPCMD> <PROJDIR>\hlp\win95\*.cnt 	<relroot>\rel2\binnt\
    <CPCMD> <PROJDIR>\hlp\os2\*.inf 	<relroot>\rel2\binp\help\
    <CPCMD> <PROJDIR>\hlp\os2\*.hlp 	<relroot>\rel2\binp\help\
    <CPCMD> <PROJDIR>\hlp\ib\*.ihp 	<relroot>\rel2\binw\
# Japanese Stuff 
#    <CPCMD> <PROJDIR>\japan\hlp\*.hlp 	<relroot>\rel2\binw\
#    <CPCMD> <PROJDIR>\japan\hlp32\*.hlp <relroot>\rel2\binnt\
#    <CPCMD> <PROJDIR>\japan\hlp32\*.cnt <relroot>\rel2\binnt\
