#pmake: build serv lcl dos dsx 32 x86

name = pedhelp
sys_x86_pls = sys tnt op st=8K

objs_pls = plsord.obj   # must be first

extra_linker = &
        op offset=0, objalign=4k &
        alias __DOSseg__=main_ &
        lib $(plsdir)\wdos32,$(plsdir)\wdosx32 &
        file $(plsdir)\dilseg

!include ..\dos.srv\makefile

plsord.obj : plsord.asm
        wasm -zq $[@
