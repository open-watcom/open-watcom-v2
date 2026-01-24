#pmake: build

#
# TRMEM_CODE environment variable for wmake
#
# TRMEM_DO_NOT_PRINT  = 0x0001
# TRMEM_IGNORE_ERROR  = 0x0002
#

tree_depth = 2

watcom_root = $(%WATCOM)

!ifdef %BUILDTEST
buildtest = 1
!endif

tests = &
    cmdline &
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
testwmake=wmake -h test_wmake=$(test_wmake)

from = $+$(%__CWD__)$-

.BEFORE
    @set TRMEM_CODE=1	# TRMEM_DO_NOT_PRINT
    @echo *****************************************************************
!ifdef __NTX64__
! ifeq host 386
    @echo Test wmake (32-bit)
! else
    @echo Test wmake (64-bit)
! endif
!else ifdef __LINUXX64__
! ifeq host 386
    @echo "Test wmake (32-bit)"
! else
    @echo "Test wmake (64-bit)"
! endif
!else
    @echo Test wmake (32-bit)
!endif
    @if exist error.out @rm error.out

all: prebuild $(tests) .SYMBOLIC
#    for %t in ($(tests)) do %make %t

prebuild: .SYMBOLIC
    # ===========================
    # Build utilities first
    # ===========================
    @echo *****************************************************************
    @cd cmds
!ifdef buildtest
    @-wmake -h buildtest=1
!else
    @-wmake -h
!endif
    @cd $(from)

cmdline: .SYMBOLIC
    # =======================================
    # -- cmdline - Command line Test
    # =======================================
    @echo *****************************************************************
    @cd $@
    @-!$(testcmd)
    @cd $(from)

test1: .SYMBOLIC
    # =======================================
    # -- test1 - Multiple Dependents Test
    # =======================================
    @echo *****************************************************************
    @cd $@
    @-$(testwmake)
    @cd $(from)

implicit: .SYMBOLIC
    # =======================================
    # -- implicit - Implicit Rules Test
    # =======================================
    @echo *****************************************************************
    @cd $@
    @-$(testwmake)
    @cd $(from)

fortest: .SYMBOLIC
    # =======================================
    # -- fortest - FOR LOOP TEST
    # =======================================
    @echo *****************************************************************
    @cd $@
    @-$(testwmake)
    @cd $(from)

pretest: .SYMBOLIC
    # =======================================
    # -- pretest - PRE COMPILER TEST
    # =======================================
    @echo *****************************************************************
    @cd $@
    @-$(testwmake)
    @cd $(from)

updtest: .SYMBOLIC
    # =======================================
    # -- updtest - UPDATE TEST
    # =======================================
    @echo *****************************************************************
    @cd $@
    @-$(testwmake)
    @cd $(from)

errtest: .SYMBOLIC
    # =======================================
    # -- errtest - ERROR TEST
    # =======================================
    @echo *****************************************************************
    @cd $@
    @set TRMEM_CODE=3	# TRMEM_DO_NOT_PRINT | TRMEM_IGNORE_ERROR
    @-$(testwmake)
    @set TRMEM_CODE=1	# TRMEM_DO_NOT_PRINT
    @cd $(from)

inline: .SYMBOLIC
    # =======================================
    # -- inline - INLINE TEST -
    # =======================================
    @echo *****************************************************************
    @cd $@
    @-$(testwmake)
    @cd $(from)

preproc: .SYMBOLIC
    # =======================================
    # -- preproc - PREPROCESS TEST -
    # =======================================
    @echo *****************************************************************
    @cd $@
    @-$(testwmake)
    @cd $(from)

macros: .SYMBOLIC
    # =======================================
    # -- macros - MACROS TEST -
    # =======================================
    @echo *****************************************************************
    @cd $@
    @-$(testwmake)
    @cd $(from)

misc: .SYMBOLIC
    # =======================================
    # -- misc - MISC TEST -
    # =======================================
    @echo *****************************************************************
    @cd $@
    @-$(testwmake)
    @cd $(from)

longfile: .SYMBOLIC
    # =======================================
    # -- longfile - LONG FILENAME TEST -
    # =======================================
    @echo *****************************************************************
    @cd $@
    @-$(testwmake)
    @cd $(from)

finish: .SYMBOLIC
    # =======================================
    # -- End of Test
    # =======================================
    @echo ************* DONE DONE DONE ********************
    @if exist error.out echo !!!!!!!! ERRORS FOUND !!!!!!!!!!
    @if exist error.out echo look at error.out for listing
