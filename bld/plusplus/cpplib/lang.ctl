# plib Builder Control file
# =========================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .
set destdir=<relroot>\rel2

[ BLOCK <1> build rel2 ]
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 ]
    set destdir=<relroot>\rel2

[ BLOCK <1> nlang ]
    set destdir=<dwatcom>
    set CPCMD=copy

[ BLOCK <1> rel2 cprel2 acprel2 cpu_i86 nlang ]
#==============================================
    <CPCMD> scsd\plibs.lib <destdir>\lib286\
    <CPCMD> scbd\plibc.lib <destdir>\lib286\
    <CPCMD> bcsd\plibm.lib <destdir>\lib286\
    <CPCMD> bcbd\plibl.lib <destdir>\lib286\
    <CPCMD> bchd\plibh.lib <destdir>\lib286\
    <CPCMD> wscsd\plibs.lib <destdir>\lib286\win\
    <CPCMD> wscbd\plibc.lib <destdir>\lib286\win\
    <CPCMD> wbcsd\plibm.lib <destdir>\lib286\win\
    <CPCMD> wbcbd\plibl.lib <destdir>\lib286\win\
    <CPCMD> os2mt\plibmtl.lib <destdir>\lib286\os2\
    <CPCMD> scsd\plibs.lib <destdir>\lib286\
    <CPCMD> scbd\plibc.lib <destdir>\lib286\
    <CPCMD> bcsd\plibm.lib <destdir>\lib286\
    <CPCMD> bcbd\plibl.lib <destdir>\lib286\
    <CPCMD> bchd\plibh.lib <destdir>\lib286\
    <CPCMD> wscsd\plibs.lib <destdir>\lib286\win\
    <CPCMD> wscbd\plibc.lib <destdir>\lib286\win\
    <CPCMD> wbcsd\plibm.lib <destdir>\lib286\win\
    <CPCMD> wbcbd\plibl.lib <destdir>\lib286\win\
    <CPCMD> os2mt\plibmtl.lib <destdir>\lib286\os2\
    <CPCMD> scsd\plbxs.lib <destdir>\lib286\
    <CPCMD> scbd\plbxc.lib <destdir>\lib286\
    <CPCMD> bcsd\plbxm.lib <destdir>\lib286\
    <CPCMD> bcbd\plbxl.lib <destdir>\lib286\
    <CPCMD> bchd\plbxh.lib <destdir>\lib286\
    <CPCMD> wscsd\plbxs.lib <destdir>\lib286\win\
    <CPCMD> wscbd\plbxc.lib <destdir>\lib286\win\
    <CPCMD> wbcsd\plbxm.lib <destdir>\lib286\win\
    <CPCMD> wbcbd\plbxl.lib <destdir>\lib286\win\
    <CPCMD> os2mt\plbxmtl.lib <destdir>\lib286\os2\
    <CPCMD> scsd\plbxs.lib <destdir>\lib286\
    <CPCMD> scbd\plbxc.lib <destdir>\lib286\
    <CPCMD> bcsd\plbxm.lib <destdir>\lib286\
    <CPCMD> bcbd\plbxl.lib <destdir>\lib286\
    <CPCMD> bchd\plbxh.lib <destdir>\lib286\
    <CPCMD> wscsd\plbxs.lib <destdir>\lib286\win\
    <CPCMD> wscbd\plbxc.lib <destdir>\lib286\win\
    <CPCMD> wbcsd\plbxm.lib <destdir>\lib286\win\
    <CPCMD> wbcbd\plbxl.lib <destdir>\lib286\win\
    <CPCMD> os2mt\plbxmtl.lib <destdir>\lib286\os2\
    <CPCMD> complex\scsd\cplxs.lib <destdir>\lib286\
    <CPCMD> complex\scbd\cplxc.lib <destdir>\lib286\
    <CPCMD> complex\bcsd\cplxm.lib <destdir>\lib286\
    <CPCMD> complex\bcbd\cplxl.lib <destdir>\lib286\
    <CPCMD> complex\bchd\cplxh.lib <destdir>\lib286\
    <CPCMD> complex\wscsd\cplxs.lib <destdir>\lib286\win\
    <CPCMD> complex\wscbd\cplxc.lib <destdir>\lib286\win\
    <CPCMD> complex\wbcsd\cplxm.lib <destdir>\lib286\win\
    <CPCMD> complex\wbcbd\cplxl.lib <destdir>\lib286\win\
    <CPCMD> complex\scsd7\cplx7s.lib <destdir>\lib286\
    <CPCMD> complex\scbd7\cplx7c.lib <destdir>\lib286\
    <CPCMD> complex\bcsd7\cplx7m.lib <destdir>\lib286\
    <CPCMD> complex\bcbd7\cplx7l.lib <destdir>\lib286\
    <CPCMD> complex\bchd7\cplx7h.lib <destdir>\lib286\
    <CPCMD> complex\wscsd7\cplx7s.lib <destdir>\lib286\win\
    <CPCMD> complex\wscbd7\cplx7c.lib <destdir>\lib286\win\
    <CPCMD> complex\wbcsd7\cplx7m.lib <destdir>\lib286\win\
    <CPCMD> complex\wbcbd7\cplx7l.lib <destdir>\lib286\win\

