LNK = wdebug.lnk
LNKEMU = wemu387.lnk

objs =  wdebug.obj
objl = $(emu386)\emu387w.obj

all : wdebug.386 wemu387.386 .SYMBOLIC
        @%null

wdebug.386: wdebug.def $(OBJS) $(OBJL) $(LNK)
        link386 @$(LNK)
        addhdr wdebug.386
        mapsym32 wdebug

wemu387.386: wemu387.def $(OBJS) $(OBJL) $(LNKEMU)
        link386 @$(LNKEMU)
        addhdr wemu387.386
        mapsym32 wemu387

$(LNK) : makefile
        %create $(LNK)
        @for %i in ($(objs)) do @%append $(lnk) %i+
        @%append $(LNK) $(objl)
        @%append $(LNK) wdebug.386 /NOI /NOD /NOP
        @%append $(LNK) wdebug.map /MAP
        @%append $(LNK)
        @%append $(LNK) wdebug.def

$(LNKEMU) : makefile
        %create $(LNKEMU)
        @for %i in ($(objs)) do @%append $(LNKEMU) %i+
        @%append $(LNKEMU) $(objl)
        @%append $(LNKEMU) wemu387.386 /NOI /NOD /NOP
        @%append $(LNKEMU) wemu387.map /MAP
        @%append $(LNKEMU)
        @%append $(LNKEMU) wemu387.def

.obj : $(OBJ)

.asm.obj :
        masm5 -p -w2 -Mx $(DEBUG) $[*;
