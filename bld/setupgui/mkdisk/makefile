#pmake/150: build os_nt os_dos os_os2 os_linux cpu_386

proj_name = mkdisk

mkdisk_autodepends = .autodepend

!ifeq %BUILD_PLATFORM dos386
ziplib = ../ziplib/osi386/ziplib.lib
zlib   = ../zlib/osi386/zlib.lib
!else ifeq %BUILD_PLATFORM nt386
ziplib = ../ziplib/osi386/ziplib.lib
zlib   = ../zlib/osi386/zlib.lib
!else ifeq %BUILD_PLATFORM os2386
ziplib = ../ziplib/osi386/ziplib.lib
zlib   = ../zlib/osi386/zlib.lib
!else ifeq %BUILD_PLATFORM linux386
ziplib = ../ziplib/osi386/ziplib.lib
zlib   = ../zlib/osi386/zlib.lib
!else
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
