objfiles =  guimain.o \
	wndgadgt.o \
	wndrect.o \
	wndtbatr.o \
	wndbgch.o \
	wndstyle.o \
	wndhist.o \
	wndhelp.o \
	wndstat.o \
	wndchoos.o \
	wndcurr.o \
	wndid.o \
	wndmem.o \
	wndutil.o \
	wndchar.o \
	wndpop.o \
	wndscrol.o \
	wndsel.o \
	wndfdlg.o \
	wnddlg.o \
	dlgpick.o \
	dlgnew.o \
	wndspawn.o \
	wndalloc.o \
	wndshrnk.o \
	wndtool.o \
	wnddoin.o \
	wndresiz.o \
	wndsysin.o \
	wndclean.o \
	wndmacro.o \
	wndring.o \
	wndefra.o \
	wndfonhk.o \
	wndshthk.o \
	wndqryhk.o \
	wndendhk.o \
	wndrszhk.o \
	wndsfra.o \
	wndfont.o \
	wndldstr.o \
	wndfront.o \
	wndpaint.o \
	wnddirt.o \
	wnddfclr.o \
	wndcolor.o \
	wndfref.o \
	wndmsg.o \
	wndmnsz.o \
	wndimain.o \
	wndcrsr.o \
	wndmouse.o wndmeth.o wndcreat.o \
	regexp.o \
	wndsrch.o dlgsrch.o wndrxerr.o 

regexp.o	: $(POSIX_DIR)/misc/regexp.c wndregx.h
