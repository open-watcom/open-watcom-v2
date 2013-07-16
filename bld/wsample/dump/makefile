#pmake: cpu_386
host_cpu = 386

!include cproj.mif
!include defrule.mif
!include deftarg.mif

smpdump.exe : smpdump.c
    $(bld_cl) -I"$(dig_dir)/h" $(bld_clflags) $[@ $(bld_ldflags)