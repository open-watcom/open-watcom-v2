# pp11 has incorrect output
plustest_name = preproc

!include testenv.mif

alt_error=00

.c.i:
    %append $(%ERROR_FILE) @@@ $^@ @@@
    @%make set_path_wpp
    $(wpp_$(arch)) $[@ -w0 -pl -fo=.i -na
    @%make set_old_path
    cat $(%ERROR_FILE) $^@ >tmp_exec.out
    cp tmp_exec.out $(%ERROR_FILE)

.c.i1:
    %append $(%ERROR_FILE) @@@ $^@ @@@
    @%make set_path_wpp
    $(wpp_$(arch)) $[@ -w0 -plc -fo=.i1 -na
    @%make set_old_path
    cat $(%ERROR_FILE) $^@ >tmp_exec.out
    cp tmp_exec.out $(%ERROR_FILE)

.c.i0:
    %append $(%ERROR_FILE) @@@ $^@ @@@
    @%make set_path_wpp
    $(wpp_$(arch)) $[@ -w0 -pl -pw=0 -fo=.i0 -na
    @%make set_old_path
    cat $(%ERROR_FILE) $^@ >tmp_exec.out
    cp tmp_exec.out $(%ERROR_FILE)

all_tests = &
    pp01.i &
    pp02.i &
    pp03.i &
    pp04.i &
    pp04.i0 &
    pp05.i1 &
    pp06.i1 &
    pp07.i1 &
    pp08.i1 &
    pp09.i1 &
    pp10.i1 &
    pp11.i1 &
    pp12.i1 &
    pp13.i &
    pp14.i1 &
    pp15.i0 &
    pp16.i1 &
    pp17.i1 &


test : .symbolic start clean start_test $(all_tests)
    diff $(%ERROR_FILE) exec.chk
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

start_test : .symbolic
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav
    @set ERROR_FILE=exec.out
    %create $(%ERROR_FILE)
    %make gen_input

zap_output : .procedure
    @if exist *.i rm *.i
    @if exist *.i0 rm *.i0
    @if exist *.i1 rm *.i1
    @if exist *.gh rm *.gh

gen_input: .procedure ./geninput.exe
    $[@
    @rm *.exe
    @rm *.obj

./geninput.exe : geninput.c
    $(bld_cl) $(bld_cl_opts) -cc++ $<

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav

clean : .procedure
    @%make zap_output
    @%make common_clean
