compiler = wcc386
cflags = -bt=os2 -bw
linkflags = sys os2v2 pm debug all
cdebug = -d2

!include ../master.mif

all: $(FN_ALL) $(FN_MACRO) $(FN_FILE) $(FN_STRING) $(FN_ALLN) &
        $(FN_DOSCALL) $(FN_THREAD) $(FN_OS2WIN32) $(FN_WQO) $(FN_32BIT) &
        $(FN_DEFWIN) $(FN_SPAWN)

{..\}.c.exe:
    $(compiler) $(cflags) $(cdebug) $<
    wlink $(linkflags) name $* file $*.obj
