#pmake: build serv remote win 16 32 vdm x86

gui = 1
os = win
srv = vdm
cpu = x86

extra_objs = $(remote_objs) vdmserv.obj
resource_file = serv.res

!include $(trap_dir)\serv.mif

serv.res : serv.rc $(trap_dir)\common\win\winserv.rc
