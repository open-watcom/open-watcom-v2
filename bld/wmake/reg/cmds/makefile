.extensions:
.extensions: . .exe .obj .c

.c.obj :
    $(comp286) $[* -d1 -oailt -s -zq -w4

.obj.exe :
    $(linker) debug all system dos file $[* opt quiet

.exe. : .symbolic
    @%null

all :   ifeq mkerr envarg .symbolic
    @%null
