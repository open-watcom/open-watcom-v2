#pmake: test

plustest_name = result

!include testenv.mif

test_grp_list = &
    arith &
    bits &
    bits64 &
    bitset &
    claslist &
    contain &
    datainit &
    dcmat &
    diagnose &
    ehmix &
    except &
    exercise &
    extref &
    field &
    field64 &
    format &
    getlong &
    gstring &
    inline &
    jam &
    nih &
    nullptr &
    overload &
    owstl &
    perl &
    positive &
    preproc &
    re2c &
    re2c05 &
    sizelist &
    splash &
    stepanov &
    torture &
    wclass &
    yacl &
!ifdef __NT__
    winnt
!endif

test_grp2_list = &
    cool &
    cool_examples &
    stl94_demo &
    stl94_examples &
    stl95

# unused tests
###############
#
# diagnostic.8
# mscompat
#

!inject test0   all gr1 gr2 gr3 gr4 gr5 gr6 gr10
!inject test1   all     gr2 gr3 gr4 gr5 gr6 gr10
!inject test2   all         gr3 gr4 gr5 gr6 gr10
!inject test3   all             gr4 gr5 gr6 gr10
!inject test4   all                 gr5 gr6 gr10
!inject test5   all                     gr6 gr10
!inject test6   all                         gr10
!inject test7   all                         gr10
!inject test8   all                         gr10
!inject test9   all                         gr10
!inject test10  all
!inject test11  all

arith_tests          = $(gr6)
bits_tests           = $(gr2)
bits64_tests         = $(gr2)
bitset_tests         = $(gr2)
claslist_tests       = $(gr2)
contain_tests        = $(gr4)
cool_tests           = $(gr4)
cool_examples_tests  = $(gr4)
datainit_tests       = $(gr1)
dcmat_tests          = $(gr2)
diagnose_tests       = $(gr1)
ehmix_tests          = $(gr6)
except_tests         = $(gr4)
exercise_tests       = $(gr10)
extref_tests         = $(gr4)
field_tests          = $(gr4)
field64_tests        = $(gr4)
format_tests         = $(gr2)
getlong_tests        = $(gr2)
gstring_tests        = $(gr2)
inline_tests         = $(gr2)
jam_tests            = $(gr2)
nih_tests            = $(gr2)
nullptr_tests        = $(gr2)
overload_tests       = $(gr3)
owstl_tests          = $(gr6)
perl_tests           = $(gr2)
positive_tests       = $(all)
preproc_tests        = $(gr1)
re2c_tests           = $(gr2)
re2c05_tests         = $(gr2)
sizelist_tests       = $(gr2)
splash_tests         = $(gr3)
stepanov_tests       = $(gr3)
stl94_demo_tests     = $(gr4)
stl94_examples_tests = $(gr4)
stl95_tests          = $(gr6)
torture_tests        = $(gr2)
wclass_tests         = $(gr3)
winnt_tests          = $(gr4)
yacl_tests           = $(gr4)

!ifdef __UNIX__
cool_examples_dir  = cool/examples
cool_dir           = cool/tests
stl94_demo_dir     = stl.94/demo
stl94_examples_dir = stl.94/examples
stl95_dir          = stl.95/examples
!else
cool_examples_dir  = cool\examples
cool_dir           = cool\tests
stl94_demo_dir     = stl.94\demo
stl94_examples_dir = stl.94\examples
stl95_dir          = stl.95\examples
!endif

test : .symbolic clean start_test cd_regress $(test_grp_list) $(test_grp2_list) cd_back ./chktest.exe
    @%append $(log_file) ==== $(%__CTIME__) $(%__CWD__) ====
    $]@ $(log_file)
    %append $(log_file) TEST
    %append $(log_file) PASS $(%__CWD__)
    %make clean
    
start_test: .symbolic
    @if exist *.sav rm -f *.sav
    @if exist $(log_file) rm -f $(log_file)
    set PLUSTEST_LOG_FILE=$(log_file)

cd_regress : .symbolic
    cd regress

cd_back : .symbolic
    cd ..

$(all) : .procedure
!ifdef %TEST_ARCH
    -wmake -h arch=$(%TEST_ARCH) $@=
    -@wmake -h arch=$(%TEST_ARCH) $@= save
!else
    -wmake -h $@=
    -@wmake -h $@= save
!ifdef %EXTRA_ARCH
    -wmake -h arch=$(%EXTRA_ARCH) $@=
    -@wmake -h arch=$(%EXTRA_ARCH) $@= save
!endif
!endif

$(test_grp_list) : .symbolic
    @cd $@
    @for %%a in ($($@_tests)) do -@%make %%a
    @cd ..

$(test_grp2_list) : .symbolic
    @cd $($@_dir)
    @for %%a in ($($@_tests)) do -@%make %%a
    @cd ..
    @cd ..

clean : .procedure
    @%make common_clean
