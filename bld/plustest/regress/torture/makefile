plustest_name = torture
!include ../environ.mif

objs = &
    assert.obj &
    jamstr.obj &
    newlog.obj &
    ordkeyma.obj &
    pool.obj

!ifdef test1
test_cflags = -d2-j
alt_error=01
!else
test_cflags = -ox-d1-j
alt_error=00
!endif

test_cflags += -zq-w0-I. -fi=__ialias.h

test_cflags_386 = -mf
test_cflags_i86 = -ml -fi=noinline.h
test_cflags_axp =

stack_386=opt stack=16k
stack_i86=opt stack=8k
stack_axp=opt stack=16k

.cpp.obj:
    $(wpp_$(arch)) $[* $(test_cflags) $(test_cflags_$(arch)) -fo=.obj

test : .symbolic start_test mdarr.$(exe) diarr.$(exe)
    diff exec.out exec.chk
    @%make global
    %append $(log_file) PASS $(%__CWD__)

diarr.$(exe) : dim2arr.obj tort.lnk tort.lib
    $(linker) @tort.lnk NAME $^@ FILE $[@ LIB $]@
    @%append exec.out --------$^&--------
    $(run) $(exec_prefix)$@ >>exec.out

mdarr.$(exe) : mdarrtst.obj tort.lnk tort.lib
    $(linker) @tort.lnk NAME $^@ FILE $[@ LIB $]@
    @%append exec.out --------$^&--------
    $(run) $(exec_prefix)$@ >>exec.out
    @%append exec.out --------output--------
    cat output >>exec.out
    rm output

start_test : .symbolic
    %create exec.out
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav

tort.lib : tort.lbc $(objs)
    $(librarian_$(arch)) -q-n-b @$[@

tort.lbc : $(__MAKEFILES__)
    %create $^@
    @%append $^@ $^*.lib
    @for %i in ( $(objs) ) do @%append $^@ -+%i

tort.lnk : $(__MAKEFILES__)
    %create $^@
    @%append $^@ $(lnk_$(arch))
    @%append $^@ $(stack_$(arch))
    @%append $^@ $(ldebug_$(arch))

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav

global : .symbolic
    @%make common_clean
