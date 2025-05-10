compiler = wcc386
cflags = -bt=netware
linkflags = sys netware debug all
cdebug = -d2

!include netware.mif

all: $(FN_MACRO) $(FN_FILE) $(FN_STRING) $(FN_ALLN) $(FN_THREAD) $(FN_DQN32) &
        $(FN_32BIT)

{..\}.c.nlm:
    $(compiler) $(cflags) $(cdebug) $<
    wlink $(linkflags) name $* file $*.obj
