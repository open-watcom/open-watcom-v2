# SRC Builder Control file
# ========================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    cdsay ..\f77\samples\windll
    wmake /h /i cover16.obj
    cdsay <PROJDIR>

[ BLOCK <1> rel2 cprel2 ]
#========================
    sweep if exist *.sex ren *.sex *.exe
    sweep if exist *.sdl ren *.sdl *.dll
    <CPCMD> <devdir>\clib\startup\a\cstrt386.asm <relroot>\rel2\src\startup\386\cstrt386.asm
    <CPCMD> <devdir>\clib\startup\a\cstrtw32.asm <relroot>\rel2\src\startup\386\cstrtw32.asm
    <CPCMD> <devdir>\clib\startup\a\cstrtwnt.asm <relroot>\rel2\src\startup\386\cstrtwnt.asm
    <CPCMD> <devdir>\clib\startup\a\cstrtwwt.asm <relroot>\rel2\src\startup\386\cstrtwwt.asm
    <CPCMD> <devdir>\clib\startup\a\cstrtx32.asm <relroot>\rel2\src\startup\386\cstrtx32.asm
    <CPCMD> <devdir>\clib\startup\c\cmain386.c <relroot>\rel2\src\startup\386\cmain386.c
    <CPCMD> <devdir>\clib\startup\c\wildargv.c <relroot>\rel2\src\startup\wildargv.c
    <CPCMD> <devdir>\clib\startup\a\cstrt086.asm <relroot>\rel2\src\startup\dos\cstrt086.asm
    <CPCMD> <devdir>\clib\startup\a\dos16m.asm <relroot>\rel2\src\startup\dos\dos16m.asm
    <CPCMD> <devdir>\clib\startup\a\dstrt386.asm <relroot>\rel2\src\startup\386\dstrt386.asm
    <CPCMD> <devdir>\clib\startup\c\cmain086.c <relroot>\rel2\src\startup\dos\cmain086.c
    <CPCMD> <devdir>\clib\startup\c\cmain086.c <relroot>\rel2\src\startup\os2\cmain086.c
    <CPCMD> <devdir>\clib\startup\c\hpgrw086.c <relroot>\rel2\src\startup\dos\hpgrw086.c
    <CPCMD> <devdir>\clib\startup\c\dmaino16.c <relroot>\rel2\src\startup\os2\dmaino16.c
    <CPCMD> <devdir>\clib\startup\c\libmno16.c <relroot>\rel2\src\startup\os2\libmno16.c
    <CPCMD> <devdir>\clib\startup\c\maino16.c <relroot>\rel2\src\startup\os2\maino16.c
    <CPCMD> <devdir>\clib\startup\c\main2o32.c <relroot>\rel2\src\startup\386\main2o32.c
    <CPCMD> <devdir>\clib\startup\c\libmno32.c <relroot>\rel2\src\startup\386\libmno32.c
    <CPCMD> <devdir>\clib\startup\c\dmaino32.c <relroot>\rel2\src\startup\386\dmaino32.c
    <CPCMD> <devdir>\clib\startup\c\main2wnt.c <relroot>\rel2\src\startup\386\main2wnt.c
    <CPCMD> <devdir>\clib\startup\c\wmainwnt.c <relroot>\rel2\src\startup\386\wmainwnt.c
    <CPCMD> <devdir>\clib\startup\c\lmainwnt.c <relroot>\rel2\src\startup\386\lmainwnt.c
    <CPCMD> <devdir>\clib\startup\c\dmainwnt.c <relroot>\rel2\src\startup\386\dmainwnt.c
    <CPCMD> <devdir>\clib\startup\c\lmn2wnt.c <relroot>\rel2\src\startup\386\lmn2wnt.c
    <CPCMD> <devdir>\clib\startup\a\cstrto16.asm <relroot>\rel2\src\startup\os2\cstrto16.asm
    <CPCMD> <devdir>\clib\startup\a\cstrto32.asm <relroot>\rel2\src\startup\386\cstrto32.asm
    <CPCMD> <devdir>\clib\startup\a\cstrtw16.asm <relroot>\rel2\src\startup\win\cstrtw16.asm
    <CPCMD> <devdir>\clib\startup\a\libentry.asm <relroot>\rel2\src\startup\win\libentry.asm
    <CPCMD> <devdir>\clib\startup\c\8087cw.c <relroot>\rel2\src\startup\8087cw.c
    <CPCMD> <devdir>\clib\startup\a\adstart.asm <relroot>\rel2\src\startup\ads\adstart.asm
    <CPCMD> <devdir>\lib_misc\h\exitwmsg.h <relroot>\rel2\src\startup\os2\exitwmsg.h
    <CPCMD> <devdir>\clib\startup\h\initfini.h <relroot>\rel2\src\startup\386\initfini.h
    <CPCMD> <devdir>\clib\startup\h\initfini.h <relroot>\rel2\src\startup\os2\initfini.h
    <CPCMD> <devdir>\watcom\h\wos2.h <relroot>\rel2\src\startup\os2\wos2.h
    <CPCMD> <devdir>\src\startup\dos\mdef.inc <relroot>\rel2\src\startup\dos\mdef.inc
    <CPCMD> <devdir>\src\startup\win\mdef.inc <relroot>\rel2\src\startup\win\mdef.inc
    <CPCMD> <devdir>\src\startup\os2\mdef.inc <relroot>\rel2\src\startup\os2\mdef.inc

    <CPCMD> <devdir>\src\misc\*.* <relroot>\rel2\src\
    <CPCMD> <devdir>\src\os2\pdd\*.*    <relroot>\rel2\src\os2\pdd\
    <CPCMD> <devdir>\f77\src\*.* <relroot>\rel2\src\fortran\
    <CPCMD> <devdir>\f77\src\win\*.* <relroot>\rel2\src\fortran\win\
    <CPCMD> <devdir>\f77\src\dos\*.* <relroot>\rel2\src\fortran\dos\
    <CPCMD> <devdir>\f77\src\ads\*.* <relroot>\rel2\src\fortran\ads\
    <CPCMD> <devdir>\f77\src\ads\rel12\*.* <relroot>\rel2\src\fortran\ads\rel12\
    <CPCMD> <devdir>\f77\src\os2\*.* <relroot>\rel2\src\fortran\os2\
    <CPCMD> <devdir>\f77\samples\os2\*.* <relroot>\rel2\samples\fortran\os2\
    <CPCMD> <devdir>\f77\samples\os2\rexx\*.* <relroot>\rel2\samples\fortran\os2\rexx\
    <CPCMD> <devdir>\f77\samples\mthread\mthrdos2.for <relroot>\rel2\samples\fortran\os2\mthread.for
    <CPCMD> <devdir>\f77\samples\mthread\mthrdnt.for <relroot>\rel2\samples\fortran\win32\mthread.for
    <CPCMD> <devdir>\f77\samples\win\*.* <relroot>\rel2\samples\fortran\win\
    <CPCMD> <devdir>\f77\samples\windll\*.* <relroot>\rel2\samples\fortran\win\dll\
    <CPCMD> <devdir>\f77\samples\dll\*.* <relroot>\rel2\samples\fortran\os2\dll\
    <CPCMD> <devdir>\f77\samples\dll\*.* <relroot>\rel2\samples\fortran\win32\dll\

    <CPCMD> <devdir>\plusplus\cpplib\contain\cpp\*.* <relroot>\rel2\src\cpplib\contain\

    <CPCMD> <devdir>\src\CLIBEXAM\*.*   <relroot>\rel2\samples\CLIBEXAM\
    <CPCMD> <devdir>\src\CPLBEXAM\*.*   <relroot>\rel2\samples\CPLBEXAM\

