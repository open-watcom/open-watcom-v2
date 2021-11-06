plustest_name = except
# EXCEPTIONS TEST
#
# 95/09/14 -- J.W.Welch     -- added batch capability

!ifdef test1
test_flags = -d2-xst
alt_error=01
!else ifdef test2
test_flags = -d2-xss
alt_error=02
!else ifdef test3
test_flags = -ox-d1-xss
alt_error=03
!else
test_flags = -ox-d1-xst
alt_error=00
!endif

test_flags += -I"../positive/source"

test_flags_386 = -mf
test_flags_i86 = -mc
test_flags_axp =

dos_exec_chk_i86 = execdos.chk
dos_exec_chk_386 = execdos.chk

!include testenv.mif

!ifeq alt_error 00
add_br_ = $(br_opt)
!else ifeq alt_error 02
add_br_ = $(br_opt)
!endif

!ifdef %plustest_bad_br
no_br_excok = no
!endif

test_flags += $(add_br_$(no_br_$^*))

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

stack_386=opt stack=32k
stack_i86=opt stack=8k
stack_axp=opt stack=32k

test : .symbolic start $(batch_sym) test.lnk $(exes)
    $(noecho)%make common_set_result

!ifdef BATCH_TEST
$(batch_lnk) : $(__MAKEFILES__)
    $(noecho)%create $(batch_lnk)
    @for %i in ( $(objs) ) do %append $(batch_lnk) STARTLINK @test FILE %i ENDLINK

$(batch_cmp) : $(__MAKEFILES__)
    $(noecho)%create $(batch_cmp)
    @for %i in ( $(srcs) ) do %append $(batch_cmp) %i

batch_build : .symbolic test.lnk $(batch_lnk) $(batch_cmp)
    @%make set_path_wpp
    $(wpp_$(arch)) -fc=$(batch_cmp) $(test_flags) $(test_flags_$(arch))
    @%make set_path_wlink
    @%make echo_link
    $(noecho)$(linker) @$(batch_lnk)
    @%make set_old_path
!endif

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean
!ifdef BATCH_TEST
    @if exist *.fcb $(noecho)rm *.fcb
    @if exist *.fcl $(noecho)rm *.fcl
!endif

!ifdef BATCH_TEST
.before
#    @echo test_flags=$(test_flags_$(arch))
!endif
