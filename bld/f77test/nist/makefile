ftest_name = positive

!ifdef test1
fflags_386 = -D2 -MF
fflags_i86 = -D2 -MM -2
fflags_axp = -d2 -si
alt_error = 01
!else ifdef test2
fflags_386 = -D2 -MF -DEBug
fflags_i86 = -D2 -MM -DEBug
fflags_axp = -d2-si
alt_error = 02
!else ifdef test3
fflags_386 = -D2 -MS -FPC
fflags_i86 = -D2 -ML -FPC
fflags_axp = -d2
alt_error = 03
!else ifdef test4
fflags_386 = -D2 -MF -6
fflags_i86 = -D2 -ML -6
fflags_axp = -d2-si
alt_error = 04
!else ifdef test5
fflags_386 = -D1 -MF
fflags_i86 = -D1 -MM -2
fflags_axp = -d1
alt_error = 05
!else ifdef test6
fflags_386 = -D1 -MF -FPI87
fflags_i86 = -D1 -MM -FPI87
fflags_axp = -d1
alt_error = 06
!else ifdef test7
fflags_386 = -D1 -MS -DEBug -4
fflags_i86 = -D1 -ML -DEBug -4
fflags_axp = -d1
alt_error = 07
!else ifdef test8
fflags_386 = -D1 -MF
fflags_i86 = -D1 -ML -SSfloats
fflags_axp = -d1
alt_error = 08
!else ifdef test9
fflags_386 = -D1 -OI -MF
fflags_i86 = -D1 -OI -MM
fflags_axp = -d1-oe
alt_error = 09
!else
fflags_386 = -OX -D1 -MS
fflags_i86 = -OX -D1 -ML
fflags_axp = -ox-d1
alt_error = 00
!endif

!include testenv.mif
!include source/positive.mif

fflags_386 += -INCPath=source #-we-w0
fflags_i86 += -INCPath=source #-we-w0
fflags_axp += -INCPath=source #-we-w0

.for: source

test : .symbolic start test.lnk $(positive_exes) test_set_result
    -$(noecho)%make common_set_result_check

check_result : .procedure
    -diff -b error.out error.chk

test_set_result : .symbolic ./chk_exec.exe
    -$(noecho)%write error.out OK
    -$]@ exec.out >>error.out

clean : .procedure
    @%make echo_clean
    @%make common_clean
    @if exist for007 $(noecho)rm for007
    @if exist for009 $(noecho)rm for009
    @if exist for024 $(noecho)rm for024
