# CLIB Builder Control file
# =========================
set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
#=======================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 cprel2 acprel2 ]
#================================
    <CPCMD> library\msdos.086\mc\clibc.lib <relroot>\rel2\lib286\dos\clibc.lib
    <CPCMD> library\msdos.086\mh\clibh.lib <relroot>\rel2\lib286\dos\clibh.lib
    <CPCMD> library\msdos.086\ml\clibl.lib <relroot>\rel2\lib286\dos\clibl.lib
    <CPCMD> library\msdos.086\ml_ov\clibl.lib <relroot>\rel2\lib286\dos\clibol.lib
    <CPCMD> startup\library\msdos.086\ml\dos16m.obj <relroot>\rel2\lib286\dos\dos16m.obj
    <CPCMD> library\msdos.086\mm\clibm.lib <relroot>\rel2\lib286\dos\clibm.lib
    <CPCMD> library\msdos.086\mm_ov\clibm.lib <relroot>\rel2\lib286\dos\clibom.lib
    <CPCMD> library\msdos.086\ms\clibs.lib <relroot>\rel2\lib286\dos\clibs.lib
    <CPCMD> startup\library\msdos.086\ms\binmode.obj <relroot>\rel2\lib286\dos\binmode.obj
    <CPCMD> startup\library\msdos.086\ms\commode.obj <relroot>\rel2\lib286\dos\commode.obj
    <CPCMD> startup\library\msdos.086\ms\cstrtiny.obj <relroot>\rel2\lib286\dos\cstart_t.obj
    <CPCMD> library\msdos.286\mc\clibc.lib <relroot>\rel2\lib286\os2\dospmc.lib
    <CPCMD> library\msdos.286\mh\clibh.lib <relroot>\rel2\lib286\os2\dospmh.lib
    <CPCMD> library\msdos.286\ml\clibl.lib <relroot>\rel2\lib286\os2\dospml.lib
    <CPCMD> library\msdos.286\mm\clibm.lib <relroot>\rel2\lib286\os2\dospmm.lib
    <CPCMD> library\msdos.286\ms\clibs.lib <relroot>\rel2\lib286\os2\dospms.lib
    <CPCMD> library\msdos.386\ms_r\clibs.lib <relroot>\rel2\lib386\dos\clib3r.lib
    <CPCMD> library\msdos.386\ms_s\clibs.lib <relroot>\rel2\lib386\dos\clib3s.lib
    <CPCMD> startup\library\msdos.386\ms_r\binmode.obj <relroot>\rel2\lib386\dos\binmode.obj
    <CPCMD> startup\library\msdos.386\ms_r\commode.obj <relroot>\rel2\lib386\dos\commode.obj
    <CPCMD> startup\library\msdos.386\ms_r\cstrtx32.obj <relroot>\rel2\lib386\dos\cstrtx3r.obj
    <CPCMD> startup\library\msdos.386\ms_s\cstrtx32.obj <relroot>\rel2\lib386\dos\cstrtx3s.obj
    <CPCMD> startup\library\msdos.386\ms_s\adsstart.obj <relroot>\rel2\lib386\dos\adsstart.obj
    <CPCMD> startup\library\msdos.386\ms_s\adiestrt.obj <relroot>\rel2\lib386\dos\adiestrt.obj
    <CPCMD> startup\library\msdos.386\ms_s\adifstrt.obj <relroot>\rel2\lib386\dos\adifstrt.obj
    <CPCMD> library\netware.386\ms_s\clibs.lib <relroot>\rel2\lib386\netware\clib3s.lib
    <CPCMD> startup\library\netware.386\ms_s\binmode.obj <relroot>\rel2\lib386\netware\binmode.obj
    <CPCMD> library\os2.286\mc\clibc.lib <relroot>\rel2\lib286\os2\clibc.lib
    <CPCMD> library\os2.286\mh\clibh.lib <relroot>\rel2\lib286\os2\clibh.lib
    <CPCMD> library\os2.286\ml\clibl.lib <relroot>\rel2\lib286\os2\clibl.lib
    <CPCMD> library\os2.286\ml_dll\clibl.lib <relroot>\rel2\lib286\os2\clibdll.lib
    <CPCMD> library\os2.286\ml_mt\clibl.lib <relroot>\rel2\lib286\os2\clibmtl.lib
    <CPCMD> library\os2.286\mm\clibm.lib <relroot>\rel2\lib286\os2\clibm.lib
    <CPCMD> library\os2.286\ms\clibs.lib <relroot>\rel2\lib286\os2\clibs.lib
    <CPCMD> startup\library\os2.286\ms\binmode.obj <relroot>\rel2\lib286\os2\binmode.obj
    <CPCMD> startup\library\os2.286\ms\commode.obj <relroot>\rel2\lib286\os2\commode.obj
    <CPCMD> library\os2.386\ms_r\clibs.lib <relroot>\rel2\lib386\os2\clib3r.lib
    <CPCMD> library\os2.386\ms_s\clibs.lib <relroot>\rel2\lib386\os2\clib3s.lib
    <CPCMD> startup\library\os2.386\ms_r\binmode.obj <relroot>\rel2\lib386\os2\binmode.obj
    <CPCMD> startup\library\os2.386\ms_r\commode.obj <relroot>\rel2\lib386\os2\commode.obj
