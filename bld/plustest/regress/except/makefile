plustest_name = except
# EXCEPTIONS TEST
#
# 95/09/14 -- J.W.Welch     -- added batch capability

!include ../environ.mif

files = &
    excok.$(ext) &
    excfancy.$(ext) &
    excalloc.$(ext) &
    excdtor.$(ext) &
    excrt_01.$(ext) &
    excrt_02.$(ext) &
    excrt_03.$(ext) &
    excrt_04.$(ext) &
    excrt_05.$(ext) &
    excrt_06.$(ext) &
    excrt_07.$(ext) &
    excrt_08.$(ext) &
    excrt_09.$(ext) &
    excrt_10.$(ext) &
    excrt_11.$(ext)

ext = $(exe)
exes = $+ $(files) $-

!ifdef BATCH_TEST
ext = c
srcs = $+ $(files) $-

ext = obj
objs = $+ $(files) $-

batch_cmp = except.fcb
batch_lnk = except.fcl

batch_sym = batch_build
!endif

test_cflags += -I"../positive/source"

!ifdef test1
test_cflags += -d2-xst
alt_error=01
!else ifdef test2
test_cflags += -d2-xss
add_br_ = $(br_opt)
alt_error=02
!else ifdef test3
test_cflags += -ox-d1-xss
alt_error=03
!else
test_cflags += -ox-d1-xst
add_br_ = $(br_opt)
alt_error=00
!endif

!ifdef %plustest_bad_br
no_br_excok = no
!endif

!ifeq arch 386
test_cflags_386 += $(test_cflags) -mf $(add_br_$(no_br_$^*))
!else ifeq arch i86
test_cflags_i86 += $(test_cflags) -mc
!else ifeq arch axp
test_cflags_axp += $(test_cflags)
!endif

stack_386=opt stack=32k
stack_i86=opt stack=8k
stack_axp=opt stack=32k

!ifneq targ_env_386 cw386
!ifneq targ_env_386 d4g386
fatal_error_check_386=| diff - fatalerr.chk
!endif
!endif
fatal_error_check_axp=| diff - fatalerr.chk

.c.obj:
    $(wpp_$(arch)) $[* $(test_cflags_$(arch)) -zq -fo=.obj

.obj.$(exe):
    $(linker) @test.lnk name $@ file $[@
    @%append exec.out --------$^*--------
!ifdef fatal_error_check_$(arch)
    -$(run) $(exec_prefix)$@ >>exec.out 2>&1
!else
    -$(run) $(exec_prefix)$@ >>exec.out
!endif

test : .symbolic start_test $(batch_sym) test.lnk $(exes)
    diff exec.out except.chk $(fatal_error_check_$(arch))
    @%make global
    %append $(log_file) PASS $(%__CWD__)

start_test : .symbolic
    %create exec.out
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav

!ifdef BATCH_TEST
$(batch_lnk) : $(__MAKEFILES__)
    %create $(batch_lnk)
    @for %i in ( $(objs) ) do %append $(batch_lnk) STARTLINK @test FILE %i ENDLINK

$(batch_cmp) : $(__MAKEFILES__)
    %create $(batch_cmp)
    @for %i in ( $(srcs) ) do %append $(batch_cmp) %i

batch_build : .symbolic test.lnk $(batch_lnk) $(batch_cmp)
    $(wpp_$(arch)) -fc=$(batch_cmp) $(test_cflags_$(arch)) 
    $(linker) @$(batch_lnk)
!endif


test.lnk : $(__MAKEFILES__)
    %create $^@
    @%append $^@ $(ldebug_$(arch))
    @%append $^@ $(lnk_$(arch))
    @%append $^@ $(stack_$(arch))
    @%append $^@ OPTION undefsok

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav

global : .procedure
    @%make common_clean
!ifdef BATCH_TEST
    @if exist *.fcb rm *.fcb
    @if exist *.fcl rm *.fcl
!endif

!ifdef BATCH_TEST
.before
    @echo test_cflags=$(test_cflags_$(arch))
!endif
