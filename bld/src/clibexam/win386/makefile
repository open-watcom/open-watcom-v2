compiler = wcc386
cflags = -bt=windows -bw
linkflags = sys win386 debug all
cdebug = -d2

!include ../master.mif

all: $(FN_ALL) $(FN_MACRO) $(FN_FILE) $(FN_STRING) $(FN_ALLN) &
        $(FN_BIOS) $(FN_DOSCALL) $(FN_DOSWIN) $(FN_DEFWIN) $(FN_32BIT)

{..\}.c.exe:
    $(compiler) $(cflags) $(cdebug) $<
    wlink $(linkflags) name $* file $*.obj
    wbind $* -q -n
