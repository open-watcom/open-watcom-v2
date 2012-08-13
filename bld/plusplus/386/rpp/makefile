#pmake: nobuild

!ifeq %BUILD_PLATFORM dos386
host_os      = dos
host_cpu     = 386
!else ifeq %BUILD_PLATFORM os2386
host_os      = os2
host_cpu     = 386
plusplus_dll = 1
!else ifeq %BUILD_PLATFORM nt386
host_os      = nt
host_cpu     = 386
plusplus_dll = 1
!else ifeq %BUILD_PLATFORM ntaxp
host_os      = nt
host_cpu     = axp
plusplus_dll = 1
!else ifeq %BUILD_PLATFORM linux386
host_os      = linux
host_cpu     = 386
!endif

reference = 1

!include ../targ_386.mif
