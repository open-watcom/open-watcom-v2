clibtest_name = result

!include testenv2.mif

test_grp_list = &
bios &
char &
direct &
_dos &
environ &
file &
fmtint &
fmttest &
handleio &
heap &
heap2 &
intel &
intmath &
mbyte &
memory &
misc &
process &
safefile &
safefmt &
safeio &
safembyt &
safesrch &
safestr &
safwstr &
safwtime &
search &
startup &
streamio &
string &
time

!ifdef __NT__
test_grp_list += ntthrd
!else ifdef __OS2__
test_grp_list += os2thrd
!endif

# unused tests
###############
#

test : .symbolic start clean start_test cd_regress $(test_grp_list) cd_back ./chktest.exe
    @set ERROR_FILE=error.out
    @set ERROR_MSG=
    @%append $(log_file) ==== $(%__CTIME__) $(%__CWD__) ====
    $]@ $(log_file)
    %append $(log_file) TEST
    %append $(log_file) PASS $(%__CWD__)
    %make clean
    
start_test: .symbolic
    @if exist *.sav rm -f *.sav
    @if exist $(log_file) rm -f $(log_file)
    set CLIBTEST_LOG_FILE=$(log_file)

cd_regress : .symbolic
    cd regress

cd_back : .symbolic
    cd ..

$(test_grp_list) : .symbolic
    @cd $@
    -wmake -h
    @cd ..

clean : .procedure
    @%make common_clean
