plustest_name = stepanov

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

!include testenv.mif

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
test : .symbolic start
    $(noecho)%append $(log_file) PASS $(%__CWD__)
    @%make clean
!else
test : .symbolic start test.lnk $(exes)
    $(noecho)%make common_set_result_display
!endif

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean
