plustest_name = yacl
!include ../../environ.mif

exes = &
bittest.$(exe) &
btreetst.$(exe) &
bytstrm.$(exe) &
dkbtrtst.$(exe) &
format1.$(exe) &
gseqtest.$(exe) &
holidays.$(exe) &
maptest.$(exe) &
perstest.$(exe) &
seqtest.$(exe) &
settest.$(exe) &
slotftst.$(exe) &
strset.$(exe) &
strtest.$(exe) &
treeprnt.$(exe) &
treetest.$(exe) &
wordcnt.$(exe)

!ifdef test1
test_cflags = -ot-op-d1
alt_error=01
!else ifdef test2
test_cflags = -ot-op-d1-xds
alt_error=02
!else ifdef test3
test_cflags = -op-d2-xds
alt_error=03
!else
test_cflags = -op-d2
alt_error=00
!endif

test_cflags += -ei -zq -I.. -w0 -D__OS2__

test_cflags_i86 = -ml

stack_386=opt stack=32k
stack_i86=opt stack=24k
stack_axp=opt stack=32k

.obj.$(exe):
    $(linker) @ex.lnk NAME $@ FILE $[@ op verbose
    @%append test.out --------$^&--------
    $(run) $(exec_prefix)$@ >>test.out

.cpp.obj:
    $(wpp_$(arch)) $[@ $(test_cflags) $(test_cflags_$(arch)) -fo=.obj

format1.$(exe) : format1.obj
    $(linker) @ex.lnk NAME $@ FILE $[@ op verbose
    @%append test.out --------$^&--------
    $(run) $(exec_prefix)$@ <format1.cpp >>test.out

holidays.$(exe) : holidays.obj
    $(linker) @ex.lnk NAME $@ FILE $[@ op verbose
    @%append test.out --------$^&--------
    $(run) $(exec_prefix)$@ 1999 >>test.out

wordcnt.$(exe) : wordcnt.obj
    $(linker) @ex.lnk NAME $@ FILE $[@ op verbose
    @%append test.out --------$^&--------
    $(run) $(exec_prefix)$@ <wordcnt.cpp >>test.out

dkbtrtst.$(exe) : dkbtrtst.obj
    $(linker) @ex.lnk NAME $@ FILE $[@ op verbose
    @%append test.out --------$^&--------
    $(run) $(exec_prefix)$@ >>test.out
    @%append test.out --------$^&--------
    $(run) $(exec_prefix)$@ >>test.out

perstest.$(exe) : perstest.obj
    $(linker) @ex.lnk NAME $@ FILE $[@ op verbose
    @%append test.out --------$^&--------
    $(run) $(exec_prefix)$@ >>test.out
    @%append test.out --------$^&--------
    $(run) $(exec_prefix)$@ >>test.out

test : .symbolic start_test ex.lnk $(exes)
    @if exist *.dat rm *.dat
    rm ../base.lib
    rm ../io.lib
    diff test.out test.chk
    @%make global
    %append $(log_file) PASS $(%__CWD__)

start_test : .symbolic
    %create test.out
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav

ex.lnk : makefile
    %create $^@
    @%append $^@ $(lnk_$(arch))
    @%append $^@ $(stack_$(arch))
    @%append $^@ $(ldebug_$(arch))
    @%append $^@ LIBRARY ../base.lib, ../io.lib

save : .symbolic
    @if exist test.out cp test.out s$(arch)_$(alt_error).sav

global : .symbolic
    @if exist *.dat rm *.dat
    @%make common_clean
