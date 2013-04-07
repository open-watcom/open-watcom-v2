proj_name = wmake_regress_cmds

!include cproj.mif
!include deftarg.mif
!include defrule.mif

targets = mkerr.exe envarg.exe ifeq.exe

!ifdef __UNIX__
targets += rem
additional_cleanup = rem
!endif

all: $(targets) .SYMBOLIC
        @%null
        
mkerr.exe: mkerr.c
        $(bld_cl) $(bld_clflags) $[@ $(bld_ldflags)

envarg.exe: envarg.c
        $(bld_cl) $(bld_clflags) $[@ $(bld_ldflags)

ifeq.exe: ifeq.c
        $(bld_cl) $(bld_clflags) $[@ $(bld_ldflags)

rem: rem.c
        $(bld_cl) $(bld_clflags) $[@ $(bld_ldflags)
