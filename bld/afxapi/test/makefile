.EXTENSIONS : .res .rc

afxapitest_name = test
!include environ.mif

opt_EH_yes = -xs

afxapitest_dir = source

!include $(afxapitest_dir)/test.mif

!ifdef test1
test_cflags_386 = -D_DEBUG
alt_error = 01
!else ifdef test2
test_cflags_386 = -D_UNICODE
alt_error = 02
!else ifdef test3
test_cflags_386 = -D_UNICODE -D_DEBUG
alt_error = 03
!else ifdef test4
test_cflags_386 = -D_AFXDLL
alt_error = 04
!else ifdef test5
test_cflags_386 = -D_AFXDLL -D_DEBUG
alt_error = 05
!else ifdef test6
test_cflags_386 = -D_AFXDLL -D_UNICODE
alt_error = 06
!else ifdef test7
test_cflags_386 = -D_AFXDLL -D_UNICODE -D_DEBUG
alt_error = 07
!else
alt_error = 00
!endif

test_cflags_386 += -we -w0 -zq -fh -D_CONSOLE $(inc_dirs_sys) -I"../nt"

.error
    @%append $(%ERROR_FILE) $(%ERROR_MSG)

.cpp: $(afxapitest_dir)
.cpp.obj:
    @set ERROR_MSG=failure to compile $@
    $(wpp_$(arch)) $[@ $(test_cflags_$(arch)) $(opt_$^*) $(opt_EH_$(EH_$^*)) -fo=.obj

.rc: $(afxapitest_dir)
.rc.res:
    @set ERROR_MSG=failure to compile $@
    wrc -q -r -fo=$@ $[@

.obj.$(exe):
    @set ERROR_MSG=failure to link $@
    @%make test.lnk
    $(linker) name $@ @test.lnk $(opt_link_$^*) file $[@
    @set ERROR_MSG=failure to execute $@
    @%append exec.out PASS executing $@
    $(run) $(exec_prefix)$@ >>exec.out
    rm $[@

test : .SYMBOLIC start_test $(test_exes)
    @set ERROR_FILE=error.out
    @set ERROR_MSG=failure to complete test stream
    %write error.out OK
    chk_exec exec.out >>error.out
    diff -b error.out error.chk
    @%make global
    %append $(log_file) PASS $(%__CWD__)

string04.$(exe) : string04.obj string04.res
    @set ERROR_MSG=failure to link $@
    @%make test.lnk
    $(linker) name $@ @test.lnk file $[@
    @set ERROR_MSG=failure to add resources to $@
    wrc -q $]@
    @set ERROR_MSG=failure to execute $@
    @%append exec.out PASS executing $@
    $(run) $(exec_prefix)$@ >>exec.out

start_test : .SYMBOLIC
    %create exec.out
    @set ERROR_FILE=exec.out
    @set PATH=..\dnt386.dll;..\dnt386u.dll;..\nt386.dll;..\nt386u.dll;$(%PATH)
    @if exist s$(arch)$(alt_error)$(host_bits).sav rm s$(arch)$(alt_error)$(host_bits).sav

test.lnk : makefile
    %create test.lnk
    @%append test.lnk $(ldebug_$(arch))
    @%append test.lnk $(lnk_$(arch))
    @%append test.lnk OPTION quiet, noredef
    @%append test.lnk LIBPATH ../dnt386
    @%append test.lnk LIBPATH ../dnt386.dll
    @%append test.lnk LIBPATH ../dnt386u
    @%append test.lnk LIBPATH ../dnt386u.dll
    @%append test.lnk LIBPATH ../nt386
    @%append test.lnk LIBPATH ../nt386.dll
    @%append test.lnk LIBPATH ../nt386u
    @%append test.lnk LIBPATH ../nt386u.dll

save : .SYMBOLIC
    @if exist exec.out %ren exec.out s$(arch)$(alt_error)$(host_bits).sav

global : .SYMBOLIC
    @%make common_clean
