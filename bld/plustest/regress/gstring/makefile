plustest_name = gstring
!include ../environ.mif

!ifdef test1
test_cflags = -d2
alt_error=01
!else
test_cflags = -ox-d1
alt_error=00
!endif

test_cflags += -zq-I.

test_cflags_386 = $(test_cflags) -mf
test_cflags_i86 = $(test_cflags) -ml
test_cflags_axp = $(test_cflags)

stack_386=opt stack=8k
stack_i86=opt stack=4k
stack_axp=opt stack=8k

.c.obj:
    $(wpp_$(arch)) $[@ $(test_cflags_$(arch)) -fo=.obj

objs = &
    string.obj &
    strngrep.obj &
    strtest1.obj

test : .symbolic start_test strtest.$(exe)
    diff strtest.out strtest.chk
    @%make global
    %append $(log_file) PASS $(%__CWD__)

strtest.$(exe) : $(objs) strtest.lnk
    $(linker) @$]@ NAME $@
    $(run) $(exec_prefix)$@ >>strtest.out

start_test : .symbolic
    %create strtest.out
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav

strtest.lnk : makefile
    %create $^@
    @%append $^@ DEBUG line
    @%append $^@ $(lnk_$(arch))
    @%append $^@ $(stack_$(arch))
    @for %i in ( $(objs) ) do @%append $^@ FILE %i

save : .symbolic
    @if exist strtest.out cp strtest.out s$(arch)_$(alt_error).sav

global : .SYMBOLIC
    @%make common_clean
