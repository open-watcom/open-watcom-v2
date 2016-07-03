host_os  = dos
host_cpu = i86

objs = selfont.obj std2.obj roman.obj italic.obj

font.lib : graph.gh $(objs) $(__MAKEFILES__)
    $(librarian) -n -b font.lib $(objs)

!include ../graphics.mif
