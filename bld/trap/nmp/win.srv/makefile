#pmake: build serv remote win 16 32 nmp x86

gui = 1
os = win
srv = nmp
cpu = x86

extra_objs = $(remote_objs) nmpdos.obj
resource_file = serv.res

!include $(trap_dir)\serv.mif

serv.res : serv.rc $(trap_dir)\common\win\winserv.rc