[ BLOCK <1> rel2 cprel2 acprel2 cpu_386 ]
#========================================
    <CPCMD> 3r\plib3r.lib <destdir>\lib386\
    <CPCMD> 3s\plib3s.lib <destdir>\lib386\
    <CPCMD> 3rmt\plibmt3r.lib <destdir>\lib386\
    <CPCMD> 3smt\plibmt3s.lib <destdir>\lib386\
    <CPCMD> 3r\plbx3r.lib <destdir>\lib386\
    <CPCMD> 3s\plbx3s.lib <destdir>\lib386\
    <CPCMD> 3rmt\plbxmt3r.lib <destdir>\lib386\
    <CPCMD> 3smt\plbxmt3s.lib <destdir>\lib386\
    <CPCMD> nt3r\plbx3r.lib <destdir>\lib386\nt\
    <CPCMD> nt3r\plib3r.lib <destdir>\lib386\nt\
    <CPCMD> nt3s\plbx3s.lib <destdir>\lib386\nt\
    <CPCMD> nt3s\plib3s.lib <destdir>\lib386\nt\
    <CPCMD> nt3rmt\plbxmt3r.lib <destdir>\lib386\nt\
    <CPCMD> nt3rmt\plibmt3r.lib <destdir>\lib386\nt\
    <CPCMD> nt3smt\plbxmt3s.lib <destdir>\lib386\nt\
    <CPCMD> nt3smt\plibmt3s.lib <destdir>\lib386\nt\
    <CPCMD> q3rmt\plbxmt3r.lib <destdir>\lib386\qnx\
    <CPCMD> q3rmt\plibmt3r.lib <destdir>\lib386\qnx\
    <CPCMD> q3smt\plbxmt3s.lib <destdir>\lib386\qnx\
    <CPCMD> q3smt\plibmt3s.lib <destdir>\lib386\qnx\
    <CPCMD> os23r\plbx3r.lib <destdir>\lib386\os2\
    <CPCMD> os23r\plib3r.lib <destdir>\lib386\os2\
    <CPCMD> os23s\plbx3s.lib <destdir>\lib386\os2\
    <CPCMD> os23s\plib3s.lib <destdir>\lib386\os2\
    <CPCMD> os23rmt\plbxmt3r.lib <destdir>\lib386\os2\
    <CPCMD> os23rmt\plibmt3r.lib <destdir>\lib386\os2\
    <CPCMD> os23smt\plbxmt3s.lib <destdir>\lib386\os2\
    <CPCMD> os23smt\plibmt3s.lib <destdir>\lib386\os2\
    <CPCMD> complex\3r\cplx3r.lib <destdir>\lib386\
    <CPCMD> complex\3s\cplx3s.lib <destdir>\lib386\
    <CPCMD> complex\73r\cplx73r.lib <destdir>\lib386\
    <CPCMD> complex\73s\cplx73s.lib <destdir>\lib386\

#[ BLOCK <1> rel2 cprel2 acprel2 cpu_axp ]
##========================================
#    <CPCMD> ntaxp\plibnt.lib <destdir>\libaxp\nt\plib.lib
#    <CPCMD> ntaxp\plbxnt.lib <destdir>\libaxp\nt\plbx.lib
#    <CPCMD> ntaxpmt\plibmt.lib <destdir>\libaxp\nt\plibmt.lib
#    <CPCMD> ntaxpmt\plbxmt.lib <destdir>\libaxp\nt\plbxmt.lib
#    <CPCMD> complex\ntaxp\cplxnt.lib <destdir>\libaxp\cplx.lib

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean
