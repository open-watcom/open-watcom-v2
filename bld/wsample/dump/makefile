#pmake: cpu_386
host_cpu = 386

!include cproj.mif
!include defrule.mif
!include deftarg.mif

smpdump.exe : smpdump.c
    @%make echo_bldccl
    $(bld_ccl) $(ppflags_common) $(ppflags_autodep) $(bld_ppflags_sys) $(bld_cppflags) -I"$(dig_dir)/h" -I"$(wsample_dir)/h" $(bld_cclflags) $[@ $(bld_ldflags)
