proj_name = wmake_regress_cmds

host_os  = $(bld_os)
host_cpu = $(bld_cpu)

!include cproj.mif
!include deftarg.mif
!include defrule.mif

targets = mkerr.exe envarg.exe ifeq.exe

!ifdef __UNIX__
targets += rem.exe
!else
targets += prntdir.exe
!endif

all: $(targets) .SYMBOLIC
        @%null
        
.c.exe :
        $(bld_cl) $[@ $(bld_clflags) $(bld_ldflags)
