dialogs = &
dgenums.$(ext) &
dgeddll.$(ext) &
dgfileed.$(ext) &
dggtroot.$(ext) &
dginhopt.$(ext) &
dgcllopt.$(ext) &
dglinedt.$(ext) &
dgmodlst.$(ext) &
dgmemflt.$(ext) &
dgnewdbr.$(ext) &
dgreport.$(ext) &
dgrxcfg.$(ext)  &
dgqrfilt.$(ext) &
dgquery.$(ext)  &
dgvwsym.$(ext)

ext = gpp
dialog_source = $+$(dialogs)$-

ext = gh
dialog_headers = $+$(dialogs)$-

dlg_dir = ..\res\dlg
bnd_dir = ..\bnd

!ifdef __OS2__
dlgprs = ..\dlgprs\o\dlgprsp
!else
dlgprs = ..\dlgprs\o\dlgprs
!endif

parse = $(dlgprs) $[@ $(bnd_dir)\$[&.bnd $[&.gh $^@

.BEFORE
    @set dos4g=quiet

.extensions:
.extensions: .gpp .gh .dlg .bnd

compile.tim : $(dialog_source) $(dialog_headers)
    wtouch $^@

.dlg: $(dlg_dir)
.dlg.gpp:
    $(parse)

.dlg: $(dlg_dir)
.dlg.gh:
    $(parse)

