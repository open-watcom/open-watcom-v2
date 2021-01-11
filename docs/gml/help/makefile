#pmake: nobuild

#
# Watcom GML/Script Test Device Library Makefile
#
# This uses the existing (legacy) gendev, either
# the DOS version or the OS/2 version
#

gendev_subdir = \help
gendev_cmdl_params = $[@

!include wgmlcmd.mif

#-------------------------------------------------------
# Set the extensions, paths, and groups
#-------------------------------------------------------

.extensions :                           # reset extensions
.extensions : .inc .cop .pcd .fon

.pcd: .
.fon: .
.inc: .

#-------------------------------------------------------
# Where the action is
#-------------------------------------------------------

default : help.cop helpdrv.cop .SYMBOLIC

!ifndef wgml_ucase

help.cop : help.pcd
        @%make gencop

helpdrv.cop : helpdrv.pcd
        @%make gencop

!else

help.cop : help.pcd
        @%make gencop
        @%ren HELP.COP $@

helpdrv.cop : helpdrv.pcd
        @%make gencop
        @%ren HELPDRV.COP $@

!endif

gencop : .procedure
        @set GMLLIB=.
        @set GMLINC=.
        %make prepare_gendev_tmp_batch
!ifeq verbose 0
    	@echo gendev $@
!endif
        $(noecho)$(GENDEVCMD)

clean   : .SYMBOLIC
        rm -f *.bat *.cop *.COP *.tmp
        rm -f stdout.txt stderr.txt
