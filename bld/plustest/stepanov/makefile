plustest_name = stepanov

!include testenv1.mif

!ifdef test1
alt_error=01
test_flags = -d2-w0
!else ifdef test2
alt_error=02
test_flags = -ox-d1-w0-j
!else
alt_error=00
test_flags = -ox-d1-w0-otexanr
!endif

test_flags_i86 = -mm

stack_386=opt stack=16k
stack_i86=opt stack=8k
stack_axp=opt stack=16k

extra_run_oop_v1p5 = Max=15000 18 Matrix=1000 15 Complex=30000 50 Iterator=80000 17
extra_run_oop_v1p6 = Max=15000 49 Matrix=1000 13 Complex=30000 52 Iterator=80000 19
extra_run_oop_v1p7 = Max=15000 14 Matrix=1000 14 Complex=30000 83 Iterator=80000 19

oopacks = &
oop_v1p5.$(exe) &
oop_v1p6.$(exe) &
oop_v1p7.$(exe)

stepanovs = &
ste_v1p1.$(exe) &
ste_v1p2.$(exe)

exes = $(oopacks) $(stepanovs)

# Note: Fast machines may need larger iteration counts, otherwise the code could run
# "too fast" and finish at "zero" execution time.

!ifeq arch i86
test : .symbolic start clean start_test
!else
test : .symbolic start clean start_test test.lnk $(exes)
    diff $(%ERROR_FILE) stepanov.chk
!endif
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

start_test : .symbolic
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav
    @set ERROR_FILE=exec.out
    %create $(%ERROR_FILE)

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav

clean : .procedure
    @%make common_clean
