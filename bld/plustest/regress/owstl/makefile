plustest_name = owstl
!include ../environ.mif

!ifdef test1
test_cflags_386 = -d2-fpd-xs-za
test_cflags_i86 = -d2-fpd-xs-za
test_cflags_axp = -d2-xs-za
alt_error=01
!else ifdef test2   # This test was using -xss, but that does not entirely work yet.
test_cflags_386 = -os-d1-3s-xs $(br_opt)
test_cflags_i86 = -os-d1-xs
test_cflags_axp = -os-d1-xs
alt_error=02
!else ifdef test3   # This test was using -xss, but that does not entirely work yet.
test_cflags_386 = -ot-d1-3s-xs-xr
test_cflags_i86 = -ot-d1-xs-xr
test_cflags_axp = -ot-d1-xs-xr
alt_error=03
!else ifdef test4   # This test was using -oe500 but that does not entirely work yet.
test_cflags_386 = -ox-3r-fpd-xs
test_cflags_i86 = -ox-fpd-xs
test_cflags_axp = -ox-xs
alt_error=04
!else ifdef test5
test_cflags_386 = -d2-xs $(br_opt)
test_cflags_i86 = -d2-xs
test_cflags_axp = -d2-xs
alt_error=05
!else
test_cflags_386 = -os-d2-xst
test_cflags_i86 = -os-d2-xst
test_cflags_axp = -os-d2-xst
alt_error=00
!endif

test_cflags_386 += -zq-wx-wcd433-fpi87-fp5-mf
test_cflags_i86 += -zq-wx-wcd433-fpi87-fp5-ml-zm
test_cflags_axp += -zq-wx-wcd433

stack_386=opt stack=32k
stack_i86=opt stack=8k
stack_axp=opt stack=16k

# to update add test names here:
template = &
        aaa$(ext)       &
        algn01$(ext)    &
        algm01$(ext)    &
        algs01$(ext)    &
        bitset01$(ext)  &
        clib01$(ext)    &
        cmplx01$(ext)   &
        deque01$(ext)   &
        deque02$(ext)   &
        fun01$(ext)     &
        iter01$(ext)    &
        lim01$(ext)     &
        list01$(ext)    &
        locale01$(ext)  &
        map01$(ext)     &
        map02$(ext)     &
        mem01$(ext)     &
        mmwin16$(ext)   &
        mmwindef$(ext)  &
        numer01$(ext)   &
        queue01$(ext)   &
        stack01$(ext)   &
        set01$(ext)     &
        set02$(ext)     &
        slist01$(ext)   &
        smtptr01$(ext)  &
        string01$(ext)  &
        string02$(ext)  &
        typetr01$(ext)  &
        unoset01$(ext)  &
        util01$(ext)    &
        vector01$(ext)  &


# the list of .exe targets
ext = .$(exe)
exes = $+ $(template) $-
# a list of names so lazy old me can type "wmake alg01" to 
# create and run algo01.exe without running the full suite
ext =
names = $+ $(template) $-

# define flags specific to tests here
extra_cflags_typetr01 = -za0x
extra_cflags_mmwin16 = -I"$(%WATCOM)/h/win" -dmmWin16
extra_cflags_mmwindef = -I"$(%WATCOM)/h/nt"
!ifeq arch i86
extra_cflags_mmwindef += -3
!endif

.cpp.obj :
    $(wpp_$(arch)) $[@ $(test_cflags_$(arch)) -e1000 $(extra_cflags_$[*) -fo=.obj

# this is my laziness bit...
$(names) : test.lnk .symbolic
    @%make $^*.obj
    $(linker) @test.lnk NAME $^*.exe FILE $^*.obj
    $(run) $^*.exe
    @echo PASS

!ifdef __OS2__
mmwindef.$(exe): .symbolic
    @%append test.out ----------$^*----------
!endif

.obj.$(exe):
    $(linker) @test.lnk NAME $@ FILE $[@
    @%append test.out ----------$^*----------
    $(run) $(exec_prefix)$@ >> test.out

# the main target environment.mif calls to run the test suite

!ifeq arch i86
test : .symbolic start_test
!else
test : .symbolic start_test test.lnk $(exes)
    diff test.out test.chk
!endif
    @%make global
    %append $(log_file) PASS $(%__CWD__)

start_test : .symbolic
    %create test.out
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav

test.lnk : makefile
    %create $^@
    @%append $^@ $(ldebug_$(arch))
    @%append $^@ $(lnk_$(arch))
    @%append $^@ $(stack_$(arch))

save : .symbolic
    @if exist test.out cp test.out s$(arch)_$(alt_error).sav

global : .SYMBOLIC
    @rm -f mmwin16.cpp mmwindef.cpp
    @%make common_clean

# ensures mmwindef.cpp and mmwin16.cpp are always identical to 
# nominmax.cpp

mmwin16.cpp : nominmax.cpp
    cp nominmax.cpp mmwin16.cpp

mmwindef.cpp : nominmax.cpp
    cp nominmax.cpp mmwindef.cpp

