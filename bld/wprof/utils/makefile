
host_cpu = $(bld_cpu)
host_os = $(bld_os)

proj_name = utils

!include cproj.mif
!include defrule.mif
!include deftarg.mif

.c: .

all : smpsync.exe mergehlp.exe smpmt.exe .symbolic
    @%null

.c.exe :
    @%make echo_bldccl
    $(bld_ccl) -fm -I"$(wprof_dir)/h" -I"$(dig_dir)/h" -I"$(wsample_dir)/h" $(bld_cclflags) $[@ $(bld_ldflags)
