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

compiler        = wcc386
link            = wlink
rc              = wrc
code_type       = /olt
build_target    = NT
def_options     = $(code_type) /fpc$(CC_DBG)/zq/mf/w4/s/bt=$(build_target)
inc_dirs        = $(watcom_nt_h);$(fsrc_dir);$(fdev_dir);$(%WIN_INCLUDE);$(watcom_h)
rc_opts         = -q -bt=$(build_target)

!include ..\mif\fmedit.mif