#    <CPCMD> <devdir>\src\CPPEXAMP\*.*  <relroot>\rel2\samples\CPPEXAMP\
#    <CPCMD> <devdir>\src\CPPEXAMP\EXCAREA\*.*  <relroot>\rel2\samples\CPPEXAMP\EXCAREA\
#    <CPCMD> <devdir>\src\CPPEXAMP\MEMBFUN\*.*  <relroot>\rel2\samples\CPPEXAMP\MEMBFUN\

    <CPCMD> <devdir>\src\DLL\*.*        <relroot>\rel2\samples\DLL\
    <CPCMD> <devdir>\src\GOODIES\*.*    <relroot>\rel2\samples\GOODIES\
#    <CPCMD> <devdir>\src\OS2\*.*       <relroot>\rel2\samples\OS2\
#    <CPCMD> <devdir>\src\P16\*.*       <relroot>\rel2\samples\P16\
    <CPCMD> <devdir>\src\WIN\*.*        <relroot>\rel2\samples\WIN\
    <CPCMD> <devdir>\src\CLIBEXAM\KANJI\*.*     <relroot>\rel2\samples\CLIBEXAM\KANJI\
    <CPCMD> <devdir>\src\CLIBEXAM\TEST\*.*      <relroot>\rel2\samples\CLIBEXAM\TEST\
    <CPCMD> <devdir>\src\CPLBEXAM\COMPLEX\*.*   <relroot>\rel2\samples\CPLBEXAM\COMPLEX\
    <CPCMD> <devdir>\src\CPLBEXAM\CONTAIN\*.*   <relroot>\rel2\samples\CPLBEXAM\CONTAIN\
    <CPCMD> <devdir>\src\CPLBEXAM\FSTREAM\*.*   <relroot>\rel2\samples\CPLBEXAM\FSTREAM\
    <CPCMD> <devdir>\src\CPLBEXAM\IOS\*.*       <relroot>\rel2\samples\CPLBEXAM\IOS\
    <CPCMD> <devdir>\src\CPLBEXAM\IOSTREAM\*.*  <relroot>\rel2\samples\CPLBEXAM\IOSTREAM\
    <CPCMD> <devdir>\src\CPLBEXAM\STRING\*.*    <relroot>\rel2\samples\CPLBEXAM\STRING\
    <CPCMD> <devdir>\src\CPLBEXAM\STRSTREA\*.*  <relroot>\rel2\samples\CPLBEXAM\STRSTREA\
    <CPCMD> <devdir>\src\CPLBEXAM\COMPLEX\FRIEND\*.*    <relroot>\rel2\samples\CPLBEXAM\COMPLEX\FRIEND\
    <CPCMD> <devdir>\src\CPLBEXAM\COMPLEX\PUBFUN\*.*    <relroot>\rel2\samples\CPLBEXAM\COMPLEX\PUBFUN\
    <CPCMD> <devdir>\src\CPLBEXAM\COMPLEX\RELFUN\*.*    <relroot>\rel2\samples\CPLBEXAM\COMPLEX\RELFUN\
    <CPCMD> <devdir>\src\CPLBEXAM\COMPLEX\RELOP\*.*     <relroot>\rel2\samples\CPLBEXAM\COMPLEX\RELOP\
    <CPCMD> <devdir>\src\CPLBEXAM\FSTREAM\FSTREAM\*.*   <relroot>\rel2\samples\CPLBEXAM\FSTREAM\FSTREAM\
    <CPCMD> <devdir>\src\CPLBEXAM\FSTREAM\IFSTREAM\*.*  <relroot>\rel2\samples\CPLBEXAM\FSTREAM\IFSTREAM\
    <CPCMD> <devdir>\src\CPLBEXAM\FSTREAM\OFSTREAM\*.*  <relroot>\rel2\samples\CPLBEXAM\FSTREAM\OFSTREAM\
    <CPCMD> <devdir>\src\CPLBEXAM\IOSTREAM\IOSTREAM\*.* <relroot>\rel2\samples\CPLBEXAM\IOSTREAM\IOSTREAM\
    <CPCMD> <devdir>\src\CPLBEXAM\IOSTREAM\ISTREAM\*.*  <relroot>\rel2\samples\CPLBEXAM\IOSTREAM\ISTREAM\
    <CPCMD> <devdir>\src\CPLBEXAM\IOSTREAM\OSTREAM\*.*  <relroot>\rel2\samples\CPLBEXAM\IOSTREAM\OSTREAM\
    <CPCMD> <devdir>\src\CPLBEXAM\STRING\FRIEND\*.*     <relroot>\rel2\samples\CPLBEXAM\STRING\FRIEND\
    <CPCMD> <devdir>\src\CPLBEXAM\STRING\PUBFUN\*.*     <relroot>\rel2\samples\CPLBEXAM\STRING\PUBFUN\
    <CPCMD> <devdir>\src\CPLBEXAM\STRSTREA\ISTRSTRE\*.* <relroot>\rel2\samples\CPLBEXAM\STRSTREA\ISTRSTRE\
    <CPCMD> <devdir>\src\CPLBEXAM\STRSTREA\OSTRSTRE\*.* <relroot>\rel2\samples\CPLBEXAM\STRSTREA\OSTRSTRE\
    <CPCMD> <devdir>\src\CPLBEXAM\STRSTREA\STRSTRE\*.*  <relroot>\rel2\samples\CPLBEXAM\STRSTREA\STRSTRE\
    <CPCMD> <devdir>\src\WIN\ALARM\*.*  <relroot>\rel2\samples\WIN\ALARM\
    <CPCMD> <devdir>\src\WIN\DATACTL\*.*        <relroot>\rel2\samples\WIN\DATACTL\
    <CPCMD> <devdir>\src\WIN\EDIT\*.*   <relroot>\rel2\samples\WIN\EDIT\
    <CPCMD> <devdir>\src\WIN\GENERIC\*.*        <relroot>\rel2\samples\WIN\GENERIC\
    <CPCMD> <devdir>\src\WIN\HELPEX\*.* <relroot>\rel2\samples\WIN\HELPEX\
    <CPCMD> <devdir>\src\WIN\ICONVIEW\*.*       <relroot>\rel2\samples\WIN\ICONVIEW\
    <CPCMD> <devdir>\src\WIN\LIFE\*.*   <relroot>\rel2\samples\WIN\LIFE\
    <CPCMD> <devdir>\src\WIN\SHOOTGAL\*.*       <relroot>\rel2\samples\WIN\SHOOTGAL\
    <CPCMD> <devdir>\src\WIN\WATZEE\*.* <relroot>\rel2\samples\WIN\WATZEE\
    <CPCMD> <devdir>\src\WIN\ALARM\WIN16\*.*    <relroot>\rel2\samples\WIN\ALARM\WIN16\
    <CPCMD> <devdir>\src\WIN\ALARM\WIN32\*.*    <relroot>\rel2\samples\WIN\ALARM\WIN32\
    <CPCMD> <devdir>\src\WIN\ALARM\WIN386\*.*   <relroot>\rel2\samples\WIN\ALARM\WIN386\
