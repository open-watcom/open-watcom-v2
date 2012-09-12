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

dev_common =	&
	dev01.inc	&
	dev03.inc	&
	dev07.inc

dev_core =	&
	dev02.inc	&
	dev04.inc	&
	dev05.inc	&
	dev06.inc

drv_common =	&
	drv01.inc	&
	drv02.inc

drv_core =	&
	drv03.inc

drv_page =	&
	drv04.inc

#-------------------------------------------------------
# Set build_platform and define gendev
#-------------------------------------------------------

!ifeq %build_platform dos386
gendev = $(%OWROOT)\docs\gml\dos\gendev

!else ifeq %build_platform os2386
gendev = $(%OWROOT)\docs\gml\os2\gendev

!else ifeq %build_platform nt386
gendev = $(%OWROOT)\docs\gml\dos\gendev

!else ifeq %build_platform linux386

#-------------------------------------------------------
# set up to use dosemu to run gendev under Linux
# copied/modified from how wgml is handled under Linux
#-------------------------------------------------------

dosemu_gendev_batch=gendev.bat
$(dosemu_gendev_batch): $(__MAKEFILES__)
    @%create $@
    @%append $@ unix -s GMLLIB
    @%append $@ unix -s GMLINC
    @%append $@ $(%OWROOT)\docs\gml\dos\gendev.exe "%1 %2 %3 %4 %5 %6 %7 %8"
    @%append $@ exitemu
%make $(dosemu_gendev_batch)

gendev = dosemu -dumb $(dosemu_wgml_batch)

!else
!error Unsupported building platform !!!!!!

!endif

#-------------------------------------------------------
# Set the environment variables
#-------------------------------------------------------

.before:
        set GMLLIB=$(%OWSRCDIR)\wgml\test\testlib
	set GMLINC=..\testsrc

#-------------------------------------------------------
# Where the action is
#-------------------------------------------------------

.fon.cop:
	$(gendev) $^&

wgmlst.cop : .EXPLICIT
        $(gendev) genall

default : wgmlst.cop test01.cop test02.cop pstest.cop psspec.cop tdrv01.cop &
          tdrv02.cop pstdrv.cop pssdrv.cop pssfon00.cop pssfon01.cop &
	  pssfon02.cop pssfon03.cop pssfon04.cop pssfon05.cop pssfon06.cop &
	  pssfon07.cop pssfon08.cop pssfon09.cop tfon00.cop tfon01.cop &
	  tfon02.cop tfon03.cop tfon04.cop tfon05.cop tfon06.cop tfon07.cop &
	  tfon08.cop tfon09.cop .SYMBOLIC

test01.cop : test01.pcd $(dev_common) $(dev_core)
	$(gendev) $^&

test02.cop : test02.pcd $(dev_common) $(dev_core)
	$(gendev) $^&

pstest.cop : pstest.pcd $(dev_common) $(dev_core)
	$(gendev) $^&

psspec.cop : psspec.pcd $(dev_common)
	$(gendev) $^&

tdrv01.cop : tdrv01.pcd $(drv_common) $(drv_core)
	$(gendev) $^&

tdrv02.cop : tdrv02.pcd $(drv_common) $(drv_core) $(drv_page)
	$(gendev) $^&

pstdrv.cop : pstdrv.pcd $(drv_common) $(drv_core) $(drv_page)
	$(gendev) $^&

pssdrv.cop : pssdrv.pcd $(drv_common) $(drv_page)
	$(gendev) $^&

genall  : .SYMBOLIC
	  $(gendev) genall

clean   : .SYMBOLIC
          del *.cop
