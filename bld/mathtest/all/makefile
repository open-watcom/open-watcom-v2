mathtest_name = all

extra_run = -v

srcfile = $(mathlib_dir)/c/mathtest.c

test_flags = -za99-wx-we-oaxt

!ifdef test1
test_type = i
test_flags_386 = -fpi
test_flags_i86 = -fpi -DFIRSTPART
test_flags_axp =
alt_error = 01
!else ifdef test2
test_type = i
test_flags_386 = -fpi
test_flags_i86 = -fpi -DFIRSTPART
test_flags_axp =
alt_error = 02
!else ifdef test3
test_type = i
test_flags_386 = -fpi87
test_flags_i86 = -fpi87 -DFIRSTPART
test_flags_axp =
alt_error = 03
!else ifeq arch i86
!ifdef test4
test_type = c
test_flags_i86 = -fpc -DSECONDPART
alt_error = 04
!else ifdef test5
test_type = i
test_flags_i86 = -fpi -DSECONDPART
alt_error = 05
!else ifdef test6
test_type = i
test_flags_i86 = -fpi -DSECONDPART
alt_error = 06
!else ifdef test7
test_type = i
test_flags_i86 = -fpi87 -DSECONDPART
alt_error = 07
!else ifdef test8
test_type = c
test_flags_i86 = -fpc -DTHIRDPART
alt_error = 08
!else ifdef test9
test_type = i
test_flags_i86 = -fpi -DTHIRDPART
alt_error = 09
!else ifdef test10
test_type = i
test_flags_i86 = -fpi -DTHIRDPART
alt_error = 10
!else ifdef test11
test_type = i
test_flags_i86 = -fpi87 -DTHIRDPART
alt_error = 11
!else ifdef test12
test_type = c
test_flags_i86 = -fpc -DFORTHPART
alt_error = 12
!else ifdef test13
test_type = i
test_flags_i86 = -fpi -DFORTHPART
alt_error = 13
!else ifdef test14
test_type = i
test_flags_i86 = -fpi -DFORTHPART
alt_error = 14
!else ifdef test15
test_type = i
test_flags_i86 = -fpi87 -DFORTHPART
alt_error = 15
!else
test_type = c
test_flags_i86 = -fpc -DFIRSTPART
alt_error = 00
!endif
!else
test_type = c
test_flags_386 = -fpc
test_flags_i86 = -fpc -DFIRSTPART
test_flags_axp =
alt_error = 00
!endif

!include ../master.mif
