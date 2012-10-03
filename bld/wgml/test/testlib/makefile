#pmake: build wgml

#
# Watcom GML/Script Test Device Library Makefile
#
# This uses the existing (legacy) gendev, either 
# the DOS version or the OS/2 version
#

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
# Set build_platform and define gendev
#-------------------------------------------------------

dosemu_gendev_batch = gendevb.bat

!ifeq bld_cpu 386
!ifeq bld_os dos
gendevcmd = $(%OWROOT)\docs\gml\dos\gendev

!else ifeq bld_os os2
gendevcmd = $(%OWROOT)\docs\gml\os2\gendev

!else ifeq bld_os nt

!ifdef %OWDOSBOX
#--------------------------------------------------------
# set up to use dosbox to run gendev under 64-bit Windows
#--------------------------------------------------------
use_dosemu = dosbox
!else
gendevcmd = $(%OWROOT)\docs\gml\dos\gendev
!endif

!else ifeq bld_os linux
#-------------------------------------------------------
# set up to use dosemu to run gendev under Linux
#-------------------------------------------------------
use_dosemu = dosemu
gendevcmd = $(%OWROOT:/=\)\docs\gml\dos\gendev

!else
!error Unsupported building platform !!!!!!
!endif
!else
!error Unsupported building CPU architecture !!!!!!
!endif

#-------------------------------------------------------
# Where the action is
#-------------------------------------------------------

.fon.cop:
        @%make gencop1

wgmlst.cop : .EXPLICIT
        @%make gencopall

default : wgmlst.cop test01.cop test02.cop pstest.cop psspec.cop tdrv01.cop &
        tdrv02.cop pstdrv.cop pssdrv.cop pssfon00.cop pssfon01.cop &
        pssfon02.cop pssfon03.cop pssfon04.cop pssfon05.cop pssfon06.cop &
        pssfon07.cop pssfon08.cop pssfon09.cop tfon00.cop tfon01.cop &
        tfon02.cop tfon03.cop tfon04.cop tfon05.cop tfon06.cop tfon07.cop &
        tfon08.cop tfon09.cop .SYMBOLIC

test01.cop : test01.pcd $(dev_common) $(dev_core)
        @%make gencop1

test02.cop : test02.pcd $(dev_common) $(dev_core)
        @%make gencop1

pstest.cop : pstest.pcd $(dev_common) $(dev_core)
        @%make gencop1

psspec.cop : psspec.pcd $(dev_common)
        @%make gencop1

tdrv01.cop : tdrv01.pcd $(drv_common) $(drv_core)
        @%make gencop1

tdrv02.cop : tdrv02.pcd $(drv_common) $(drv_core) $(drv_page)
        @%make gencop1

pstdrv.cop : pstdrv.pcd $(drv_common) $(drv_core) $(drv_page)
        @%make gencop1

pssdrv.cop : pssdrv.pcd $(drv_common) $(drv_page)
        @%make gencop1

genall  : .SYMBOLIC
        @%make gencopall

gencop1 : .procedure
!ifeq use_dosemu dosemu
        %create $(dosemu_gendev_batch)
        @%append $(dosemu_gendev_batch) set GMLLIB=..\testlib
        @%append $(dosemu_gendev_batch) set GMLINC=..\testsrc
        @%append $(dosemu_gendev_batch) $(gendevcmd) $^&
        @%append $(dosemu_gendev_batch) exitemu
        dosemu -dumb $(dosemu_gendev_batch)
!else ifeq use_dosemu dosbox
        %create $(dosemu_gendev_batch)
        @%append $(dosemu_gendev_batch) d:
        @%append $(dosemu_gendev_batch) set GMLLIB=e:\testlib
        @%append $(dosemu_gendev_batch) set GMLINC=e:\testsrc
        @%append $(dosemu_gendev_batch) c:\docs\gml\dos\gendev.exe $^&
        @%append $(dosemu_gendev_batch) exit
        $(%OWDOSBOX) -c "mount c $(%OWROOT)" -c "mount d ." -c "mount e .." -c "d:$(dosemu_gendev_batch)" -noconsole
!else
        set GMLLIB=..\testlib
        set GMLINC=..\testsrc
        $(gendevcmd) $^&
!endif

gencopall : .procedure
!ifeq use_dosemu dosemu
        %create $(dosemu_gendev_batch)
        @%append $(dosemu_gendev_batch) set GMLLIB=..\testlib
        @%append $(dosemu_gendev_batch) set GMLINC=..\testsrc
        @%append $(dosemu_gendev_batch) $(gendevcmd) genall
        @%append $(dosemu_gendev_batch) exitemu
        dosemu -dumb $(dosemu_gendev_batch)
!else ifeq use_dosemu dosbox
        %create $(dosemu_gendev_batch)
        @%append $(dosemu_gendev_batch) d:
        @%append $(dosemu_gendev_batch) set GMLLIB=e:\testlib
        @%append $(dosemu_gendev_batch) set GMLINC=e:\testsrc
        @%append $(dosemu_gendev_batch) c:\docs\gml\dos\gendev.exe genall
        @%append $(dosemu_gendev_batch) exit
        $(%OWDOSBOX) -c "mount c $(%OWROOT)" -c "mount d ." -c "mount e .." -c "d:$(dosemu_gendev_batch)" -noconsole
!else
        set GMLLIB=..\testlib
        set GMLINC=..\testsrc
        $(gendevcmd) genall
!endif

clean   : .SYMBOLIC
        rm -f *.cop *.bat
        rm -f stdout.txt stderr.txt
