compiler  = *wcc
cflags    = -zq -bt=dos -I"$(%WATCOM)/h" -wx
linkflags = sys dos debug all op q
cdebug    = -d2
exe       = .exe

!include ../master.mif

all: $(FN_ALL) $(FN_MACRO) $(FN_FILE) $(FN_STRING) &
     $(FN_DP) $(FN_BIOS) $(FN_DOSCALL) $(FN_16BIT) $(FN_DOSWIN) &
     $(FN_DOS) $(FN_SPAWN)
