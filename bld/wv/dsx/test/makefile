#pmake: os_dos cpu_i86 i86 intel

!include deftarg.mif

.EXTENSIONS:
.EXTENSIONS: .exe .exp .obj .3bj .c .asm

#platform = pharlap
platform = dos4g

all : debugger.exe task.exe .SYMBOLIC

debugger.exe : debugger.obj dbgutil.obj dbgfork.obj
    wlink DEBUG all SYS dos OPT map,st=10k,q FILE debugger,dbgutil,dbgfork

task.exe : task.3bj
    wlink DEBUG all SYS $(platform) OPT map,st=10k,q FILE task.3bj

.c.obj :
    wcc.exe $[@ -zq -w4 -ms -ox -d2 -bt=dos
.c.3bj :
    wcc386.exe $[@ -zq -w4 -mf -ox -d2 -bt=$(platform)  -fo=.3bj
.asm.obj :
    wasm -zq -ml -bt=dos $[@ -fo=$^@
