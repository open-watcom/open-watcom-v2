# DIP Builder Control file
# ========================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 cprel2 ]
#========================
#       OS2
        <CPCMD> <devdir>\bin\binp\dll\dwarf.dll         <relroot>\rel2\binp\dll\
        <CPCMD> <devdir>\bin\binp\dll\codeview.dll      <relroot>\rel2\binp\dll\
        <CPCMD> <devdir>\bin\binp\dll\export.dll        <relroot>\rel2\binp\dll\
        <CPCMD> <devdir>\bin\binp\dll\watcom.dll        <relroot>\rel2\binp\dll\
        <CPCMD> <devdir>\bin\binp\dll\dwarf.sym         <relroot>\rel2\binp\dll\
        <CPCMD> <devdir>\bin\binp\dll\codeview.sym      <relroot>\rel2\binp\dll\
        <CPCMD> <devdir>\bin\binp\dll\export.sym        <relroot>\rel2\binp\dll\
        <CPCMD> <devdir>\bin\binp\dll\watcom.sym        <relroot>\rel2\binp\dll\
#       NT
        <CPCMD> <devdir>\bin\binnt\dwarf.dll    <relroot>\rel2\binnt\
        <CPCMD> <devdir>\bin\binnt\codeview.dll <relroot>\rel2\binnt\
        <CPCMD> <devdir>\bin\binnt\export.dll   <relroot>\rel2\binnt\
        <CPCMD> <devdir>\bin\binnt\watcom.dll   <relroot>\rel2\binnt\
        <CPCMD> <devdir>\bin\binnt\dwarf.sym    <relroot>\rel2\binnt\
        <CPCMD> <devdir>\bin\binnt\codeview.sym <relroot>\rel2\binnt\
        <CPCMD> <devdir>\bin\binnt\export.sym   <relroot>\rel2\binnt\
        <CPCMD> <devdir>\bin\binnt\watcom.sym   <relroot>\rel2\binnt\
#       WINDOWS
        <CPCMD> <devdir>\bin\bin\dwarf.dll      <relroot>\rel2\binw\
        <CPCMD> <devdir>\bin\bin\codeview.dll   <relroot>\rel2\binw\
        <CPCMD> <devdir>\bin\bin\export.dll     <relroot>\rel2\binw\
        <CPCMD> <devdir>\bin\bin\watcom.dll     <relroot>\rel2\binw\
        <CPCMD> <devdir>\bin\bin\dwarf.sym      <relroot>\rel2\binw\
        <CPCMD> <devdir>\bin\bin\codeview.sym   <relroot>\rel2\binw\
        <CPCMD> <devdir>\bin\bin\export.sym     <relroot>\rel2\binw\
        <CPCMD> <devdir>\bin\bin\watcom.sym     <relroot>\rel2\binw\
#       DOS
        <CPCMD> <devdir>\bin\bin\dwarf.dip      <relroot>\rel2\binw\
        <CPCMD> <devdir>\bin\bin\codeview.dip   <relroot>\rel2\binw\
        <CPCMD> <devdir>\bin\bin\export.dip     <relroot>\rel2\binw\
        <CPCMD> <devdir>\bin\bin\watcom.dip     <relroot>\rel2\binw\
        <CPCMD> <devdir>\bin\bin\dwarf.sym      <relroot>\rel2\binw\dwarf.dsy
        <CPCMD> <devdir>\bin\bin\codeview.sym   <relroot>\rel2\binw\codeview.dsy
        <CPCMD> <devdir>\bin\bin\export.sym     <relroot>\rel2\binw\export.dsy
        <CPCMD> <devdir>\bin\bin\watcom.sym     <relroot>\rel2\binw\watcom.dsy
#       QNX
        <CPCMD> <devdir>\bin\qnx\dwarf.dip      <relroot>\rel2\qnx\watcom\wd\
        <CPCMD> <devdir>\bin\qnx\codeview.dip   <relroot>\rel2\qnx\watcom\wd\
        <CPCMD> <devdir>\bin\qnx\export.dip     <relroot>\rel2\qnx\watcom\wd\
        <CPCMD> <devdir>\bin\qnx\watcom.dip     <relroot>\rel2\qnx\watcom\wd\
        <CPCMD> <devdir>\bin\qnx\dwarf.sym      <relroot>\rel2\qnx\sym\
        <CPCMD> <devdir>\bin\qnx\codeview.sym   <relroot>\rel2\qnx\sym\
        <CPCMD> <devdir>\bin\qnx\export.sym     <relroot>\rel2\qnx\sym\
        <CPCMD> <devdir>\bin\qnx\watcom.sym     <relroot>\rel2\qnx\sym\
#       ALPHA
#       <CPCMD> <devdir>\bin\axpnt\dwarf.dll    <relroot>\rel2\axpnt\
#       <CPCMD> <devdir>\bin\axpnt\codeview.dll <relroot>\rel2\axpnt\
#       <CPCMD> <devdir>\bin\axpnt\export.dll   <relroot>\rel2\axpnt\
#       <CPCMD> <devdir>\bin\axpnt\watcom.dll   <relroot>\rel2\axpnt\
#       <CPCMD> <devdir>\bin\axpnt\dwarf.sym    <relroot>\rel2\axpnt\
#       <CPCMD> <devdir>\bin\axpnt\codeview.sym <relroot>\rel2\axpnt\
#       <CPCMD> <devdir>\bin\axpnt\export.sym   <relroot>\rel2\axpnt\
#       <CPCMD> <devdir>\bin\axpnt\watcom.sym   <relroot>\rel2\axpnt\


[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean
    sweep killobjs
