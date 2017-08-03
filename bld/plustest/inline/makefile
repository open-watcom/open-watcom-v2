plustest_name = inline

!include testenv1.mif
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

.src: $(wasmtest_dir)/inline
.asm: $(wasmtest_dir)/inline

.asm.gcp:
    @set ERROR_MSG=failure to create $^&
    @*awk -v target=$(init_$^&) -f $(wasmtest_dir)/createc.awk -v OUTFILE=$^. $[@

.src.gcp:
    @cp $< $@

test : .symbolic start clean start_test $($(arch)_inline_objs) ./chk_exec.exe
    @set ERROR_FILE=error.out
    @set ERROR_MSG=failure to complete test stream
    %write $(%ERROR_FILE) OK
    $]@ exec.out >>$(%ERROR_FILE)
    %make save_sav
    diff -b $(sav_file) error.chk
    %make clean_sav
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

start_test : .symbolic
    %make clean_sav
    @set ERROR_FILE=exec.out
    %create $(%ERROR_FILE)
!ifndef $(arch)_inline_objs
    @%append $(%ERROR_FILE) PASS decoding $(arch)
!endif

save_sav : .procedure
    @if exist exec.out @cp exec.out $(sav_file) >$(nulldevice)
    @if exist exec.out @rm exec.out

clean_sav : .procedure
    @if exist $(sav_file) @rm $(sav_file)

clean : .procedure
    @if exist *.gcp rm -f *.gcp
    @if exist *.ls1 rm -f *.ls1
    @if exist *.dis rm -f *.dis
    @if exist failure rm failure
    %make common_clean
