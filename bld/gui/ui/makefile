proj_name = gui
.mif: $(mif_path)
!include cproj.mif

type=ui
!ifndef axp
xflags=/of
!endif
!ifeqi os nt
plat_inc +=$(watcom_h);$(%watcom)\h\nt;
!else
plat_inc +=$(watcom_h);$(%watcom)\h\win;
!endif

wv_sysobjs = &
          guismove.obj &
          guigmous.obj &
          guiflush.obj &
          guicursr.obj &
          guixhot.obj  &
          guihelp.obj  &
          guienum.obj  &
          guistat.obj  &
          uimenucr.obj &
          guixscal.obj &
          guistub.obj  &
          guitool.obj  &
          guismetr.obj &
          guifdlg.obj  &
          guidrawr.obj &
          guiicon.obj  &
          guifocus.obj &
          guistext.obj &
          guiximdi.obj &
          guixmdi.obj  &
          guixhook.obj &
          guigtext.obj &
          guimapky.obj &
          guidoscr.obj &
          guilistb.obj &
          guihotsp.obj &
          guicontr.obj &
          guigetpt.obj &
          guikey.obj   &
          guigadgt.obj &
          guixdraw.obj &
          guistrin.obj &
          guitextm.obj &
          guixmain.obj &
          guixsys.obj &
          guixloop.obj &
          guidirty.obj &
          guisstyl.obj &
          guirange.obj &
          guisize.obj  &
          guimouse.obj &
          guithumb.obj &
          guidrow.obj  &
          guigscrl.obj &
          guirect.obj  &
          guisysme.obj &
          guiarect.obj &
          guicrect.obj &
          guidrect.obj &
          guiwhole.obj &
          guifront.obj &
          guiscrol.obj &
          guifloat.obj &
          guimenu.obj  &
          guixedit.obj &
          guixdisp.obj &
          guixdlg.obj  &
          guixtent.obj &
          guixinit.obj &
          guidrawt.obj &
          guignval.obj &
          guicheck.obj &
          guixaddt.obj &
          guixutil.obj &
          guirfrsh.obj &
          guixwind.obj &
          guicolor.obj &
          guigetnr.obj &
          guidbclk.obj &
          guispawn.obj &
          guiuiev.obj  &
          guisicon.obj &
          guixbar.obj  &
          guildstr.obj  &
          guirdlg.obj  &
          guirmenu.obj  &
          guiclen.obj &

help_objext=obj
!include $(help_dir)\help.mif

wv_sysobjs += $(help_objs)

!ifndef on_build_machine
wv_sysobjs += &
       uibarf.obj
!endif

sys_hooks = &
          guisysin.obj &
          guisysfi.obj &
          uialloc.obj &
          uidchfld.obj &

sysobjs = $(wv_sysobjs) $(sys_hooks) $(sys_dep_objs)

sysinclude +=;$(ui_dir)\h;$(watcom_dir)\h;$(sdk_dir)\rc\wres\h

!include ..\..\makefile

guikey.obj :    guikey.c
           $(compile)

guixdraw.obj    : guixdraw.c guimin.h guidraw.h
guixloop.obj    : guixloop.c guimin.h
guixutil.obj    : guixutil.c guimin.h
guisize.obj     : guisize.c guimin.h
guixmain.obj    : guixmain.c $(guidir)\h\guix.h
guixmdi.obj     : guixmdi.c $(guidir)\h\guix.h

