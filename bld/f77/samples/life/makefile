fincpath        = $(%WATCOM)\src\fortran\os2
os2incpath      = $(%TOOLKIT)\c\os2h

.before
        set include=$(os2incpath)
        set finclude=$(fincpath)

.extensions:
.extensions: .exe .obj .for .fi .h

objs = life.obj maindriv.obj menuitem.obj initmenu.obj &
        about.obj timer.obj dotimer.obj chktimer.obj initbit.obj &
        blit.obj blitbit.obj array.obj getps.obj random.obj &
        initbd.obj redraw.obj repaint.obj relinq.obj rules.obj &
        filename.obj readpat.obj readapat.obj patmenu.obj pattern.obj &
        xorreg.obj mouse.obj setcap.obj xform.obj loadpat.obj &
        error.obj writpat.obj pmwin.obj

life.exe : $(objs) life.res $(__makefiles__)
        @%create tmp.lnk
        @for %i in ($(objs)) do @%append tmp.lnk file %i
        wlink op st=64k sys os2v2_pm lib os2386 @tmp
        rc life.res
        @del tmp.lnk

life.res : life.rc life.ico tiny.bmp small.bmp med.bmp &
           large.bmp huge.bmp rules.dlg about.dlg name.dlg
        rc -r life.rc

.for : $(fincpath)
.for.obj
        wfc386 $[* /nowarn /xline /d1 /ox /explicit