#    <CPCMD> library\osi.386\ms_r\clibs.lib <relroot>\rel2\lib386\osi\clib3r.lib
#    <CPCMD> startup\library\osi.386\ms_r\binmode.obj <relroot>\rel2\lib386\osi\binmode.obj
    <CPCMD> library\qnx.286\mc\clibc.lib <relroot>\rel2\lib286\qnx\clibc.lib
    <CPCMD> library\qnx.286\mh\clibh.lib <relroot>\rel2\lib286\qnx\clibh.lib
    <CPCMD> library\qnx.286\ml\clibl.lib <relroot>\rel2\lib286\qnx\clibl.lib
    <CPCMD> library\qnx.286\ml_sh\clibl.lib <relroot>\rel2\lib286\qnx\clibshar.lib
    <CPCMD> library\qnx.286\mm\clibm.lib <relroot>\rel2\lib286\qnx\clibm.lib
    <CPCMD> library\qnx.286\ms\clibs.lib <relroot>\rel2\lib286\qnx\clibs.lib
    <CPCMD> startup\library\qnx.286\ms\commode.obj <relroot>\rel2\lib286\qnx\commode.obj
    <CPCMD> library\qnx.386\ms_r\clibs.lib <relroot>\rel2\lib386\qnx\clib3r.lib
    <CPCMD> library\qnx.386\ms_s\clibs.lib <relroot>\rel2\lib386\qnx\clib3s.lib
    <CPCMD> startup\library\qnx.386\ms_r\commode.obj <relroot>\rel2\lib386\qnx\commode.obj
    <CPCMD> library\windows.086\mc\clibc.lib <relroot>\rel2\lib286\win\clibc.lib
    <CPCMD> library\windows.086\ml\clibl.lib <relroot>\rel2\lib286\win\clibl.lib
    <CPCMD> startup\library\windows.086\ml\libentry.obj <relroot>\rel2\lib286\win\libentry.obj
    <CPCMD> library\windows.086\mm\clibm.lib <relroot>\rel2\lib286\win\clibm.lib
    <CPCMD> library\windows.086\ms\clibs.lib <relroot>\rel2\lib286\win\clibs.lib
    <CPCMD> startup\library\windows.086\ms\binmode.obj <relroot>\rel2\lib286\win\binmode.obj
    <CPCMD> startup\library\windows.086\ms\commode.obj <relroot>\rel2\lib286\win\commode.obj
    <CPCMD> library\windows.386\mf_r\clibf.lib <relroot>\rel2\lib386\win\clib3r.lib
    <CPCMD> library\windows.386\mf_s\clibf.lib <relroot>\rel2\lib386\win\clib3s.lib
    <CPCMD> startup\library\windows.386\mf_r\binmode.obj <relroot>\rel2\lib386\win\binmode.obj
    <CPCMD> startup\library\windows.386\mf_r\commode.obj <relroot>\rel2\lib386\win\commode.obj
    <CPCMD> library\winnt.386\ms_r\clibs.lib <relroot>\rel2\lib386\nt\clib3r.lib
    <CPCMD> library\winnt.386\ms_s\clibs.lib <relroot>\rel2\lib386\nt\clib3s.lib
    <CPCMD> startup\library\winnt.386\ms_r\binmode.obj <relroot>\rel2\lib386\nt\binmode.obj
    <CPCMD> startup\library\winnt.386\ms_r\commode.obj <relroot>\rel2\lib386\nt\commode.obj
#    <CPCMD> library\winnt.axp\_s\clib.lib <relroot>\rel2\libaxp\nt\clib.lib
#    <CPCMD> startup\library\winnt.axp\_s\binmode.obj <relroot>\rel2\libaxp\nt\binmode.obj
#    <CPCMD> startup\library\winnt.axp\_s\commode.obj <relroot>\rel2\libaxp\nt\commode.obj

[ BLOCK <1> clibqa ]
#===================
    cd qa
    pmake -d clibqa -h

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean
