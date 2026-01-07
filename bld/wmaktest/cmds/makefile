host_os  = $(bld_os)
host_cpu = $(bld_cpu)

proj_name = wmake_regress_cmds

!ifdef __UNIX__
exe=
c_dir=../c/
additional_clean = $(commands)
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

!ifdef %BUILDTEST

!include cproj.mif
!include defrule.mif
!include deftarg.mif

$(commands) : .SYMBOLIC
    @echo Compile: $@
    $(bld_cl) $(c_dir)$^&.c -I"../../watcom/h" $(bld_clflags) $(bld_ldflags)

!else

$(commands) : .SYMBOLIC
    @echo Compile: $@
    wcl386 $(c_dir)$^&.c -I"../../watcom/h" -fe=$@

!endif

