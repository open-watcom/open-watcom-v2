plustest_name = claslist
!include ../environ.mif

!ifdef test1
test_cflags = -d2~
alt_error=01
!else
test_cflags = -ox-d1
alt_error=00
!endif

test_cflags += -zq

test_cflags_386 = -mf
test_cflags_i86 = -mc
test_cflags_axp =

stack_386=opt stack=8k
stack_i86=opt stack=4k
stack_axp=opt stack=8k

.c.obj:
    $(wpp_$(arch)) $[@ $(test_cflags) $(test_cflags_$(arch)) -fo=.obj

objs = pcfistt.obj
    
test : .symbolic start_test pcfistt.$(exe)
    diff pcfistt.out pcfist.chk
    @%make global
    %append $(log_file) PASS $(%__CWD__)

pcfistt.$(exe) : $(objs) pcfistt.lnk
    $(linker) @$]@ name $@ file {$(objs)}
    $(run) $(exec_prefix)$@ >>pcfistt.out

start_test : .symbolic
    %create pcfistt.out
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav

pcfistt.lnk : makefile
    %create $@
    @%append $@ DEBUG line
    @%append $@ $(lnk_$(arch))
    @%append $@ $(stack_$(arch))
    
save : .symbolic
    @if exist pcfistt.out cp pcfistt.out s$(arch)_$(alt_error).sav

global : .procedure
    @%make common_clean
