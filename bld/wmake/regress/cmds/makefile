host_os  = $(bld_os)
host_cpu = $(bld_cpu)

proj_name = wmake_regress_cmds

!include cproj.mif
!include defrule.mif
!include deftarg.mif

commands = mkerr envarg ifeq

!ifdef __UNIX__
commands += rem
!else
commands += prntdir
!endif

$(commands) : .symbolic
        $(bld_cl) $@.c -I"../../../watcom/h" $(bld_clflags) $(bld_ldflags)
