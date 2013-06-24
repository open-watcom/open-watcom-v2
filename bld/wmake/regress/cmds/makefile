proj_name = wmake_regress_cmds

targets = mkerr.exe envarg.exe ifeq.exe create.exe

!ifdef __UNIX__
targets += rem.exe
!else
targets += prntdir.exe
!endif

cl_386  = wcl386
cl_x64  = wcl386
cl_axp  = wclaxp
cl_i86  = wcl
cl_mps  = wclmps
cl_ppc  = wclppc

all: $(targets) .SYMBOLIC
        @%null
        
.c.exe :
        $(cl_$(bld_cpu)) -zq -I"../../../watcom/h" -Fe$@ $[@

clean: .symbolic
        rm -f *.obj *.exe
