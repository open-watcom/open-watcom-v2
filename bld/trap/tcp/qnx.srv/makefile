#pmake: build serv remote qnx 16 32 tcp x86

os = qnx
srv = tcp
cpu = x86

extra_objs = $(remote_objs)
extra_libs = $(watcom_dir)\lib\qnx\socket3r.lib
extra_incs = $(watcom_dir)\qnx_h

!include $(trap_dir)\serv.mif
