#pmake: build wgml

#
# Watcom GML/Script Test Device Library Makefile
#
# This uses the existing (legacy) gendev, either
# the DOS version or the OS/2 version
#

gendev_cmdl_params = $[@

!include wgmlcmd.mif

#-------------------------------------------------------
# Set the extensions, paths, and groups
#-------------------------------------------------------

.extensions :                           # reset extensions
.extensions : .inc .cop .pcd .fon

.pcd: ../testsrc
.fon: ../testsrc
.inc: ../testsrc

dev_common =  &
    dev01.inc &
    dev03.inc &
    dev07.inc

dev_core =    &
    dev02.inc &
    dev04.inc &
    dev05.inc &
    dev06.inc

drv_common =  &
    drv01.inc &
    drv02.inc

drv_core =    &
    drv03.inc

drv_page =    &
    drv04.inc

#-------------------------------------------------------
# Where the action is
#-------------------------------------------------------

default : test01.cop test02.cop pstest.cop psspec.cop tdrv01.cop &
        tdrv02.cop pstdrv.cop pssdrv.cop pssfon00.cop pssfon01.cop &
        pssfon02.cop pssfon03.cop pssfon04.cop pssfon05.cop pssfon06.cop &
        pssfon07.cop pssfon08.cop pssfon09.cop tfon00.cop tfon01.cop &
        tfon02.cop tfon03.cop tfon04.cop tfon05.cop tfon06.cop tfon07.cop &
        tfon08.cop tfon09.cop .SYMBOLIC

!ifndef wgml_ucase

.fon.cop:
        @%make gencop

test01.cop : test01.pcd $(dev_common) $(dev_core)
        @%make gencop

test02.cop : test02.pcd $(dev_common) $(dev_core)
        @%make gencop

pstest.cop : pstest.pcd $(dev_common) $(dev_core)
        @%make gencop

psspec.cop : psspec.pcd $(dev_common)
        @%make gencop

tdrv01.cop : tdrv01.pcd $(drv_common) $(drv_core)
        @%make gencop

tdrv02.cop : tdrv02.pcd $(drv_common) $(drv_core) $(drv_page)
        @%make gencop

pstdrv.cop : pstdrv.pcd $(drv_common) $(drv_core) $(drv_page)
        @%make gencop

pssdrv.cop : pssdrv.pcd $(drv_common) $(drv_page)
        @%make gencop

!else

test01.cop : test01.pcd $(dev_common) $(dev_core)
        @%make gencop
        @%ren TEST01.COP $@

test02.cop : test02.pcd $(dev_common) $(dev_core)
        @%make gencop
        @%ren TEST02.COP $@

pstest.cop : pstest.pcd $(dev_common) $(dev_core)
        @%make gencop
        @%ren PSTEST.COP $@

psspec.cop : psspec.pcd $(dev_common)
        @%make gencop
        @%ren PSSPEC.COP $@

tdrv01.cop : tdrv01.pcd $(drv_common) $(drv_core)
        @%make gencop
        @%ren TDRV01.COP $@

tdrv02.cop : tdrv02.pcd $(drv_common) $(drv_core) $(drv_page)
        @%make gencop
        @%ren TDRV02.COP $@

pstdrv.cop : pstdrv.pcd $(drv_common) $(drv_core) $(drv_page)
        @%make gencop
        @%ren PSTDRV.COP $@

pssdrv.cop : pssdrv.pcd $(drv_common) $(drv_page)
        @%make gencop
        @%ren PSSDRV.COP $@

pssfon00.cop : pssfon00.fon
        @%make gencop
        @%ren PSSFON00.COP $@

pssfon01.cop : pssfon01.fon
        @%make gencop
        @%ren PSSFON01.COP $@

pssfon02.cop : pssfon02.fon
        @%make gencop
        @%ren PSSFON02.COP $@

pssfon03.cop : pssfon03.fon
        @%make gencop
        @%ren PSSFON03.COP $@

pssfon04.cop : pssfon04.fon
        @%make gencop
        @%ren PSSFON04.COP $@

pssfon05.cop : pssfon05.fon
        @%make gencop
        @%ren PSSFON05.COP $@

pssfon06.cop : pssfon06.fon
        @%make gencop
        @%ren PSSFON06.COP $@

pssfon07.cop : pssfon07.fon
        @%make gencop
        @%ren PSSFON07.COP $@

pssfon08.cop : pssfon08.fon
        @%make gencop
        @%ren PSSFON08.COP $@

pssfon09.cop : pssfon09.fon
        @%make gencop
        @%ren PSSFON09.COP $@

tfon00.cop : tfon00.fon
        @%make gencop
        @%ren TFON00.COP $@

tfon01.cop : tfon01.fon
        @%make gencop
        @%ren TFON01.COP $@

tfon02.cop : tfon02.fon
        @%make gencop
        @%ren TFON02.COP $@

tfon03.cop : tfon03.fon
        @%make gencop
        @%ren TFON03.COP $@

tfon04.cop : tfon04.fon
        @%make gencop
        @%ren TFON04.COP $@

tfon05.cop : tfon05.fon
        @%make gencop
        @%ren TFON05.COP $@

tfon06.cop : tfon06.fon
        @%make gencop
        @%ren TFON06.COP $@

tfon07.cop : tfon07.fon
        @%make gencop
        @%ren TFON07.COP $@

tfon08.cop : tfon08.fon
        @%make gencop
        @%ren TFON08.COP $@

tfon09.cop : tfon09.fon
        @%make gencop
        @%ren TFON09.COP $@

!endif

genall : genall.pcd .symbolic
        @%make gencop

gencop : .procedure
        @set GMLLIB=..\testlib
        @set GMLINC=..\testsrc
        %make prepare_gendev_tmp_batch
!ifeq verbose 0
    	@echo gendev $@
!endif
        $(noecho)$(GENDEVCMD)

clean   : .SYMBOLIC
        rm -f *.bat *.cop *.COP *.tmp
        rm -f stdout.txt stderr.txt
