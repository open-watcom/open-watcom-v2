#pmake: build wgml

#
# Watcom GML/Script Test Device Library Makefile
#
# This uses the existing (legacy) gendev, either 
# the DOS version or the OS/2 version
#

gendev_cmdl_params = $[@

#-------------------------------------------------------
# Set build_platform and define gendev
#-------------------------------------------------------

dosemu_gendev_batch = gendevb.bat

!ifeq bld_os dos

GENDEVCMD = $(docs_dir)\gml\dos\gendev $(gendev_cmdl_params)

!else ifeq bld_os os2

GENDEVCMD = $(docs_dir)\gml\os2\gendev $(gendev_cmdl_params)

!else ifeq bld_os nt

!ifdef %OWDOSBOX
GENDEVCMD = $(%OWDOSBOX) -c "mount c $(docs_dir)" -c "mount d ." -c "mount e .." -c "d:$(dosemu_gendev_batch)" -noconsole
use_dosemu = dosbox
!else
GENDEVCMD = $(docs_dir)\gml\dos\gendev $(gendev_cmdl_params)
!endif

!else ifeq bld_os linux

GENDEVCMD = dosemu -dumb $(dosemu_gendev_batch)
use_dosemu = dosemu

!else ifeq bld_os bsd

GENDEVCMD = dosbox -c "mount c $(docs_dir)" -c "mount d ." -c "mount e .." -c "d:$(dosemu_gendev_batch)" -noconsole
use_dosemu = dosbox

!else
!error Unsupported building platform !!!!!!
!endif

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

.fon.cop:
        @%make gencop

default : test01.cop test02.cop pstest.cop psspec.cop tdrv01.cop &
        tdrv02.cop pstdrv.cop pssdrv.cop pssfon00.cop pssfon01.cop &
        pssfon02.cop pssfon03.cop pssfon04.cop pssfon05.cop pssfon06.cop &
        pssfon07.cop pssfon08.cop pssfon09.cop tfon00.cop tfon01.cop &
        tfon02.cop tfon03.cop tfon04.cop tfon05.cop tfon06.cop tfon07.cop &
        tfon08.cop tfon09.cop .SYMBOLIC

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

genall : genall.pcd .symbolic
        @%make gencop

gencop : .procedure
!ifeq use_dosemu dosemu
        %create $(dosemu_gendev_batch)
        @%append $(dosemu_gendev_batch) lredir e: linux\fs$(docs_dir)
        @%append $(dosemu_gendev_batch) set GMLLIB=..\testlib
        @%append $(dosemu_gendev_batch) set GMLINC=..\testsrc
        @%append $(dosemu_gendev_batch) e:\gml\dos\gendev.exe $(gendev_cmdl_params)
        @%append $(dosemu_gendev_batch) exitemu
        $(GENDEVCMD)
!else ifeq use_dosemu dosbox
        %create $(dosemu_gendev_batch)
        @%append $(dosemu_gendev_batch) d:
        @%append $(dosemu_gendev_batch) set GMLLIB=e:\testlib
        @%append $(dosemu_gendev_batch) set GMLINC=e:\testsrc
        @%append $(dosemu_gendev_batch) c:\gml\dos\gendev.exe $(gendev_cmdl_params)
        @%append $(dosemu_gendev_batch) exit
        $(GENDEVCMD)
!else
        set GMLLIB=..\testlib
        set GMLINC=..\testsrc
        $(GENDEVCMD)
!endif

clean   : .SYMBOLIC
        rm -f *.cop *.bat
        rm -f stdout.txt stderr.txt
