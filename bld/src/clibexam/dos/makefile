compiler = wcc
cflags = -bt=dos
linkflags = sys dos debug all
cdebug = -d2

!include ..\master.mif

all: $(FN_ALL) $(FN_MACRO) $(FN_FILE) $(FN_STRING) $(FN_ALLN) &
        $(FN_DP) $(FN_BIOS) $(FN_DOSCALL) $(FN_16BIT) $(FN_DOSWIN) &
        $(FN_DOS)

{..\}.c.exe:
    $(compiler) $(cflags) $(cdebug) $<
    wlink $(linkflags) name $* file $*.obj
