objs = selfont.obj std2.obj roman.obj italic.obj

font.lib : $(objs)
    wlib /n /b font -+selfont -+std2 -+roman -+italic

!include ..\o.s\makefile

selfont.obj : selfont.c
        wccp $(cflags) $[*