#    <CPCMD> <devdir>\src\WIN\ALARM\WINAXP\*.*  <relroot>\rel2\samples\WIN\ALARM\WINAXP\
    <CPCMD> <devdir>\src\WIN\DATACTL\WIN16\*.*  <relroot>\rel2\samples\WIN\DATACTL\WIN16\
    <CPCMD> <devdir>\src\WIN\DATACTL\WIN32\*.*  <relroot>\rel2\samples\WIN\DATACTL\WIN32\
    <CPCMD> <devdir>\src\WIN\DATACTL\WIN386\*.* <relroot>\rel2\samples\WIN\DATACTL\WIN386\
#    <CPCMD> <devdir>\src\WIN\DATACTL\WINAXP\*.*        <relroot>\rel2\samples\WIN\DATACTL\WINAXP\
    <CPCMD> <devdir>\src\WIN\EDIT\WIN16\*.*     <relroot>\rel2\samples\WIN\EDIT\WIN16\
    <CPCMD> <devdir>\src\WIN\EDIT\WIN32\*.*     <relroot>\rel2\samples\WIN\EDIT\WIN32\
    <CPCMD> <devdir>\src\WIN\EDIT\WIN386\*.*    <relroot>\rel2\samples\WIN\EDIT\WIN386\
#    <CPCMD> <devdir>\src\WIN\EDIT\WINAXP\*.*   <relroot>\rel2\samples\WIN\EDIT\WINAXP\
    <CPCMD> <devdir>\src\WIN\GENERIC\WIN16\*.*  <relroot>\rel2\samples\WIN\GENERIC\WIN16\
    <CPCMD> <devdir>\src\WIN\GENERIC\WIN32\*.*  <relroot>\rel2\samples\WIN\GENERIC\WIN32\
    <CPCMD> <devdir>\src\WIN\GENERIC\WIN386\*.* <relroot>\rel2\samples\WIN\GENERIC\WIN386\
