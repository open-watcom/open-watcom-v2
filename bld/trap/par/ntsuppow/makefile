#pmake: nobuild remote os_nt cpu_386 par
host_os  = nt
host_cpu = 386

proj_name = dbgport

dbgport_autodepends = .autodepend

additional_cleanup = dbgport.sys

suppress_zc = 1

!include cproj.mif
!include defrule.mif
!include deftarg.mif

extra_cpp_flags = -D_X86_=1 -Di386=1 -DWIN32_LEAN_AND_MEAN=1
extra_c_flags = -zp8 -s
inc_dirs = -I. -I"../h" -I"$(w32api_dir)/nt/ddk"

ddk_dir = $(w32api_dir)/nt386/ddk

.c : ../c

dbgport.sys : dbgport.obj $(__MAKEFILES__)
    %create dbgport.lnk
    @%append dbgport.lnk system nt_dll
    @%append dbgport.lnk runtime native=4.0
    @%append dbgport.lnk option osversion=4.0
    @%append dbgport.lnk option version=4.0
    @%append dbgport.lnk option nodef, start='_DriverEntry@8'
    @%append dbgport.lnk option alignment=0x20, objalign=0x20
    @%append dbgport.lnk option offset=0x10000, map, checksum
    @%append dbgport.lnk option stack=0x100000 commit stack=0x1000
    @%append dbgport.lnk option heapsize=0x100000 commit heap=0x1000
    @%append dbgport.lnk segment class CODE nonpageable
    @%append dbgport.lnk segment class DATA nonpageable
    @%append dbgport.lnk file dbgport.obj
    @%append dbgport.lnk lib $(ddk_dir)/ntoskrnl.lib
    @%append dbgport.lnk lib $(ddk_dir)/hal.lib
    @%append dbgport.lnk lib clib3r.lib
    @%make echo_link
    $(linker) name dbgport.sys @dbgport.lnk
