proj_name = wmake_regress_cmds

commands = mkerr envarg ifeq create

!ifdef __UNIX__
commands += rem
!else
commands += prntdir
!endif

cl_386  = wcl386
cl_x64  = wcl386
cl_axp  = wclaxp
cl_i86  = wcl
cl_mps  = wclmps
cl_ppc  = wclppc

$(commands) : .symbolic
        $(cl_$(bld_cpu)) -zq -I"../../../watcom/h" $@.c

clean: .symbolic
        rm -f *.obj *.o *.exe $(commands)
