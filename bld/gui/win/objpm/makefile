os = pm
build_target = os2_pm
model=l
compiler = comp286
plat_inc=$(watcom_dir)\os21x_h;
xflags=-zu-s
wpi_lib=wpi_pm16
wlib_opt = /p=32

!include ..\makefile

guifdlg.obj : $(guidir)\ui\c\guifdlg.c
           $($(compiler)) $(guidir)\ui\c\guifdlg.c /d__OS2__ $(cflags)

