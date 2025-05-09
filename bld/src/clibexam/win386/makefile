compiler  = *wcc386
cflags    = -bt=windows -I"$(%WATCOM)/h" -bw
linkflags = sys win386 debug all
cdebug    = -d2
exe       = .exe

!include ../master.mif

all: $(FN_ALL) $(FN_MACRO) $(FN_FILE) $(FN_STRING) &
        $(FN_BIOS) $(FN_DOSCALL) $(FN_DOSWIN) $(FN_DEFWIN) $(FN_32BIT)
