#
#ASM=TASM -c /DTASM
#LIB=TLIB
#LIBEND=
#

ASM=ML -c /Zd
LIB=LIB
LIBEND=;

ALL:	prntstr.obj _prntstr.obj upprstr.obj _upprstr.obj copystr.obj \
	_copystr.obj apndstr.obj _apndstr.obj malloc.obj  _malloc.obj \
	remallo.obj _remallo.obj free.obj _free.obj openf.obj _openf.obj \
	createf.obj _createf.obj closef.obj _closef.obj deletef.obj \
	_deletef.obj readf.obj _readf.obj writef.obj _writef.obj \
	setposf.obj _setposf.obj buffile.obj _buffile.obj makflst.obj \
	_makflst.obj ulflist.obj _ulflist.obj srtflst.obj _srtflst.obj \
	flmknm.obj _flmknm.obj getpath.obj _getpath.obj setpath.obj \
	_setpath.obj getdrv.obj _getdrv.obj setdrv.obj _setdrv.obj \
	getdrvs.obj _getdrvs.obj beep.obj _beep.obj bin2dec.obj \
	_bin2dec.obj bin2hex.obj _bin2hex.obj command.obj _command.obj \
	delay.obj _delay.obj getkey.obj _getkey.obj waitkey.obj \
	_waitkey.obj startup.obj null.obj vesa.obj _vesa.obj video.obj \
	_video.obj setdacs.obj _setdacs.obj srchrgb.obj _srchrgb.obj \
	remsbit.obj _remsbit.obj rmsbits.obj _rmsbits.obj regsbit.obj \
	_regsbit.obj relsbit.obj _relsbit.obj setpal.obj _setpal.obj \
	setmode.obj _setmode.obj sys2hrd.obj _sys2hrd.obj bord.obj \
	_bord.obj setpix.obj _setpix.obj fillrct.obj drwlin.obj \
	blitbit.obj mouse.obj _msetype.obj _mgetpos.obj windows.obj \
	openw.obj calsplts.obj messw.obj _messw.obj regclipw.obj \
	regupw.obj _regupw.obj closew.obj _closew.obj fillw.obj blitw.obj \
	printw.obj whichw.obj _whichw.obj bitmaps.obj ldfont.obj \
	_ldfont.obj dlgstst.obj _dlgstst.obj dlggtst.obj _dlggtst.obj \
	dlggtbx.obj _dlggtbx.obj dlggtby.obj _dlggtby.obj dlggtbw.obj \
	_dlggtbw.obj dlggtbd.obj _dlggtbd.obj dlggtw.obj _dlggtw.obj \
	dlggtd.obj _dlggtd.obj dlgbtwn.obj _dlgbtwn.obj dlgdraw.obj \
	_dlgdraw.obj dlgwhch.obj _dlgwhch.obj strlnpx.obj _strlnpx.obj \
	upchr.obj _upchr.obj crbtmp.obj _crbtmp.obj ldimage.obj \
	_ldimage.obj ldiff.obj _ldiff.obj ldgif.obj _ldgif.obj ldtga.obj \
	_ldtga.obj lnstr.obj _lnstr.obj filesel.obj _filesel.obj \
	bitfrec.obj bsetpix.obj bdrwlin.obj wdrwlin.obj wsetpix.obj \
	dlg_oc.obj mdelst.obj sviff.obj svgif.obj svtga.obj svimage.obj \
	_svtga.obj _svgif.obj _sviff.obj _svimage.obj bmyflp.obj bmxflp.obj \
	renfile.obj _renfile.obj ldpcx.obj svbm.obj ldbm.obj

.asm.obj:
 $(ASM) $*.asm
 mem
 $(LIB) ..\cw.lib -+$*.obj $(LIBEND)
 del ..\cw.bak
