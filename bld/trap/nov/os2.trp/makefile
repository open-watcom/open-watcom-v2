#pmake: build trap remote os2 16 32 nov x86

os = os2
srv = nov
cpu = x86

extra_objs = $(remote_objs) novos2.obj
extra_incs = $(nwos2dir)
extra_libs = $(nwos2dir)\ipxcalls.lib $(nwos2dir)\nwcallsp.lib $(nwos2dir)\spxcalls.lib

!include $(trap_dir)\trap.mif
