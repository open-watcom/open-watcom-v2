plustest_name = stepanov
!include ../environ.mif


oopacks = &
oop_v1p5.$(exe) &
oop_v1p6.$(exe) &
oop_v1p7.$(exe)

stepanovs = &
ste_v1p1.$(exe) &
ste_v1p2.$(exe)

exes = $(oopacks) $(stepanovs)

!ifdef test1
alt_error=01
test_flags = -d2-w0-zq
!else ifdef test2
alt_error=02
test_flags = -ox-d1-zq-w0-j
!else
alt_error=00
test_flags = -ox-d1-zq-w0-otexanr
!endif

test_flags_i86 = -mm

stack_386=opt stack=16k
stack_i86=opt stack=8k
stack_axp=opt stack=16k

.c.obj:
    $(wpp_$(arch)) $[@ $(test_flags) $(test_flags_$(arch)) $(opt_$^*) -fo=.obj

# Note: Fast machines may need larger iteration counts, otherwise the code could run
# "too fast" and finish at "zero" execution time.

.obj.$(exe):
    $(linker) @stepanov.lnk NAME $@ FILE $[@
    $(run) $(exec_prefix)$@ 2500 >>stepanov.out

oop_v1p5.$(exe) : oop_v1p5.obj
    $(linker) @stepanov.lnk NAME $@ FILE $[@
    $(run) $(exec_prefix)$@ Max=15000 18 Matrix=1000 15 Complex=30000 50 Iterator=80000 17 >>stepanov.out

oop_v1p6.$(exe) : oop_v1p6.obj
    $(linker) @stepanov.lnk NAME $@ FILE $[@
    $(run) $(exec_prefix)$@ Max=15000 49 Matrix=1000 13 Complex=30000 52 Iterator=80000 19 >>stepanov.out

oop_v1p7.$(exe) : oop_v1p7.obj
    $(linker) @stepanov.lnk NAME $@ FILE $[@
    $(run) $(exec_prefix)$@ Max=15000 14 Matrix=1000 14 Complex=30000 83 Iterator=80000 19 >>stepanov.out

!ifeq arch i86
test : .symbolic start_test
!else
test : .symbolic start_test stepanov.lnk $(exes)
    diff stepanov.out stepanov.chk
!endif
    @%make global
    %append $(log_file) PASS $(%__CWD__)

start_test : .symbolic
    %create stepanov.out
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav

stepanov.lnk : $(__MAKEFILES__)
    %create $^@
    @%append $^@ $(ldebug_$(arch))
    @%append $^@ $(lnk_$(arch))
    @%append $^@ $(stack_$(arch))

save : .symbolic
    @if exist stepanov.out cp stepanov.out s$(arch)_$(alt_error).sav

global : .SYMBOLIC
    @%make common_clean
