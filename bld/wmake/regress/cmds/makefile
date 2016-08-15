host_os  = $(bld_os)
host_cpu = $(bld_cpu)

proj_name = wmake_regress_cmds

!include cproj.mif
!include defrule.mif
!include deftarg.mif

!ifdef __UNIX__
exe=
c_dir=../c/
!else
exe=.exe
c_dir=..\c\
!endif

commands = mkerr$(exe) envarg$(exe) ifeq$(exe)
!ifdef __UNIX__
commands += rem$(exe)
!else
commands += prntdir$(exe)
!endif

$(commands) : .symbolic
        $(bld_cl) $(c_dir)$^&.c -I"../../../watcom/h" $(bld_clflags) $(bld_ldflags)