#    <CPCMD> <devdir>\src\WIN\GENERIC\WINAXP\*.*        <relroot>\rel2\samples\WIN\GENERIC\WINAXP\
    <CPCMD> <devdir>\src\WIN\HELPEX\WIN16\*.*   <relroot>\rel2\samples\WIN\HELPEX\WIN16\
    <CPCMD> <devdir>\src\WIN\HELPEX\WIN32\*.*   <relroot>\rel2\samples\WIN\HELPEX\WIN32\
    <CPCMD> <devdir>\src\WIN\HELPEX\WIN386\*.*  <relroot>\rel2\samples\WIN\HELPEX\WIN386\
#    <CPCMD> <devdir>\src\WIN\HELPEX\WINAXP\*.* <relroot>\rel2\samples\WIN\HELPEX\WINAXP\
    <CPCMD> <devdir>\src\WIN\ICONVIEW\WIN16\*.* <relroot>\rel2\samples\WIN\ICONVIEW\WIN16\
    <CPCMD> <devdir>\src\WIN\ICONVIEW\WIN32\*.* <relroot>\rel2\samples\WIN\ICONVIEW\WIN32\
    <CPCMD> <devdir>\src\WIN\ICONVIEW\WIN386\*.*        <relroot>\rel2\samples\WIN\ICONVIEW\WIN386\
