# VI Builder Control file
# =======================
set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
#========================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h bind=1

[ BLOCK <1> rel2 cprel2 ]
#========================
#   <CPCMD> <devdir>\vi\obj286\vi.exe <relroot>\rel2\binw\vi286.exe
    <CPCMD> <devdir>\vi\objwin\viw.exe <relroot>\rel2\binw\viw.exe
#   <CPCMD> <devdir>\vi\objwinp\viwp.exe <relroot>\rel2\binw\viwp.exe
    <CPCMD> <devdir>\vi\objwinnt\vintw.exe <relroot>\rel2\binnt\viw.exe
#   <CPCMD> <devdir>\vi\objos2\vi2r.exe <relroot>\rel2\binp\vi16.exe
    <CPCMD> <devdir>\vi\obj2\vi2.exe <relroot>\rel2\binp\vi.exe
    <CPCMD> <devdir>\vi\obj386\vi386.exe <relroot>\rel2\binw\vi.exe
#    <CPCMD> <devdir>\vi\obj286v\viv.exe <relroot>\rel2\binw\vir.exe
    <CPCMD> <devdir>\vi\objqnx\viq.qnx <relroot>\rel2\qnx\binq\vi.
    <CPCMD> <devdir>\vi\objnt\vint.exe <relroot>\rel2\binnt\vi.exe
#    <CPCMD> <devdir>\vi\objaxp\viaxp.exe <relroot>\rel2\axpnt\vi.exe
#    <CPCMD> <devdir>\vi\objwinap\viaxpw.exe <relroot>\rel2\axpnt\viw.exe
    <CPCMD> <devdir>\vi\ctags\objos2\ctags.exe <relroot>\rel2\binp\ctags.exe
    <CPCMD> <devdir>\vi\ctags\obj\ctags.exe <relroot>\rel2\binw\ctags.exe
    <CPCMD> <devdir>\vi\ctags\objnt\ctags.exe <relroot>\rel2\binnt\ctags.exe
#    <CPCMD> <devdir>\vi\ctags\objaxp\ctags.exe <relroot>\rel2\axpnt\ctags.exe
    <CPCMD> <devdir>\vi\bind\obj\edbind.exe <relroot>\rel2\binw\edbind.exe
    <CPCMD> <devdir>\vi\bind\objnt\edbind.exe <relroot>\rel2\binnt\edbind.exe

    <CPCMD> <devdir>\vi\dat\v*.cmd <relroot>\rel2\binp\
    <CPCMD> <devdir>\vi\dat\v*.bat <relroot>\rel2\binw\
    <CPCMD> <devdir>\vi\dat\*.vi <relroot>\rel2\eddat\
    <CPCMD> <devdir>\vi\dat\*.cfg <relroot>\rel2\eddat\
    <CPCMD> <devdir>\vi\dat\*.dat <relroot>\rel2\eddat\
    <CPCMD> <devdir>\vi\dat\*.ini <relroot>\rel2\eddat\
    <CPCMD> <devdir>\vi\dat\bindvi.* <relroot>\rel2\eddat\
#   <CPCMD> <devdir>\vi\dat\weditor.ini <relroot>\rel2\binw\
#   <CPCMD> <relroot>\rel2\weditor.ini <relroot>\rel2\binnt\

    <CPCMD> <devdir>\vi\doc\*.hlp <relroot>\rel2\eddat\

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean
