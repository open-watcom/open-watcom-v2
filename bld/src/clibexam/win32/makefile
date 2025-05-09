compiler  = *wcc386
cflags    = -zq -bt=nt -I"$(%WATCOM)/h" -wx
linkflags = sys nt debug all op q
cdebug    = -d2
exe       = .exe

!include ../master.mif

all: $(FN_ALL) $(FN_MACRO) $(FN_FILE) $(FN_STRING) &
        $(FN_WQO) $(FN_DOSCALL) $(FN_THREAD) $(FN_OS2WIN32) $(FN_32BIT) &
        $(FN_MMX) $(FN_SPAWN)
