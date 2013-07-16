#pmake: build remote os_nt cpu_386 par

host_os  = nt
host_cpu = 386

!include cproj.mif
!include defrule.mif
!include deftarg.mif

all: dbginst.exe .SYMBOLIC

!ifeq release 0

debug_compiler=/Z7 /Odi
debug_linker=-debug:none -debugtype:both /pdb:none

!else

debug_compiler=
debug_linker=-debug:none

!endif

#
# Parallel port device driver (creates "\\.\DBGPORTn", where 'n' is 0, 1, 2).
#
# Note: This driver is built externally since it require Visual C++. See
#       the makefile.dmake for the SciTech dmake script that builds it.
#
#dbgport.sys : dbgport.obj
#       !$(msdev_dir)\bin\link -out:dbgport.sys -machine:i386           &
#           $(debug_linker)                             &
#           -merge:_page=page -merge:_text=.text        &
#           -section:INIT,d                             &
#           -opt:ref -incremental:NO -force:multiple    &
#           -release -fullbuild -nodefaultlib           &
#           -version:4.00 -osversion:4.00               &
#           -merge:.rdata=.text -optidata               &
#           -driver -align:0x20 -subsystem:native,4.00  &
#           -base:0x10000 -entry:DriverEntry@8          &
#           dbgport.obj                                 &
#           $(ddk_dir)\lib\i386\free\int64.lib          &
#           $(ddk_dir)\lib\i386\free\ntoskrnl.lib       &
#           $(ddk_dir)\lib\i386\free\hal.lib
#
#dbgport.obj : ..\c\dbgport.c
#       SET OPATH=$(%PATH)
#       SET PATH=$(msdev_dir)\bin
#       !cl -nologo -I. -I..\h -I$(ddk_dir)\inc -I$(msdev_dir)\include -D_X86_=1 -Di386=1 -DWIN32_LEAN_AND_MEAN=1 /c /Zel /Zp8 /Gy /cbstring /W3 /Gz /QIfdiv- /QIf /Gi- /Gm- /GX- /GR- /GF /Yd $(debug_compiler) /FI$(ddk_dir)\inc\warning.h ..\c\dbgport.c
#       SET PATH=$(%OPATH)

#
# Install the device driver
#
# "drvinst -dParPort -s2 -e1 DbgPort dbgport.sys" to install
# "drvinst -r DbgPort" to remove
#
#
.c : ../c

inc_dirs = -I"$(w32api_dir)/nt"

extra_cpp_flags = -D_WIN32 -DWIN32

dbginst.exe : dbginst.obj
        $(cl) $(clflags) $< $(ldflags)
