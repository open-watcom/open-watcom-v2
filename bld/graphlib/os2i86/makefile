#pmake: build os_os2 cpu_i86

host_os  = dos
host_cpu = i86

extra_cpp_flags_dos = -DDOSX286
extra_c_flags_seginit = -zld

build: graph.gh seginit.obj .symbolic

!include $(graphics_prefix)../graphics.mif
