#
#   REPORT EDITOR make file
#

name            = fmedit

fsrc_dir         = ..\src\
fdev_dir         = ..\dev\

!ifdef %FMEDIT_DBG
!define CC_DBG /$(%FMEDIT_DBG)
!else
!define CC_DBG /d1
!endif

compiler        = wcc
link            = wlink
rc              = wrc
code_type       = /olt
build_target    = windows
def_options     = $(code_type) /fpc$(CC_DBG)/zq/zw/ml/w4/s/zu/bt=$(build_target) /d_WINDOWS_16_
inc_dirs        = $(fsrc_dir);$(fdev_dir);$(%WIN_INCLUDE);$(watcom_h)
rc_opts         = -30 -q -bt=$(build_target)

win_library     = $(%WATCOM)\lib286\win\windows.lib
c_library       = $(%WATCOM)\lib286\win\clibl.lib

!include ..\mif\fmedit.mif