#    <CPCMD> <devdir>\src\WIN\ICONVIEW\WINAXP\*.*       <relroot>\rel2\samples\WIN\ICONVIEW\WINAXP\
    <CPCMD> <devdir>\src\WIN\LIFE\WIN16\*.*     <relroot>\rel2\samples\WIN\LIFE\WIN16\
    <CPCMD> <devdir>\src\WIN\LIFE\WIN32\*.*     <relroot>\rel2\samples\WIN\LIFE\WIN32\
    <CPCMD> <devdir>\src\WIN\LIFE\WIN386\*.*    <relroot>\rel2\samples\WIN\LIFE\WIN386\
#    <CPCMD> <devdir>\src\WIN\LIFE\WINAXP\*.*   <relroot>\rel2\samples\WIN\LIFE\WINAXP\
    <CPCMD> <devdir>\src\WIN\SHOOTGAL\WIN16\*.* <relroot>\rel2\samples\WIN\SHOOTGAL\WIN16\
    <CPCMD> <devdir>\src\WIN\SHOOTGAL\WIN32\*.* <relroot>\rel2\samples\WIN\SHOOTGAL\WIN32\
    <CPCMD> <devdir>\src\WIN\SHOOTGAL\WIN386\*.*        <relroot>\rel2\samples\WIN\SHOOTGAL\WIN386\
#    <CPCMD> <devdir>\src\WIN\SHOOTGAL\WINAXP\*.*       <relroot>\rel2\samples\WIN\SHOOTGAL\WINAXP\
    <CPCMD> <devdir>\src\WIN\WATZEE\WIN16\*.*   <relroot>\rel2\samples\WIN\WATZEE\WIN16\
    <CPCMD> <devdir>\src\WIN\WATZEE\WIN32\*.*   <relroot>\rel2\samples\WIN\WATZEE\WIN32\
    <CPCMD> <devdir>\src\WIN\WATZEE\WIN386\*.*  <relroot>\rel2\samples\WIN\WATZEE\WIN386\
#    <CPCMD> <devdir>\src\WIN\WATZEE\WINAXP\*.* <relroot>\rel2\samples\WIN\WATZEE\WINAXP\

    if exist <relroot>\rel2\src\__wprj__.del del <relroot>\rel2\src\__wprj__.del
    sweep if exist *.exe ren *.exe *.sex
    sweep if exist *.dll ren *.dll *.sdl

[ BLOCK <1> clean ]
#==================
    sweep killobjs
