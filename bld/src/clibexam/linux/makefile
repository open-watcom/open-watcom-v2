compiler  = *wcc386
cflags    = -zq -bt=linux -I"$(%WATCOM)/lh" -wx
linkflags = sys linux debug all op q
cdebug    = -d2
exe       = .exe

!include ../master.mif

all: $(FN_ALL) $(FN_MACRO) $(FN_FILE) $(FN_STRING) &
     $(FN_DQN32) $(FN_32BIT) $(FN_SPAWN)
