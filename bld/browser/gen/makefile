#pmake/50: build os_nt os_os2 os_win cpu_axp cpu_i86 cpu_386

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

dlg_dir = ../res/dlg
bnd_dir = ../bnd

.BEFORE
    @set dos4g=quiet

.extensions:
.extensions: .gpp .gh .dlg .bnd

all : $(dialog_source) $(dialog_headers)

.dlg: $(dlg_dir)
.dlg.gpp:
    dlgprs $[@ $(bnd_dir)/$[&.bnd $[&.gh $@

.dlg: $(dlg_dir)
.dlg.gh:
    dlgprs $[@ $(bnd_dir)/$[&.bnd $[&.gh $@

clean: .SYMBOLIC
    rm -f *.gh *.gpp *.tim
