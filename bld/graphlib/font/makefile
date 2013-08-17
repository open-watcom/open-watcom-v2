host_os  = dos
host_cpu = i86

objs = selfont.obj std2.obj roman.obj italic.obj

font.lib : graph.gh $(objs)
    $(librarian) -n -b font.lib -+selfont.obj -+std2.obj -+roman.obj -+italic.obj

!include ../graphics.mif
