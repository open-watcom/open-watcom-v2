#pmake: build os_nt cpu_386 cpu_axp cpu_ppc

additional_cleanup = include/*.h include/GL/*.h include/ddk/*.h include/directx/*.h

!include cproj.mif
!include deftarg.mif

all: .symbolic
        @for %i in (../include/*.h) do @wmake -h -f ../fix/new_h.mif file=%i
        @for %i in (../include/GL/*.h) do @wmake -h -f ../fix/new_h.mif file=%i
        @for %i in (../include/ddk/*.h) do @wmake -h -f ../fix/new_h.mif file=%i
        @for %i in (../include/directx/*.h) do @wmake -h -f ../fix/new_h.mif file=%i
        $(vi) -s ../fix/fix_h.vi
