#pmake: build lcl win 16 32 support x86

!ifndef bin_root
!ifdef bin_root_wv
bin_root = $(bin_root_wv)
!endif
!endif

CFLAGS = -zq -s -oas -fpc -mc -w3 -d1 -i=$(%lang)\h;$(%lang)\h\win;$(watcom_dir)\h

NAME = wint32
LNK = $(NAME).lnk

OBJS =  intmain.obj ints.obj int32.obj

w : .symbolic $(bin_root)\bin\$(name).dll
        @%null

$(bin_root)\bin\$(name).dll: $(OBJS) $(LNK)
       wlink name $^@ @$(LNK)

$(LNK) : $(__MAKEFILES__)
        %create $(LNK)
        @%append $(LNK) system windows dll initinstance memory
        @%append $(LNK) library windows,clibc,toolhelp
        @%append $(LNK) option map
        @%append $(LNK) option heapsize=1k
        @for %i in ($(objs)) do @%append $(LNK) file %i
        @%append $(LNK) libfile libentry.obj
        @%append $(LNK) exp WEP.1 resident
        @%append $(LNK) segment CLASS 'CODE' PRELOAD FIXED
        @%append $(LNK) segment CLASS 'FAR_DATA' PRELOAD FIXED
        @%append $(LNK) segment CLASS 'BEGDATA' PRELOAD FIXED
        @%append $(LNK) segment CLASS 'DATA' PRELOAD FIXED
        @%append $(LNK) segment CLASS 'BSS' PRELOAD FIXED

.c : ..\c
.c.obj :
        $(comp286) $[* $(CFLAGS) -zu -zw

.asm : ..\asm
.asm.obj :
        wasm $[* $(CFLAGS)
