compiler = wcc386
cflags = -bt=dos
linkflags = sys dos4g debug all
cdebug = -d2

!include ..\master.mif

all: $(FN_ALL) $(FN_MACRO) $(FN_FILE) $(FN_STRING) $(FN_ALLN) &
        $(FN_DP) $(FN_BIOS) $(FN_DOSCALL) $(FN_DQN32) &
        $(FN_DOS) $(FN_32BIT)

{..\}.c.exe:
    $(compiler) $(cflags) $(cdebug) $<
    wlink $(linkflags) name $* file $*.obj
