plustest_name = exercise
!include ../environ.mif

exes = &
    chkcl.$(exe) &
    ctordtor.$(exe) &
    mp.$(exe) &
    ovldall.$(exe) &
    switcher.$(exe) &

!ifdef test1
test_cflags += -d2-xds
alt_error=01
!else ifdef test2
test_cflags += -d2-xss
alt_error=02
!else ifdef test3
test_cflags += -d2-xdt
alt_error=03
!else ifdef test4
test_cflags += -d2-xst
alt_error=04
!else ifdef test5
test_cflags += -ox-d1-xds
alt_error=05
!else ifdef test6
test_cflags += -ox-d1-xss
alt_error=06
!else ifdef test7
test_cflags += -ox-d1-xdt
alt_error=07
!else ifdef test8
test_cflags += -d2-xs
add_br_ = $(br_opt)
alt_error=08
!else ifdef test9
test_cflags += -ox-d1-xs
add_br_ = $(br_opt)
alt_error=09
!else
test_cflags += -ox-d1-xst
alt_error=00
!endif

test_cflags += -zq-w0

!ifdef %plustest_bad_br
no_br_ctordtor = no
no_br_switcher = no
!endif

!ifeq arch 386
test_cflags += $(add_br_$(no_br_$^*))
!endif

stack_386=opt stack=16k
stack_i86=opt stack=8k
stack_axp=opt stack=16k

.cpp.obj:
    $(wpp_$(arch)) $[@ $(test_cflags) -fo=.obj

.obj.$(exe):
    $(linker) @test.lnk name $@ FILE $[@
    $(run) $(exec_prefix)$@ >>exercise.out

test : .symbolic start_test test.lnk $(exes)
    diff exercise.out exercise.chk
    @%make global
    %append $(log_file) PASS $(%__CWD__)

start_test : .symbolic
    %create exercise.out
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav

test.lnk : $(__MAKEFILES__)
    %create $^@
    @%append $^@ DEBUG line
    @%append $^@ $(lnk_$(arch))
    @%append $^@ $(stack_$(arch))

save : .symbolic
    @if exist exercise.out cp exercise.out s$(arch)_$(alt_error).sav

global : .SYMBOLIC
    @%make common_clean
