#pmake: build remote os_nt cpu_386 par

tree_depth = 4

host_os  = nt
host_cpu = 386

proj_name = dbgport

dbgport_autodepends = .AUTODEPEND

additional_clean = dbgport.sys

suppress_zc = 1

!include cproj.mif
!include defrule.mif
!include deftarg.mif

extra_cpp_flags = -D_X86_=1 -Di386=1
extra_c_flags = -zp8
inc_dirs = -I. -I"../h" -I"$(w32api_dir)/nt/h/ddk"

ddk_lib_dir = $(w32api_dir)/nt386/lib/ddk

.c : ../c

dbgport.sys : dbgport.obj $(__MAKEFILES__)
    $(noecho)%create dbgport.lnk
    @%append dbgport.lnk system nt
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
    @%append dbgport.lnk lib $(ddk_lib_dir)/ntoskrnl.lib
    @%append dbgport.lnk lib $(ddk_lib_dir)/hal.lib
    @%append dbgport.lnk lib clib3r.lib
    @%make echo_link
    $(linker) name dbgport.sys @dbgport.lnk
