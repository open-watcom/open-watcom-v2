compiler = wcc
cflags = -bt=windows -bw
linkflags = sys win debug all
cdebug = -d2

!include ..\master.mif

all: $(FN_ALL) $(FN_MACRO) $(FN_FILE) $(FN_STRING) $(FN_ALLN) &
        $(FN_BIOS) $(FN_DOSCALL) $(FN_16BIT) $(FN_DOSWIN) $(FN_WIN16) &
        $(FN_DEFWIN)

{..\}.c.exe:
    $(compiler) $(cflags) $(cdebug) $<
    wlink $(linkflags) name $* file $*.obj
