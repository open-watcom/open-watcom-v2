host_os = dos
host_cpu = i86

libname = dummy

objs = selfont.obj std2.obj roman.obj italic.obj

font.lib : $(objs)
    $(librarian) -n -b font.lib -+selfont.obj -+std2.obj -+roman.obj -+italic.obj

!include ../graphics.mif
