plustest_name = cool
!include ../../environ.mif

exes = &
    test_as1.$(exe) &
    test_ass.$(exe) &
    test_avl.$(exe) &
    test_big.$(exe) &
    test_bit.$(exe) &
    test_bt.$(exe) &
    test_cha.$(exe) &
    test_com.$(exe) &
    test_dt.$(exe) &
    test_gs.$(exe) &
    test_has.$(exe) &
    test_lis.$(exe) &
    test_mat.$(exe) &
    test_nod.$(exe) &
    test_nt.$(exe) &
    test_pai.$(exe) &
    test_pro.$(exe) &
    test_que.$(exe) &
    test_ran.$(exe) &
    test_rat.$(exe) &
    test_reg.$(exe) &
    test_rng.$(exe) &
    test_set.$(exe) &
    test_sta.$(exe) &
    test_str.$(exe) &
    test_tim.$(exe) &
    test_val.$(exe) &
    test_vec.$(exe) &
!ifneq arch i86       # Eager template instantiation causes problems for i86.
    test_han.$(exe) &
    test_ite.$(exe) &
    test_m_v.$(exe) &
    test_qua.$(exe) &
!endif

!ifdef test1
test_cflags = -op-d2
alt_error=01
!else ifdef test2
test_cflags = -ot-op-d1-xds
alt_error=02
!else ifdef test3
test_cflags = -op-d2-xds
alt_error=03
!else
test_cflags = -ot-op-d1
alt_error=00
!endif

test_cflags += -zq -I../.. -w0
!ifdef __UNIX__
test_cflags += -fi=../__ialias.h
!endif

test_cflags_i86 = -ml -zm

stack_386=opt stack=32k
stack_i86=opt stack=24k
stack_axp=opt stack=32k

.obj.$(exe):
    $(linker) @ex.lnk name $@ file $[@
    %append test.out $^&
    $(run) $(exec_prefix)$@ >>test.out

.c.obj:
    $(wpp_$(arch)) $[@ $(test_cflags) $(test_cflags_$(arch)) -fo=.obj

test : .symbolic start_test ex.lnk $(exes)
!ifeq arch i86
    diff test.out testi86.chk
!else
    diff test.out test.chk
!endif
    @%make global
    %append $(log_file) PASS $(%__CWD__)

start_test : .symbolic
    %create test.out
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav

ex.lnk : makefile
    %create $^@
    @%append $^@ $(lnk_$(arch))
    @%append $^@ $(ldebug_$(arch))
    @%append $^@ $(stack_$(arch))
    @%append $^@ LIBRARY ../cool.lib

save : .symbolic
    @if exist test.out cp test.out s$(arch)_$(alt_error).sav

global : .procedure
    @%make common_clean
