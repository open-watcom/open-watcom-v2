#pmake/150: build os_nt os_dos os_os2 os_linux cpu_386

host_cpu = $(%BOOTSTRAP_CPU)
host_os  = $(%BOOTSTRAP_OS)

proj_name = mkdisk

mkdisk_autodepends = .autodepend

!ifeq host_cpu 386
ziplib_linux = ../ziplib/linux386/ziplib.lib
!ifdef ziplib_$(host_os)
ziplib = $(ziplib_$(host_os))
!else
ziplib = ../ziplib/osi386/ziplib.lib
!endif
zlib   = ../zlib/osi386/zlib.lib
!endif

bld_extra_incs = -I"$(libzip_dir)/lib" -I"$(zlib_dir)" -I"$(builder_dir)/h"

!include cproj.mif
!include deftarg.mif
!include defrule.mif

commands = mkexezip.exe uzip.exe splitfil.exe packsize.exe genmkfil.exe langdat.exe mkinf.exe

all : $(commands) .SYMBOLIC

.c.exe:
    $(bld_cl) -fe=$@ $< $(bld_cl_opts)

langdat.exe : langdat.c $(builder_dir)/c/utils.c

uzip.exe: microzip.c $(ziplib) $(zlib)
    $(bld_cl) -fe=$@ $< $(bld_cl_opts)
