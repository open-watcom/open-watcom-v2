#pmake: build

tree_depth = 2

watcom_root = $+$(%WATCOM)$-

!ifdef %BUILDTEST
buildtest = 1
!endif

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

!ifdef buildtest
! ifdef __OS2__
test_wmake = ..\..\wmake\os2386\wmk.exe
! else ifdef __LINUX386__
test_wmake = ../../wmake/linux386/wmk.exe
! else ifdef __LINUXX64__
!  ifeq host 386
test_wmake = ../../wmake/linux386/wmk.exe
!  else
test_wmake = ../../wmake/linuxx64/wmk.exe
!  endif
! else ifdef __NT386__
test_wmake = ..\..\wmake\nt386\wmk.exe
! else ifdef __NTX64__
!  ifeq host 386
test_wmake = ..\..\wmake\nt386\wmk.exe
!  else
test_wmake = ..\..\wmake\ntx64\wmk.exe
!  endif
! else	# DOS
test_wmake = ..\..\wmake\dos386\wmk.exe
! endif
!else
! ifdef __OS2__
test_wmake = $(watcom_root)\binp\wmake
! else ifdef __LINUX386__
test_wmake = $(watcom_root)/binl/wmake
! else ifdef __LINUXX64__
!  ifeq host 386
test_wmake = $(watcom_root)/binl/wmake
!  else
test_wmake = $(watcom_root)/binl64/wmake
!  endif
! else ifdef __NT386__
test_wmake = $(watcom_root)\binnt\wmake
! else ifdef __NTX64__
!  ifeq host 386
test_wmake = $(watcom_root)\binnt\wmake
!  else
test_wmake = $(watcom_root)\binnt64\wmake
!  endif
! else	# DOS
test_wmake = $(watcom_root)\binw\wmake
! endif
!endif

!ifdef __OS2__
testcmd=call test.cmd $(test_wmake)
!else ifdef __LINUX__
testcmd=sh test.sh $(test_wmake)
!else ifdef __NT__
testcmd=call test.bat $(test_wmake)
!else	# DOS
testcmd=call test.bat $(test_wmake)
!endif

from = $+$(%__CWD__)$-

.BEFORE
    @set TRMEM_CODE=1
    @if exist error.out del error.out
    # ===========================
    # Build utilities first
    # ===========================
    @echo *****************************************************************
!ifdef __NTX64__
! ifeq host 386
    @echo Test wmake (32-bit)
! else
    @echo Test wmake (64-bit)
! endif
!else ifdef __LINUXX64__
! ifeq host 386
    @echo Test wmake (32-bit)
! else
    @echo Test wmake (64-bit)
! endif
!else
    @echo Test wmake (32-bit)
!endif
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
    @!$(testcmd)
    @cd $(from)

implicit: .SYMBOLIC
    # =======================================
    # -- TEST2 - Implicit Rules Test
    # =======================================
    @echo *****************************************************************
    @cd $@
    @!$(testcmd)
    @cd $(from)

fortest: .SYMBOLIC
    # =======================================
    # -- FORTEST - FOR LOOP TEST
    # =======================================
    @echo *****************************************************************
    @cd $@
    @!$(testcmd)
    @cd $(from)

pretest: .SYMBOLIC
    # =======================================
    # -- PRETEST - PRE COMPILER TEST
    # =======================================
    @echo *****************************************************************
    @cd $@
    @!$(testcmd)
    @cd $(from)

updtest: .SYMBOLIC
    # =======================================
    # -- UPDTEST - UPDATE TEST
    # =======================================
    @echo *****************************************************************
    @cd $@
    @!$(testcmd)
    @cd $(from)

errtest: .SYMBOLIC
    set TRMEM_CODE=3
    # =======================================
    # -- ERRTEST - ERROR TEST
    # =======================================
    @echo *****************************************************************
    @cd $@
    @!$(testcmd)
    @cd $(from)

inline: .SYMBOLIC
    set TRMEM_CODE=1
    # =======================================
    # -- INLINE TEST -
    # =======================================
    @echo *****************************************************************
    @cd $@
    @!$(testcmd)
    @cd $(from)

preproc: .SYMBOLIC
    # =======================================
    # -- PREPROCESS TEST -
    # =======================================
    @echo *****************************************************************
    @cd $@
    @!$(testcmd)
    @cd $(from)

macros: .SYMBOLIC
    # =======================================
    # -- MACROS TEST -
    # =======================================
    @echo *****************************************************************
    @cd $@
    @!$(testcmd)
    @cd $(from)

misc: .SYMBOLIC
    # =======================================
    # -- MISC TEST -
    # =======================================
    @echo *****************************************************************
    @cd $@
    @!$(testcmd)
    @cd $(from)

longfile: .SYMBOLIC
    # =======================================
    # -- LONG FILENAME TEST -
    # =======================================
    @echo *****************************************************************
    @cd $@
    @!$(testcmd)
    @cd $(from)

finish: .SYMBOLIC
    # =======================================
    # -- End of Test
    # =======================================
    @echo ************* DONE DONE DONE ********************
    @if exist error.out echo !!!!!!!! ERRORS FOUND !!!!!!!!!!
    @if exist error.out echo look at error.out for listing
