#
#   REPORT EDITOR make file
#

host_OS = nt
host_CPU= axp
proj_name = fmedit
!include cproj.mif


name            = fmedit

fsrc_dir         = ..\src\
fdev_dir         = ..\dev\

!ifdef %FMEDIT_DBG
!define CC_DBG /$(%FMEDIT_DBG)
!else
!define CC_DBG /d1
!endif

compiler        = wccaxp
link            = wlink
rc              = wrc
code_type       = /olt
build_target    = NT
def_options     = $(code_type) $(CC_DBG)/zq/w4/s/bt=$(build_target)
inc_dirs        = $(watcom_dir)\h;$(fsrc_dir);$(fdev_dir);$(%WIN_INCLUDE)
rc_opts         = -q -bt=$(build_target)
__ALPHA__       = 1

!include ..\mif\fmedit.mif

