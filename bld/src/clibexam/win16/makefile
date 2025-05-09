compiler  = *wcc
cflags    = -bt=windows -I"$(%WATCOM)/h" -bw
linkflags = sys windows debug all
cdebug    = -d2
exe       = .exe

!include ../master.mif

all: $(FN_ALL) $(FN_MACRO) $(FN_FILE) $(FN_STRING) &
        $(FN_BIOS) $(FN_DOSCALL) $(FN_16BIT) $(FN_DOSWIN) $(FN_WIN16) &
        $(FN_DEFWIN)
