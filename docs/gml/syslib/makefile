#pmake: nobuild syslib

#
# This uses the existing (legacy) gendev, either
# the DOS version or the OS/2 version
#

gendev_subdir = \syslib
gendev_cmdl_params = $[@

!include wgmlcmd.mif

#-------------------------------------------------------
# Where the action is
#-------------------------------------------------------

cop_files = mono01.cop ps.cop psdrv.cop help.cop helpdrv.cop

default : $(cop_files) .SYMBOLIC

mono01.cop : mono01.fon
        @%make gencop
!ifdef wgml_ucase
        @%ren MONO01.COP $@
!endif

ps.cop : ps.pcd
        @%make gencop
!ifdef wgml_ucase
        @%ren PS.COP $@
!endif

psdrv.cop : psdrv.pcd
        @%make gencop
!ifdef wgml_ucase
        @%ren PSDRV.COP $@
!endif

help.cop : help.pcd
        @%make gencop
!ifdef wgml_ucase
        @%ren HELP.COP $@
!endif

helpdrv.cop : helpdrv.pcd
        @%make gencop
!ifdef wgml_ucase
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
        rm -f *.bat *.tmp
        rm -f $(cop_files)
        rm -f stdout.txt stderr.txt
