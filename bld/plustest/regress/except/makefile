plustest_name = except
# EXCEPTIONS TEST
#
# 95/09/14 -- J.W.Welch     -- added batch capability

!include ..\environ.mif

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
    
cflags += -i..\positive\source
    
!ifdef test1
cflags += -d2-xst
!else ifdef test2
cflags += -d2-xss
add_br_ = -br
!else ifdef test3
cflags += -ox-d1-xss
!else
cflags += -ox-d1-xst
add_br_ = -br
!endif

!ifdef %plustest_bad_br
no_br_excok = no
!endif

!ifeq arch 386
cflags_386 += $(cflags) -mf $(add_br_$(no_br_$^*))
!else ifeq arch i86
cflags_i86 += $(cflags) -mc
!else ifeq arch axp
cflags_axp += $(cflags)
!endif

.c.obj:
    $(wpp_$(arch)) $[* $(cflags_$(arch)) -zq
    
.obj.$(exe):
    $(linker) @test file $[@
    
test : .symbolic $(batch_sym) run_test
    %make global
    %append $(log_file) PASS $(%__CWD__)
    
run_test : .symbolic $(batch_sym) test.lnk $(exes)
    %create except.out
    @for %i in ( $(exes) ) do -$(run) %i >>except.out
    diff except.out except.chk
    
!ifdef BATCH_TEST
$(batch_lnk) : $(__MAKEFILES__)
    %create $(batch_lnk)
    @for %i in ( $(objs) ) do %append $(batch_lnk) STARTLINK @test FILE %i ENDLINK

$(batch_cmp) : $(__MAKEFILES__)
    %create $(batch_cmp)
    @for %i in ( $(srcs) ) do %append $(batch_cmp) %i

batch_build : .symbolic test.lnk $(batch_lnk) $(batch_cmp)
    $(wpp_$(arch)) -fc=$(batch_cmp) $(cflags_$(arch)) 
    $(linker) @$(batch_lnk)
!endif
    
    
test.lnk : $(__MAKEFILES__)
    %create $^@
    @%append $^@ OPTION quiet, map, stack=32k
    @%append $^@ $(ldebug_$(arch))
    @%append $^@ $(lnk_$(arch))
    @%append $^@ OPTION undefsok
    
global : .symbolic
    @%make common_clean
!ifdef BATCH_TEST
    @if exist *.fcb del *.fcb
    @if exist *.fcl del *.fcl
!endif
    
!ifdef BATCH_TEST
.before
    @echo cflags=$(cflags_$(arch))
!endif
