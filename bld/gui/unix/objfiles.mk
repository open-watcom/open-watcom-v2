objfiles += guicreat.o guihint.o guihook.o guimdime.o \
           guiimdi.o guiimmdi.o guihflt.o guihot.o guidraw.o guiutil.o \
           guipixel.o guipick.o guiscale.o guirscal.o guigcolr.o \
           guimkey.o guizlist.o guiextra.o guimin.o guisetup.o guidlg.o \
           guistyle.o guiwnclr.o guistr.o guiisgui.o guideath.o guidead.o \
           guihtool.o guiclrst.o guiev.o guisdef.o guiextnm.o guifcrck.o \
           guimem.o guimdi.o \
  guismove.o \
  guigmous.o \
  guiflush.o \
  guicursr.o \
  guixhot.o  \
  guienum.o  \
  guistat.o  \
  uimenucr.o \
  guixscal.o \
  guistub.o  \
  guitool.o  \
  guismetr.o \
  guidrawr.o \
  guiicon.o  \
  guifocus.o \
  guistext.o \
  guiximdi.o \
  guixmdi.o  \
  guixhook.o \
  guigtext.o \
  guimapky.o \
  guidoscr.o \
  guilistb.o \
  guihotsp.o \
  guicontr.o \
  guigetpt.o \
  guikey.o   \
  guigadgt.o \
  guixdraw.o \
  guistrin.o \
  guitextm.o \
  guixmain.o \
  guixloop.o \
  guidirty.o \
  guisstyl.o \
  guirange.o \
  guisize.o  \
  guimouse.o \
  guithumb.o \
  guidrow.o  \
  guigscrl.o \
  guirect.o  \
  guisysme.o \
  guiarect.o \
  guicrect.o \
  guidrect.o \
  guiwhole.o \
  guifront.o \
  guiscrol.o \
  guifloat.o \
  guimenu.o  \
  guixedit.o \
  guixdisp.o \
  guixdlg.o  \
  guixtent.o \
  guixinit.o \
  guidrawt.o \
  guignval.o \
  guicheck.o \
  guixaddt.o \
  guixutil.o \
  guirfrsh.o \
  guixwind.o \
  guigetnr.o \
  guidbclk.o \
  guispawn.o \
  guiuiev.o  \
  guisicon.o \
  guixbar.o  \
  guiclen.o \
  guindlg.o \
  guisysin.o guisysfi.o uialloc.o uidchfld.o \
  guixsys.o guicolor.o guildstr.o guirdlg.o guirmenu.o guihelp.o guifdlg.o 

guikey.o :	guikey.c

#           $(compile)

guixdraw.o	: guixdraw.c guimin.h guidraw.h
guixloop.o	: guixloop.c guimin.h
guixutil.o	: guixutil.c guimin.h
guisize.o	: guisize.c guimin.h
guixmain.o	: guixmain.c $(guidir)\h\guix.h
guixmdi.o	: guixmdi.c $(guidir)\h\guix.h



guisetup.o	: guisetup.c guiwind.h

guimin.o	: guimin.c guimin.h

guistr.o	: guistr.c guistr.h gui.msg

guimem.o	: guimem.c

#trmemcvr.o	: trmemcvr.c

#trmem.o	: trmem.c

