.extensions:
.extensions: .exe .obj .asm .c

exe_list = fact.exe mov.exe semi.exe abs.exe ctrl.exe

objs_fact = fact.obj factdrv.obj
objs_mov = mov.obj movdrv.obj
objs_semi = semi.obj factdrv.obj
objs_abs = abs.obj absdrv.obj
objs_ctrl = ctrl.obj ctrldrv.obj

link_cmd = wlink sys ntaxp file { $(objs_$*) } op quiet

all: $(exe_list) .symbolic

test.out: $(exe_list)
        if exist test.out del test.out
        wtouch test.out
        @for %i in ($(exe_list)) do %i >> test.out
        diff test.out good.out

fact.exe: $(objs_fact)
        $(link_cmd)

mov.exe: $(objs_mov)
        $(link_cmd)

semi.exe: $(objs_semi)
        $(link_cmd)

abs.exe: $(objs_abs)
        $(link_cmd)

ctrl.exe: $(objs_ctrl)
        $(link_cmd)

clean: .symbolic .explicit
        if exist *.exe del *.exe
        if exist *.o del *.o
        if exist *.obj del *.obj
        if exist *.err del *.err

.asm.obj:
        wasaxp -q $(as_opt) $[@

.c.obj:
        wccaxp -zq $[@
