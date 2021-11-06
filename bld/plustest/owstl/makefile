plustest_name = owstl

test_flags = -e1000

!ifdef test1
test_flags_386 = -d2-fpd-xs-za
test_flags_i86 = -d2-fpd-xs-za
test_flags_axp = -d2-xs-za
alt_error=01
!else ifdef test2   # This test was using -xss, but that does not entirely work yet.
test_flags_386 = -os-d1-3s-xs
test_flags_i86 = -os-d1-xs
test_flags_axp = -os-d1-xs
alt_error=02
!else ifdef test3   # This test was using -xss, but that does not entirely work yet.
test_flags_386 = -ot-d1-3s-xs-xr
test_flags_i86 = -ot-d1-xs-xr
test_flags_axp = -ot-d1-xs-xr
alt_error=03
!else ifdef test4   # This test was using -oe500 but that does not entirely work yet.
test_flags_386 = -ox-3r-fpd-xs
test_flags_i86 = -ox-fpd-xs
test_flags_axp = -ox-xs
alt_error=04
!else ifdef test5
test_flags_386 = -d2-xs
test_flags_i86 = -d2-xs
test_flags_axp = -d2-xs
alt_error=05
!else
test_flags_386 = -os-d2-xst
test_flags_i86 = -os-d2-xst
test_flags_axp = -os-d2-xst
alt_error=00
!endif

test_flags_386 += -wx-wcd433-fpi87-fp5-mf
test_flags_i86 += -wx-wcd433-fpi87-fp5-ml-zm
test_flags_axp += -wx-wcd433

!include testenv.mif

!ifeq alt_error 02
test_flags += $(br_opt)
!else ifeq alt_error 05
test_flags += $(br_opt)
!endif

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
        fwrd01$(ext)    &
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
opt_lim01 = -ze
opt_typetr01 = -za0x
opt_mmwin16 = $(incl_sys_win) -dmmWin16
opt_mmwindef = $(incl_sys_nt)
!ifeq arch i86
opt_mmwindef += -3
!endif

# the main target to run the test suite

!ifeq arch i86
test : .symbolic start
    $(noecho)%append $(log_file) PASS $(%__CWD__)
    @%make clean
!else
test : .symbolic start test.lnk $(exes)
    $(noecho)%make common_set_result_display
!endif

# this is my laziness bit...
$(names) : test.lnk .symbolic
    @%make $^*.obj
    @%make set_path_wlink
    @%make echo_link
    $(noecho)$(linker) @test.lnk NAME $^*.exe FILE $^*.obj
    @%make set_run_path
    $(run) $(exec_prefix)$^*.exe
    @%make set_old_run_path
    @echo PASS

!ifdef __OS2__
mmwindef.$(exe): .symbolic
    @%append $(%ERROR_FILE) ----------$^*----------
!endif

# ensures mmwindef.cpp and mmwin16.cpp are always identical to
# nominmax.cpp

mmwin16.cpp : nominmax.cpp
    copy nominmax.cpp mmwin16.cpp

mmwindef.cpp : nominmax.cpp
    copy nominmax.cpp mmwindef.cpp

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean
    @if exist mmwin16.cpp $(noecho)rm -f mmwin16.cpp
    @if exist mmwindef.cpp $(noecho)rm -f mmwindef.cpp
