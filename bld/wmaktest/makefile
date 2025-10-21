#pmake: build

tree_depth = 2

tests = &
    test1 &
    implicit &
    fortest &
    pretest &
    updtest &
    errtest &
    inline &
    preproc &
    macros &
    misc &
    longfile &
    finish

!ifdef __OS2__
test_wmake = ..\..\wmake\os2386\wmk.exe
cmdext=.cmd
!else ifdef __LINUX__
test_wmake = ../../wmake/linux386/wmk.exe
cmdext=.sh
!else ifdef __NT__
test_wmake = ..\..\wmake\nt386\wmk.exe
cmdext=.bat
!else	# DOS
test_wmake = ..\..\wmake\dos386\wmk.exe
cmdext=.bat
!endif

from = $+$(%__CWD__)$-

.BEFORE
    @set TRMEM_CODE=1
    @if exist error.out del error.out
    # ===========================
    # Build utilities first
    # ===========================
    @cd cmds
    @-wmake -h
    @cd $(from)

all: $(tests) .SYMBOLIC
#    for %t in ($(tests)) do %make %t

test1: .SYMBOLIC
    # =======================================
    # -- test1 - Multiple Dependents Test
    # =======================================
    @echo *****************************************************************
    @cd $@
    @!call test$(cmdext) $(test_wmake)
    @cd $(from)

implicit: .SYMBOLIC
    # =======================================
    # -- TEST2 - Implicit Rules Test
    # =======================================
    @echo *****************************************************************
    @cd $@
    @!call test$(cmdext) $(test_wmake)
    @cd $(from)

fortest: .SYMBOLIC
    # =======================================
    # -- FORTEST - FOR LOOP TEST
    # =======================================
    @echo *****************************************************************
    @cd $@
    @!call test$(cmdext) $(test_wmake)
    @cd $(from)

pretest: .SYMBOLIC
    # =======================================
    # -- PRETEST - PRE COMPILER TEST
    # =======================================
    @echo *****************************************************************
    @cd $@
    @!call test$(cmdext) $(test_wmake)
    @cd $(from)

updtest: .SYMBOLIC
    # =======================================
    # -- UPDTEST - UPDATE TEST
    # =======================================
    @echo *****************************************************************
    @cd $@
    @!call test$(cmdext) $(test_wmake)
    @cd $(from)

errtest: .SYMBOLIC
    set TRMEM_CODE=3
    # =======================================
    # -- ERRTEST - ERROR TEST
    # =======================================
    @echo *****************************************************************
    @cd $@
    @!call test$(cmdext) $(test_wmake)
    @cd $(from)

inline: .SYMBOLIC
    set TRMEM_CODE=1
    # =======================================
    # -- INLINE TEST -
    # =======================================
    @echo *****************************************************************
    @cd $@
    @!call test$(cmdext) $(test_wmake)
    @cd $(from)

preproc: .SYMBOLIC
    # =======================================
    # -- PREPROCESS TEST -
    # =======================================
    @echo *****************************************************************
    @cd $@
    @!call test$(cmdext) $(test_wmake)
    @cd $(from)

macros: .SYMBOLIC
    # =======================================
    # -- MACROS TEST -
    # =======================================
    @echo *****************************************************************
    @cd $@
    @!call test$(cmdext) $(test_wmake)
    @cd $(from)

misc: .SYMBOLIC
    # =======================================
    # -- MISC TEST -
    # =======================================
    @echo *****************************************************************
    @cd $@
    @!call test$(cmdext) $(test_wmake)
    @cd $(from)

longfile: .SYMBOLIC
    # =======================================
    # -- LONG FILENAME TEST -
    # =======================================
    @echo *****************************************************************
    @cd $@
    @!call test$(cmdext) $(test_wmake)
    @cd $(from)

finish: .SYMBOLIC
    # =======================================
    # -- End of Test
    # =======================================
    @echo ************* DONE DONE DONE ********************
    @if exist error.out echo !!!!!!!! ERRORS FOUND !!!!!!!!!!
    @if exist error.out echo look at error.out for listing
