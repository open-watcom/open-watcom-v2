.IGNORE

scan_files = &
bind.obj &
dialog.obj &
main.obj &
scanner.obj &
styles.obj &
prsbnd.obj &
prsdlg.obj

browse_files = &
chfile.obj  &
chbffile.obj &
mempool.obj

c_objs = $+$(scan_files) $(browse_files)$-

lnk = dlgprs.lnk

!ifdef __OS2__
yacc = $(plusplus_dir)\bin\yacc2.exe -b
!else
yacc = $(plusplus_dir)\bin\yacc.exe -b
!endif

wppdbgf = -d1-oaxt
wldbgf = pa .

.BEFORE
    @set compflags=-zq-mf-xs-w4 -bt=dos $(wppdbgf) -i=$(browse_dir)\contain\h;$(browse_dir)\h;..\h;..\gen;$(lang_root)\h\win

.extensions:
.extensions: .exe .obj .gh .c .cpp .gpp .y

compile.tim : dlgprs.exe  dlgprsp.exe
    wtouch $^@

dlgprs.exe : $(c_objs) $(browse_objs)
    @%create $(lnk)
    @%append $(lnk) name $^@ system dos4g
    @%append $(lnk) $(wldbgf)
    @%append $(lnk) op map=dlgprs,quiet,show,art,static sort
    @for %i in ($(c_objs)) do @%append $(lnk) file %i
    wlink @$(lnk)

dlgprsp.exe : $(c_objs) $(browse_objs)
    @%create $(lnk)
    @%append $(lnk) name $^@ system os2v2
    @%append $(lnk) debug all
    @%append $(lnk) op map=dlgprsp,quiet,show,art,static sort
    @for %i in ($(c_objs)) do @%append $(lnk) file %i
    wlink @$(lnk)

..\gen\prsdlg.gh ..\gen\prsdlg.gpp : ..\y\prsdlg.y ..\cpp\yydriver.cpp
    $(yacc) $[@ $]@ >y.out
    @if exist ..\gen\prsdlg.gh erase ..\gen\prsdlg.gh >nul
    @copy ytab.h ..\gen\prsdlg.gh >nul
    @erase ytab.h >nul
    @if exist ..\gen\prsdlg.gpp erase ..\gen\prsdlg.gpp >nul
    @copy ytab.c ..\gen\prsdlg.gpp >nul
    @erase ytab.c >nul

..\gen\prsbnd.gh ..\gen\prsbnd.gpp : ..\y\prsbnd.y ..\cpp\yydriver.cpp
    $(yacc) $[@ $]@ >y.out
    @if exist ..\gen\prsbnd.gh erase ..\gen\prsbnd.gh >nul
    @copy ytab.h ..\gen\prsbnd.gh >nul
    @erase ytab.h >nul
    @if exist ..\gen\prsbnd.gpp erase ..\gen\prsbnd.cpp >nul
    @copy ytab.c ..\gen\prsbnd.gpp >nul
    @erase ytab.c >nul

scanner.obj : ..\cpp\scanner.cpp ..\gen\prsdlg.gh ..\gen\prsbnd.gh .AUTODEPEND
    wpp386 @compflags $[@

.cpp: ..\cpp;$(browse_dir)\cpp
.cpp.obj: .AUTODEPEND
    wpp386 @compflags $[@

.gpp: ..\gen
.gpp.obj: .AUTODEPEND
    wpp386 @compflags $[@
