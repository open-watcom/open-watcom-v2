compiler  = wcc386
cflags    = -zq -bt=dos -wx
linkflags = sys dos4g debug all op q
cdebug    = -d2

!include ../master.mif

all: $(FN_ALL) $(FN_MACRO) $(FN_FILE) $(FN_STRING) $(FN_ALLN) &
     $(FN_DP) $(FN_BIOS) $(FN_DOSCALL) $(FN_DQN32) &
     $(FN_DOS) $(FN_32BIT) $(FN_SPAWN)

{..\}.c.exe:
    $(compiler) $(cflags) $(cdebug) $<
    wlink $(linkflags) name $* file $*.obj
