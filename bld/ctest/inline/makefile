ctest_name = inline

inline_assembly = 1

!ifdef test1
test_flags_386 = -s -os -wx -we
test_flags_i86 = -s -os -wx -we
alt_error = 01
!else
test_flags_386 = -s -os -wx -we
test_flags_i86 = -s -os -wx -we
alt_error = 00
!endif

!include testenv.mif
!include ../inline.mif

.src: $(wasmtest_dir)/inline
.asm: $(wasmtest_dir)/inline

test : .symbolic start start_test $($(arch)_inline_objs) test_set_result
    $(noecho)%make common_set_result_check

check_result : .procedure
    -diff -b error.out error.chk

test_set_result : .symbolic ./chk_exec.exe
    -@%write error.out OK
    -$]@ exec.out >>error.out

start_test : .symbolic
!ifndef $(arch)_inline_objs
    @%append $(%ERROR_FILE) PASS decoding
!endif

clean : .procedure
    @%make echo_clean
    @%make common_clean
    @if exist *.gc $(noecho)rm -f *.gc
    @if exist *.ls1 $(noecho)rm -f *.ls1
    @if exist *.dis $(noecho)rm -f *.dis
    @if exist failure $(noecho)rm failure
