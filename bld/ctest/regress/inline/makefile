ctest_name = inline

!include testenv.mif
!include inline.mif

!ifdef test1
test_flags_386 =
test_flags_i86 =
alt_error = 01
!else
test_flags_386 =
test_flags_i86 =
alt_error = 00
!endif

test_flags_386 += -s -os -wx -we
test_flags_i86 += -s -os -wx -we

.c: $(wasmtest_dir)/inline
.asm: $(wasmtest_dir)/inline

.asm.gc:
    @set ERROR_MSG=failure to create $^.
    @awk -v target=$(init_$^&) -f $(wasmtest_dir)/createc.awk $[@ >$@

test : .symbolic start clean start_test $($(arch)_inline_objs) ./chk_exec.exe
    @set ERROR_FILE=error.out
    @set ERROR_MSG=failure to complete test stream
    %write $(%ERROR_FILE) OK
    $]@ exec.out >>$(%ERROR_FILE)
    diff -b $(%ERROR_FILE) error.chk
    %append $(log_file) PASS $(%__CWD__)
    %make clean

start_test : .symbolic
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav
    @set ERROR_FILE=exec.out
    %create $(%ERROR_FILE)
!ifndef $(arch)_inline_objs
    @%append $(%ERROR_FILE) PASS decoding
!endif

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav

clean : .procedure
    @if exist *.gc rm -f *.gc
    @if exist *.ls1 rm -f *.ls1
    @if exist *.dis rm -f *.dis
    @if exist failure rm failure
    @%make common_clean
