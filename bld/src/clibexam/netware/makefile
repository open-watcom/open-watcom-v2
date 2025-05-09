compiler  = *wcc386
cflags    = -bt=netware -I"$(%WATCOM)/h"
linkflags = sys netware debug all
cdebug    = -d2
exe       = .nlm

!include ../master.mif

all: $(FN_MACRO) $(FN_FILE) $(FN_STRING) $(FN_NETW) $(FN_THREAD) $(FN_DQN32) &
        $(FN_32BIT)
