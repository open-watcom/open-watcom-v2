#pmake/150: build os_nt os_dos os_os2 os_linux cpu_386

proj_name = mkdisk

mkdisk_autodepends = .autodepend

!ifeq %BOOTSTRAP_CPU 386
!ifeq %BOOTSTRAP_OS dos
ziplib = ../ziplib/osi386/ziplib.lib
zlib   = ../zlib/osi386/zlib.lib
!else ifeq %BOOTSTRAP_OS nt
ziplib = ../ziplib/osi386/ziplib.lib
zlib   = ../zlib/osi386/zlib.lib
!else ifeq %BOOTSTRAP_OS os2
ziplib = ../ziplib/osi386/ziplib.lib
zlib   = ../zlib/osi386/zlib.lib
!else ifeq %BOOTSTRAP_OS linux
ziplib = ../ziplib/linux386/ziplib.lib
zlib   = ../zlib/osi386/zlib.lib
!else
!endif
!endif

bld_extra_incs = -I"$(libzip_dir)/lib" -I"$(zlib_dir)" -I"$(builder_dir)/h"

!include cproj.mif
!include deftarg.mif
!include defrule.mif

commands = mkexezip.exe uzip.exe splitfil.exe packsize.exe genmkfil.exe langdat.exe mkinf.exe

all : $(commands) .SYMBOLIC

.c.exe:
    $(bld_cl) -fe=$@ $<

langdat.exe : langdat.c $(builder_dir)/c/utils.c

uzip.exe: microzip.c $(ziplib) $(zlib)
    $(bld_cl) -fe=$@ $<
