# type=win
# sys_dep_dir = $(sdk_dir)\misc;$(wpi_dir)\h
# !ifeqi os nt
# plat_inc +=$(watcom_h);$(%watcom)\h\nt;
# !else
# plat_inc +=$(watcom_h);$(%watcom)\h\win;
# !endif
# extra_objs = wpimem.obj
objfiles += guicreat.o guihint.o guihook.o guimdime.o \
           guiimdi.o guiimmdi.o guihflt.o guihot.o guidraw.o guiutil.o \
           guipixel.o guipick.o guiscale.o guirscal.o guigcolr.o \
           guimkey.o guizlist.o guiextra.o guimin.o guisetup.o guidlg.o \
           guistyle.o guiwnclr.o guistr.o guiisgui.o guideath.o guidead.o \
           guihtool.o guiclrst.o guiev.o guisdef.o guiextnm.o guifcrck.o \
           guimem.o guimdi.o guindlg.o fontstr.o wmdisim.o wstatus.o

objfiles += guixscal.o \
 	  guidrect.o \
	  guimvctr.o \
	  gui3ddlg.o \
	  guienum.o \
	  guistub.o \
	  guistat.o \
          guigetpt.o \
	  guistext.o \
	  guidoscr.o \
	  mem.o \
	  guisstyl.o \
	  guirange.o \
	  guithumb.o \
	  guigscrl.o \
	  guigetnr.o \
	  guistrin.o \
	  guiarect.o \
	  guicrect.o \
	  guirect.o \
	  guipaint.o \
	  guihelp.o \
	  guicutil.o \
	  guidrawr.o \
	  guismetr.o \
	  guidropd.o \
	  guifocus.o \
	  guiiscrl.o \
	  guigetrc.o \
	  guiclear.o \
	  guisys.o \
	  guisysin.o \
	  guisysfi.o \
          guixwind.o \
          guixmain.o \
	  guixtent.o \
	  guicheck.o \
	  guigetx.o \
	  guirfrsh.o \
	  guidbclk.o \
	  guicombo.o \
	  guignval.o \
	  guispawn.o \
	  guigsysh.o \
	  guiwinlp.o \
          guixutil.o \
	  guimouse.o \
	  guixhook.o \
	  guidrow.o \
	  guicolor.o \
	  guitextm.o \
	  guifront.o \
	  guiscrol.o \
	  guidirty.o \
  	  guixdraw.o \
  	  guisicon.o \
 	  guisize.o \
	  guifont.o \
	  guixhot.o \
	  guiflush.o \
 	  guireset.o \
 	  guitool.o \
 	  guicursr.o \
 	  guifdlg.o \
 	  guissel.o \
 	  guimapky.o \
 	  guixdlg.o \
          guixtext.o \
 	  guixdisp.o \
 	  guicontr.o \
 	  guifloat.o \
 	  guimenus.o \
 	  guiximdi.o \
 	  guixmdi.o \
 	  guixbar.o \
 	  guildstr.o \
 	  guirdlg.o \
 	  guirmenu.o \
 	  guiclass.o \
 	  guif1hk.o \
	  wptoolbr.o \
	  guiclen.o \
	  wpi_win.o  \
	  windlg.o

#		wstatus.o \
#		fontstr.o \
#	  wmdisim.o \

# !ifneq os pm
# sysobjs +=	       &
#  	  windlg.obj   
# !else
# sysobjs +=	       &
# 	  os2mem.obj   &
#  	  os2dlg.obj
# !endif

# !ifeq os w386
# sysobjs +=	       &
# 	  ctl3d32.obj
# !endif

##  !include ..\..\makefile

rm_wpi :  $(wpi_dir)/h/wpicomp.h .SYMBOLIC
	@-rm $(objfiles)

# ctl3d32.o : $(sdk_dir)/misc/ctl3d32.c

$(OBJDIR)/wptoolbr.o :	$(SDK_DIR)/misc/wptoolbr.c $(SDK_DIR)/misc/wptoolbr.h
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

$(OBJDIR)/wpi_win.o : $(WPI_DIR)/c/wpi_win.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

$(OBJDIR)/fontstr.o : $(SDK_DIR)/misc/fontstr.c $(SDK_DIR)/misc/fontstr.h
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

$(OBJDIR)/wmdisim.o : $(SDK_DIR)/misc/wmdisim.c $(SDK_DIR)/misc/wmdisim.h
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

$(OBJDIR)/wstatus.o : $(SDK_DIR)/misc/wstatus.c $(SDK_DIR)/misc/wstatus.h
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

wstatus.o : wstatus.c
#	$(compile)

fontstr.o : fontstr.c
#	$(compile)

wmdisim.o : wmdisim.c
#	$(compile)

guixdlg.o	: guixdlg.c guistyle.h
guicontr.o	: guicontr.c guistyle.h
guisetup.o	: guisetup.c guiwind.h
guixwind.o	: guixwind.c
guimdi.o	: guimdi.c
mdisim.o	: mdisim.c
guixmdi.o	: guixmdi.c
#guisetup.o	: guisetup.c guiwind.h $(SDK_DIR)/misc/wptoolbr.h
#guixwind.o	: guixwind.c $(SDK_DIR)/misc/wptoolbr.h
#guimdi.o	: guimdi.c $(SDK_DIR)/misc/mdisim.h
#mdisim.o	: mdisim.c $(SDK_DIR)/misc/mdisim.h
#guixmdi.o	: guixmdi.c $(guidir)/h/guix.h
