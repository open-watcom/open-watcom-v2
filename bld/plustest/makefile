#pmake: test

plustest_name = result

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
    stl94_demo &
    stl94_examples &
    stl95 &
    torture &
    wclass &

extra_grp_list = &
    cool &
!ifdef __NT__
    winnt &
!endif
    yacl

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

bits64_dir         = bits.64
field64_dir        = field.64
!ifdef __UNIX__
stl94_demo_dir     = stl.94/demo
stl94_examples_dir = stl.94/examples
stl95_dir          = stl.95/examples
!else
stl94_demo_dir     = stl.94\demo
stl94_examples_dir = stl.94\examples
stl95_dir          = stl.95\examples
!endif

!include regress/environ.mif

!ifdef __UNIX__
main_dir=$+$(%CWD)$-/regress
!else
main_dir=$+$(%CWD)$-\regress
!endif

test : .symbolic global start_test $(extra_grp_list) $(test_grp_list) finish_test global

start_test: .symbolic
    @if exist *.sav rm -f *.sav
    @if exist *.log rm -f *.log
    set PLUSTEST_NAME=$(plustest_name)
    cd $(main_dir)

cd_back : .symbolic
    cd ..

./chktest.exe : $(watcom_dir)/c/chktest.c
    $(bld_cl) $< $(wcl_util_opts)

finish_test : .symbolic cd_back ./chktest.exe
    @cdsay >> $(log_file)
    $]@ $(log_file)
    %append $(log_file) TEST
    %append $(log_file) PASS $(%__CWD__)

$(all) : .procedure
    $(%CMD2) $(%CMD_CD1)
    -@wmake -h global
    -wmake -h $(%CMD_MAKE_OPTS) $@=
    -@wmake -h $(%CMD_MAKE_OPTS) $@= save
    $(%CMD2) @cd ..
    $(%CMD2) $(%CMD_CD2)
    $(%CMD2) -@wmake -h global
    $(%CMD2) -wmake -h $(%CMD_MAKE_OPTS) $@=
    $(%CMD2) -@wmake -h $(%CMD_MAKE_OPTS) $@= save
    $(%CMD2) @cd ..
    $(%CMD3) $(%CMD_CD3)
    $(%CMD3) -@wmake -h global
    $(%CMD3) -wmake -h $(%CMD_MAKE_OPTS) $@=
    $(%CMD3) -@wmake -h $(%CMD_MAKE_OPTS) $@= save
    $(%CMD3) -@wmake -h global
    $(%CMD3) @cd ..
    $(%CMD2) $(%CMD_CD2)
    $(%CMD2) -@wmake -h global
    $(%CMD2) @cd ..
    $(%CMD2) $(%CMD_CD1)
    -@wmake -h global
    $(%CMD2) @cd ..

$(test_grp_list) : .symbolic
    -@if exist $@ @cd $@
!ifdef __UNIX__
    -@if exist ./$($@_dir) @cd ./$($@_dir)
!else
    -@if exist .\$($@_dir) @cd .\$($@_dir)
!endif
    @set CMD_CD1=
    @set CMD_CD2=
    @set CMD_CD3=
    @set CMD2=@%null
    @set CMD3=@%null
!ifdef %TEST_ARCH
    @set CMD_MAKE_OPTS=arch=$(%TEST_ARCH)
!else
    @set CMD_MAKE_OPTS=
!ifdef %EXTRA_ARCH
    @for %%a in ($($@_tests)) do -@%make %%a
    @set CMD_MAKE_OPTS=arch=$(%EXTRA_ARCH)
!endif
!endif
    @for %%a in ($($@_tests)) do -@%make %%a
    @cd $(main_dir)

cool : .symbolic
    @cd $@
    @set CMD_CD1=@cd objs
    @set CMD2=
    @set CMD_CD2=@cd examples
    @set CMD3=
    @set CMD_CD3=@cd tests
!ifdef %TEST_ARCH
    @set CMD_MAKE_OPTS=arch=$(%TEST_ARCH)
!else
    @set CMD_MAKE_OPTS=
!ifdef %EXTRA_ARCH
    @for %%a in ($($@_tests)) do -@%make %%a
    @set CMD_MAKE_OPTS=arch=$(%EXTRA_ARCH)
!endif
!endif
    @for %%a in ($($@_tests)) do -@%make %%a
    @cd $(main_dir)

winnt : .symbolic
    @cd $@
    @set CMD_CD1=
    @set CMD_CD2=
    @set CMD_CD3=
    @set CMD2=@%null
    @set CMD3=@%null
    @set CMD_MAKE_OPTS=
    @for %%a in ($($@_tests)) do -@%make %%a
    @cd $(main_dir)

yacl : .symbolic
    @cd $@
    @set CMD_CD1=@cd base
    @set CMD2=
    @set CMD_CD2=@cd io
    @set CMD3=
    @set CMD_CD3=@cd test
!ifdef %TEST_ARCH
    @set CMD_MAKE_OPTS=arch=$(%TEST_ARCH)
!else
    @set CMD_MAKE_OPTS=
!ifdef %EXTRA_ARCH
    @for %%a in ($($@_tests)) do -@%make %%a
    @set CMD_MAKE_OPTS=arch=$(%EXTRA_ARCH)
!endif
!endif
    @for %%a in ($($@_tests)) do -@%make %%a
    @cd $(main_dir)

global : .procedure
    @if exist *.obj rm -f *.obj
    @if exist *.err rm -f *.err
    @if exist *.exe rm -f *.exe
