#pmake: build serv remote nt 16 32 tcp x86

gui = 1
os = nt
srv = tcp
cpu = x86

extra_objs = $(remote_objs)
extra_libs = $(lang_root)\lib386\nt\wsock32.lib
# extra_libs = $(watcom_dir)\lib\tcpipdll.lib
# extra_incs = $(watcom_dir)\os220_h\tcpip

!include $(trap_dir)\serv.mif
