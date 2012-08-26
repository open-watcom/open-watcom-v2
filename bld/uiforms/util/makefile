cflags = -zq-d1-osal-s-w3 -i"$(ui_h)"
cflags286 = -ml-of
cflags386 = -mf

compile286  = $(comp286) $[* $(cflags) $(cflags286)
compile386  = $(comp386) $[* $(cflags) $(cflags386)

# NOTE: Make sure the QNX executable is always last on the list

objs  = &
        browse.obj browse.wbj browse.pbj browse.3bj browse.qbj

all : .SYMBOLIC $(objs)
        @%null

.IGNORE
.NOCHECK

.EXTENSIONS
.EXTENSIONS : .exe .obj .pbj .3bj .qbj .wbj .c .asm


.c.obj:
        $(compile286)

.c.qbj:
        set include=$(lang_root)/qh
        $(compile286) -d__QNX__ -fo=.qbj

.c.wbj:
        $(compile286) -zW -fo=.wbj

.c.3bj:
        $(compile386) -bt=nt -fo=.3bj

.c.pbj:
        $(compile386) -fo=.pbj
