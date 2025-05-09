compiler  = *wcc386
cflags    = -zq -bt=os2 -I"$(%WATCOM)/h" -wx
linkflags = sys os2v2 debug all op q
cdebug    = -d2
exe       = .exe

!include ../master.mif

all: $(FN_ALL) $(FN_MACRO) $(FN_FILE) $(FN_STRING) &
     $(FN_DOSCALL) $(FN_THREAD) $(FN_OS2WIN32) $(FN_WQO) $(FN_32BIT) &
     $(FN_SPAWN) $(FN_MATH) $(FN_MMX)
