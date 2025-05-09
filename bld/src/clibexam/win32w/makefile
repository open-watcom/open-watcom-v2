compiler  = *wcc386
cflags    = -bt=nt -I"$(%WATCOM)/h" -bw
linkflags = sys nt_win debug all
cdebug    = -d2
exe       = .exe

!include ../master.mif

all: $(FN_ALL) $(FN_MACRO) $(FN_FILE) $(FN_STRING) $(FN_WQO) &
     $(FN_DOSCALL) $(FN_THREAD) $(FN_OS2WIN32) $(FN_32BIT) $(FN_DEFWIN) &
     $(FN_MMX) $(FN_SPAWN)
