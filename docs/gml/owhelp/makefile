proj_name = docslib

.JUST_ENOUGH

.EXTENSIONS: .cop .pcd .fon

.pcd: ../syslib
.fon: ../syslib

cop_files = &
    mono01.cop &
    psab.cop &
    psabo.cop &
    psad.cop &
    psado.cop &
    psbd.cop &
    psbdi.cop &
    psbl.cop &
    psbli.cop &
    pscb.cop &
    pscob.cop &
    pscobo.cop &
    psco.cop &
    ps.cop &
    psdrv.cop &
    pshnb.cop &
    pshnbo.cop &
    pshn.cop &
    pshno.cop &
    pshvb.cop &
    pshvbo.cop &
    pshv.cop &
    pshvo.cop &
    psnb.cop &
    psnbi.cop &
    psni.cop &
    psnr.cop &
    pspb.cop &
    pspbi.cop &
    pspi.cop &
    pspr.cop &
    psshade.cop &
    pssym.cop &
    pstrb.cop &
    pstrbi.cop &
    pstr.cop &
    pstri.cop &
    pszap.cop &
    whelp.cop &
    whelpdrv.cop

gendev_cmdl_params = $[@

!include wgmlcmd.mif

all : $(cop_files) .SYMBOLIC
    @%ren WGMLST.COP wgmlst.cop

clean: .SYMBOLIC
    @rm -f *.cop *.COP *.lst *.LST *.bat *.BAT

.fon.cop:
    @set GMLINC=.
    @set GMLLIB=
    -@%make run_gendev

.pcd.cop:
    @set GMLINC=.
    @set GMLLIB=
    -@%make run_gendev

