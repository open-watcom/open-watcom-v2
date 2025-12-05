plustest_name = getlong

!ifdef test1
test_flags = -d2
alt_error=01
!else
test_flags = -ox-d1
alt_error=00
!endif

!include testenv.mif

stack_386=opt stack=8k
stack_i86=opt stack=4k
stack_axp=opt stack=8k

objs = &
    getlongo.obj &
    readme.obj

exes = ./opt.$(exe)

test : .SYMBOLIC start $(exes)
    $(noecho)%make common_set_result

./opt.$(exe) : $(objs)
    @set ERROR_MSG=failure to link $^*
    @%make set_path_wlink
    @%make echo_link
    @%make test.lnk
    @which $(linker)
    $(noecho)$(linker) @test.lnk NAME $@ file {$(objs)}
    @set ERROR_MSG=failure to run $^*
    @%make set_path_run
    $(run) $@ >>$(%ERROR_FILE)
    $(run) $@ -h >>$(%ERROR_FILE)
    -$(run) $@ -hopeless >>$(%ERROR_FILE)
    $(run) $@ -vers >>$(%ERROR_FILE)
    $(run) $@ -v >>$(%ERROR_FILE)
    -$(run) $@ -o >>$(%ERROR_FILE)
    -$(run) $@ -v=1 >>$(%ERROR_FILE)
    @%make set_old_path

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean
