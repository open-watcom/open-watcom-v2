#pmake: cpu_386 os_qnx
host_CPU = 386
host_OS = qnx

memory_model_386 = s
model = s

##########################################################################
##########################################################################
###                                                                    ###
### NOTE: This is a 'slash and burn' makefile created in an attempt    ###
###       to get a QNX Willows version of WD up quickly. Once Watcom   ###
###       and QSSL have come to an agreement on Willows support, this  ###
###       makefile should be replaced with one that's properly         ###
###       integrated into the project. This file is checked in only to ###
###       provide example source.                                      ###
###                                                                    ###
##########################################################################

os = qnx
build_target = qnx

sysinclude = $(willows_dir)/h;$(watcom_dir)/qnx_h

sys_dep_dir = $(sdk_dir)/misc;$(wpi_dir)/h

plat_inc +=$(watcom_h)

sysobjs = guixscal.obj &
          guidrect.obj &
          guimvctr.obj &
          gui3ddlg.obj &
          guienum.obj  &
          guistub.obj  &
          guistat.obj  &
          guigetpt.obj &
          guistext.obj &
          guidoscr.obj &
          mem.obj      &
          guisstyl.obj &
          guirange.obj &
          guithumb.obj &
          guigscrl.obj &
          guigetnr.obj &
          guistrin.obj &
          guiarect.obj &
          guicrect.obj &
          guirect.obj  &
          guipaint.obj &
          guihelp.obj  &
          guicutil.obj &
          guidrawr.obj &
          guismetr.obj &
          guidropd.obj &
          guifocus.obj &
          guiiscrl.obj &
          guigetrc.obj &
          guiclear.obj &
          guisys.obj   &
          guisysin.obj &
          guisysfi.obj &
          guixwind.obj &
          guixmain.obj &
          guixtent.obj &
          guicheck.obj &
          guigetx.obj  &
          guirfrsh.obj &
          guidbclk.obj &
          guicombo.obj &
          guignval.obj &
          guispawn.obj &
          guigsysh.obj &
          guiwinlp.obj &
          wstatus.obj  &
          fontstr.obj  &
          guixutil.obj &
          guimouse.obj &
          guixhook.obj &
          guidrow.obj  &
          guicolor.obj &
          guitextm.obj &
          guifront.obj &
          guiscrol.obj &
          guidirty.obj &
          guixdraw.obj &
          guisicon.obj &
          guisize.obj  &
          guifont.obj  &
          guixhot.obj  &
          guiflush.obj &
          guireset.obj &
          guitool.obj  &
          guicursr.obj &
          guifdlg.obj  &
          guissel.obj  &
          guimapky.obj &
          guixdlg.obj  &
          guixtext.obj &
          guixdisp.obj &
          guicontr.obj &
          guifloat.obj &
          guimenus.obj &
          guiximdi.obj &
          guixmdi.obj  &
          guixbar.obj  &
          guildstr.obj  &
          guirdlg.obj  &
          guirmenu.obj  &
          guiclass.obj  &
          guif1hk.obj  &
          wmdisim.obj  &
          wptoolbr.obj &
          guiclen.obj &

sysobjs += windlg.obj

!include ../guiwin.mif

extra_c_flags += -DWILLOWS -DTWIN32

#rm_wpi :  $(wpi_dir)/h/wpicomp.h .SYMBOLIC
#        @-rm $(sysobjs)

ctl3d32.obj : $(sdk_dir)/misc/ctl3d32.c

#wptoolbr.obj :  wptoolbr.c $(sdk_dir)/misc/wptoolbr.h
#           $($(compiler)) $[* $(cflags) -dNDEBUG

#wstatus.obj : wstatus.c
#        $(compile)

#fontstr.obj : fontstr.c
#        $(compile)

#wmdisim.obj : wmdisim.c
#        $(compile)

guixdlg.obj     : guixdlg.c guistyle.h
guicontr.obj    : guicontr.c guistyle.h
guisetup.obj    : guisetup.c guiwind.h $(sdk_dir)/misc/wptoolbr.h
guixwind.obj    : guixwind.c $(sdk_dir)/misc/wptoolbr.h
guimdi.obj      : guimdi.c $(sdk_dir)/misc/mdisim.h
mdisim.obj      : mdisim.c $(sdk_dir)/misc/mdisim.h
guixmdi.obj     : guixmdi.c $(guidir)/h/guix.h
