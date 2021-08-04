plustest_name = cool

!ifdef test1
test_flags = -op-d2
alt_error=01
!else ifdef test2
test_flags = -ot-op-d1-xds
alt_error=02
!else ifdef test3
test_flags = -op-d2-xds
alt_error=03
!else
test_flags = -ot-op-d1
alt_error=00
!endif

test_flags += -I.. -I"../source" -w0 -fi=../__ialias.h

test_flags_i86 = -ml -zm

exec_chk_i86 = execi86.chk

!include testenv.mif
!include ../lib.mif

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

stack_386=opt stack=32k
stack_i86=opt stack=24k
stack_axp=opt stack=32k

opt_link = LIBRARY ../cool.lib

.cpp : .;../source

test : .symbolic start ../cool.lib test.lnk $(exes)
    $(noecho)%make common_set_result

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean
    @if exist ../cool.lib $(noecho)rm ../cool.lib
