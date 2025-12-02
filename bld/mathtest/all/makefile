mathtest_name = all

extra_run = -v

srcfile = ../c/mathtest.c

test_flags = -za99-wx-we-oaxt

!ifdef test1
test_type = i
test_flags_386 = -DFIRSTPART -fpi
test_flags_i86 = -DFIRSTPART -fpi
test_flags_axp = -DFIRSTPART
alt_error = 01
!else ifdef test2
test_type = i
test_flags_386 = -DFIRSTPART -fpi
test_flags_i86 = -DFIRSTPART -fpi
test_flags_axp = -DFIRSTPART
alt_error = 02
!else ifdef test3
test_type = i
test_flags_386 = -DFIRSTPART -fpi87
test_flags_i86 = -DFIRSTPART -fpi87
test_flags_axp = -DFIRSTPART
alt_error = 03
!else ifdef test4
test_type = c
test_flags_386 = -DSECONDPART -fpc
test_flags_i86 = -DSECONDPART -fpc
test_flags_axp = -DSECONDPART
alt_error = 04
!else ifdef test5
test_type = i
test_flags_386 = -DSECONDPART -fpi
test_flags_i86 = -DSECONDPART -fpi
test_flags_axp = -DSECONDPART
alt_error = 05
!else ifdef test6
test_type = i
test_flags_386 = -DSECONDPART -fpi
test_flags_i86 = -DSECONDPART -fpi
test_flags_axp = -DSECONDPART
alt_error = 06
!else ifdef test7
test_type = i
test_flags_386 = -DSECONDPART -fpi87
test_flags_i86 = -DSECONDPART -fpi87
test_flags_axp = -DSECONDPART
alt_error = 07
!else ifdef test8
test_type = c
test_flags_386 = -DTHIRDPART -fpc
test_flags_i86 = -DTHIRDPART -fpc
test_flags_axp = -DTHIRDPART
alt_error = 08
!else ifdef test9
test_type = i
test_flags_386 = -DTHIRDPART -fpi
test_flags_i86 = -DTHIRDPART -fpi
test_flags_axp = -DTHIRDPART
alt_error = 09
!else ifdef test10
test_type = i
test_flags_386 = -DTHIRDPART -fpi
test_flags_i86 = -DTHIRDPART -fpi
test_flags_axp = -DTHIRDPART
alt_error = 10
!else ifdef test11
test_type = i
test_flags_386 = -DTHIRDPART -fpi87
test_flags_i86 = -DTHIRDPART -fpi87
test_flags_axp = -DTHIRDPART
alt_error = 11
!else ifdef test12
test_type = c
test_flags_386 = -DFORTHPART -fpc
test_flags_i86 = -DFORTHPART -fpc
test_flags_axp = -DFORTHPART
alt_error = 12
!else ifdef test13
test_type = i
test_flags_386 = -DFORTHPART -fpi
test_flags_i86 = -DFORTHPART -fpi
test_flags_axp = -DFORTHPART
alt_error = 13
!else ifdef test14
test_type = i
test_flags_386 = -DFORTHPART -fpi
test_flags_i86 = -DFORTHPART -fpi
test_flags_axp = -DFORTHPART
alt_error = 14
!else ifdef test15
test_type = i
test_flags_386 = -DFORTHPART -fpi87
test_flags_i86 = -DFORTHPART -fpi87
test_flags_axp = -DFORTHPART
alt_error = 15
!else
test_type = c
test_flags_386 = -DFIRSTPART -fpc
test_flags_i86 = -DFIRSTPART -fpc
test_flags_axp = -DFIRSTPART
alt_error = 00
!endif

!include ../master.mif
