#pmake: nobuild

#
# Watcom GML/Script Whelp Device/Driver Library Makefile
#
# This uses the existing (legacy) gendev, either
# the DOS version or the OS/2 version
#

gendev_subdir = \whelp
gendev_cmdl_params = $[@

!include wgmlcmd.mif

#-------------------------------------------------------
# Where the action is
#-------------------------------------------------------

cop_files = mono01.cop whelp.cop whelpdrv.cop

default : $(cop_files) .SYMBOLIC

whelp.cop : whelp.pcd
        @%make gencop
!ifdef wgml_ucase
        @%ren WHELP.COP $@
!endif

whelpdrv.cop : whelpdrv.pcd
        @%make gencop
!ifdef wgml_ucase
        @%ren WHELPDRV.COP $@
!endif

mono01.cop : ../syslib/mono01.fon
        @%make gencop
!ifdef wgml_ucase
        @%ren MONO01.COP $@
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
        rm -f *.bat *.tmp
        rm -f $(cop_files)
        rm -f stdout.txt stderr.txt
