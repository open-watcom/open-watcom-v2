#pmake: build serv remote win 16 32 net x86

gui = 1
os = win
srv = net
cpu = x86

extra_objs = $(remote_objs)
resource_file = serv.res

!include $(trap_dir)\serv.mif

serv.res : serv.rc $(trap_dir)\common\win\winserv.rc
