plustest_name = inline

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
!include inline.mif

.src: $(wasmtest_dir)/inline
.asm: $(wasmtest_dir)/inline

test : .symbolic start start_test $($(arch)_inline_objs) ./chk_exec.exe
    $(noecho)%make common_set_result

start_test : .symbolic
!ifndef $(arch)_inline_objs
    @%append $(%ERROR_FILE) PASS decoding $(arch)
!endif

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean
    @if exist *.gcp $(noecho)rm -f *.gcp
    @if exist *.ls1 $(noecho)rm -f *.ls1
    @if exist *.dis $(noecho)rm -f *.dis
    @if exist failure $(noecho)rm failure
