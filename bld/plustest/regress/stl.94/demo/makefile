plustest_name = stl94
!include ../../environ.mif

objs = demo.obj tempbuf.obj

!ifdef test1
test_cflags = -oxa-d1-hd
alt_error=01
!else ifdef test2
test_cflags = -oxa-d1-xs-hd
alt_error=02
!else ifdef test3
test_cflags = -d2-xs-hd
alt_error=03
!else
test_cflags = -d2-hd
alt_error=00
!endif

test_cflags += -w0-zq -I..

test_cflags_i86 = -mm -zm

stack_386=opt stack=128k
stack_i86=opt stack=32k
stack_axp=opt stack=128k

.cpp: .;..
.cpp.obj:
    $(wpp_$(arch)) $[@ $(test_cflags) $(test_cflags_$(arch)) -fo=.obj

#
# function code overflow 16-bit segment
# don't use it for i86 target
#
!ifeq arch i86
test : .symbolic start_test
!else
test : .symbolic start_test demo.$(exe)
    diff demo.out demo.chk
!endif
    @%make global
    %append $(log_file) PASS $(%__CWD__)

demo.$(exe) : $(objs) demo.lnk
    $(linker) @$]@ NAME $@
    $(run) $(exec_prefix)$@ <demo.in >>demo.out

start_test : .symbolic
    %create demo.out
    @if exist s$(arch)_$(alt_error).sav rm -f s$(arch)_$(alt_error).sav

demo.lnk : $(__MAKEFILES__)
    %create $^@
    @%append $^@ $(ldebug_$(arch))
    @%append $^@ $(lnk_$(arch))
    @%append $^@ $(stack_$(arch))
    @for %i in ( $(objs) ) do @%append $^@ FILE %i

save : .symbolic
    @if exist demo.out cp demo.out s$(arch)_$(alt_error).sav

global : .symbolic
    @%make common_clean
