plustest_name = except
# EXCEPTIONS TEST
#
# 95/09/14 -- J.W.Welch     -- added batch capability

!include testenv.mif

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

test_flags += -I"../positive/source"

!ifdef test1
test_flags += -d2-xst
alt_error=01
!else ifdef test2
test_flags += -d2-xss
add_br_ = $(br_opt)
alt_error=02
!else ifdef test3
test_flags += -ox-d1-xss
alt_error=03
!else
test_flags += -ox-d1-xst
add_br_ = $(br_opt)
alt_error=00
!endif

!ifdef %plustest_bad_br
no_br_excok = no
!endif

!ifeq arch 386
test_flags_386 += $(test_flags) -mf $(add_br_$(no_br_$^*))
!else ifeq arch i86
test_flags_i86 += $(test_flags) -mc
!else ifeq arch axp
test_flags_axp += $(test_flags)
!endif

stack_386=opt stack=32k
stack_i86=opt stack=8k
stack_axp=opt stack=32k

!ifneq targ_env_386 cw386
!ifneq targ_env_386 d4g386
stderr_check_386 = 2>>stderr.out
!endif
!endif
stderr_check_axp = 2>>stderr.out

test : .symbolic start clean start_test $(batch_sym) test.lnk $(exes)
    diff $(%ERROR_FILE) exec.chk
!ifdef stderr_check_$(arch)
    diff stderr.out stderr.chk
!endif
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

start_test : .symbolic
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav
    @if exist x$(arch)_$(alt_error).sav rm x$(arch)_$(alt_error).sav
    @set ERROR_FILE=exec.out
    %create $(%ERROR_FILE)
!ifdef stderr_check_$(arch)
    %create stderr.out
!endif

!ifdef BATCH_TEST
$(batch_lnk) : $(__MAKEFILES__)
    %create $(batch_lnk)
    @for %i in ( $(objs) ) do %append $(batch_lnk) STARTLINK @test FILE %i ENDLINK

$(batch_cmp) : $(__MAKEFILES__)
    %create $(batch_cmp)
    @for %i in ( $(srcs) ) do %append $(batch_cmp) %i

batch_build : .symbolic test.lnk $(batch_lnk) $(batch_cmp)
    @%make set_path_wpp
    $(wpp_$(arch)) -fc=$(batch_cmp) $(test_flags_$(arch)) 
    @%make set_path_wlink
    $(linker) @$(batch_lnk)
    @%make set_old_path
!endif

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav
    @if exist stderr.out cp stderr.out x$(arch)_$(alt_error).sav

clean : .procedure
    @%make common_clean
!ifdef BATCH_TEST
    @if exist *.fcb rm *.fcb
    @if exist *.fcl rm *.fcl
!endif

!ifdef BATCH_TEST
.before
    @echo test_flags=$(test_flags_$(arch))
!endif
