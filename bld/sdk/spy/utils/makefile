!include bhost.mif
name = mkdlg
lnk = $(name).lnk
cflags= -zq-oaxt-s-d2-fpc-w4-i=..\..\misc;..\ -d__NOSPYMSGS__

link = $(linker)
cc = wcc

.EXTENSIONS:
.EXTENSIONS: .exe
.EXTENSIONS: .obj
.EXTENSIONS: .asm .c .h


objs =  mkdlg.obj

$(name).exe : $(objs) $(lnk)
        $(bld_link) @$(lnk)

$(lnk) : makefile
        %create $(lnk)
        @%append $(lnk) debug all
        @%append $(lnk) name $(name).exe
        @%append $(lnk) option map
        @for %i in ($(objs)) do @%append $(lnk) file %i

mkdlg.obj: .SYMBOLIC
        $(bld_cc) $^* $(cflags) $(defines)

clean: .SYMBOLIC
        @if exist *.obj @del *.obj
        @if exist *.map @del *.map
        @if exist $(lnk) @del $(lnk)
        @if exist $(name).exe @del $(name).exe
