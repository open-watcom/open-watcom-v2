compiler = wcc386
cflags = -bt=nt
linkflags = sys nt debug all
cdebug = -d2

!include ..\master.mif

all: $(FN_ALL) $(FN_MACRO) $(FN_FILE) $(FN_STRING) $(FN_ALLN) &
        $(FN_WQO) $(FN_DOSCALL) $(FN_THREAD) $(FN_OS2WIN32) $(FN_32BIT) &
        $(FN_WIN32)

{..\}.c.exe:
    $(compiler) $(cflags) $(cdebug) $<
    wlink $(linkflags) name $* file $*.obj
