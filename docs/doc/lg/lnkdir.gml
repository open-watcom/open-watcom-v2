.*
.*
.dm dir begin
.sr *$$sys=''
.se *$$fnd=&'wordpos('dos',&*,3)
.if &*$$fnd. ne 0 .do begin
.   .sr *$$sys=&*$$sys., DOS
.do end
.se *$$fnd=&'wordpos('zdos',&*,3)
.if &*$$fnd. ne 0 .do begin
.   .sr *$$sys=&*$$sys., ZDOS
.do end
.se *$$fnd=&'wordpos('raw',&*,3)
.if &*$$fnd. ne 0 .do begin
.   .sr *$$sys=&*$$sys., RAW
.do end
.se *$$fnd=&'wordpos('elf',&*,3)
.if &*$$fnd. ne 0 .do begin
.   .sr *$$sys=&*$$sys., ELF
.do end
.se *$$fnd=&'wordpos('netware',&*,3)
.if &*$$fnd. ne 0 .do begin
.   .sr *$$sys=&*$$sys., NetWare
.do end
.se *$$fnd=&'wordpos('os2',&*,3)
.if &*$$fnd. ne 0 .do begin
.   .sr *$$sys=&*$$sys., OS/2
.do end
.se *$$fnd=&'wordpos('pharlap',&*,3)
.if &*$$fnd. ne 0 .do begin
.   .sr *$$sys=&*$$sys., PharLap
.do end
.se *$$fnd=&'wordpos('qnx',&*,3)
.if &*$$fnd. ne 0 .do begin
.   .sr *$$sys=&*$$sys., QNX
.do end
.se *$$fnd=&'wordpos('win16',&*,3)
.if &*$$fnd. ne 0 .do begin
.   .sr *$$sys=&*$$sys., Win16
.do end
.se *$$fnd=&'wordpos('win32',&*,3)
.if &*$$fnd. ne 0 .do begin
.   .sr *$$sys=&*$$sys., Win32
.do end
.sr *$$ban=&'upper(&*1)
.sr suppsys='All'
.if '&*$$sys' ne '' .do begin
.sr *$$ban=&*$$ban. (&'substr(&*$$sys.,3))
.sr suppsys=&'substr(&*$$sys.,3)
.do end
.if '&target' eq 'QNX' .do begin
.   .se *$$fnd=&'wordpos('QNX',&suppsys)
.   .if &*$$fnd. ne 0 or '&suppsys' eq 'All' .do begin
.   .   .pa
.   .   .se headtxt0$=&*$$ban
.   .   .se headtxt1$=&*$$ban
.   .   .im &*2
.   .do end
.do end
.el .do begin
.   .pa
.   .se headtxt0$=&*$$ban
.   .se headtxt1$=&*$$ban
.   .im &*2
.do end
.dm dir end
.*
.dm dirctv begin
.sr *$$rfs=&'left(&*,6)
.if "&*$$rfs" eq "#" .do begin
.sr *$$rfs='commen'
.do end
.el .if "&*$$rfs" eq "@" .do begin
.sr *$$rfs='includ'
.do end
.section *refid=x&*$$rfs. The &* Directive
.if '&target' ne 'QNX' .do begin
.np
:SF font=9.Formats: &suppsys:eSF.
.do end
.ix '&* directive'
.ix 'directives' '&*'
.dm dirctv end
.*
.dm option begin
.sr *$$rfs=&'left(&*,6)
.section *refid=x&*$$rfs. The &* Option
.if '&target' ne 'QNX' .do begin
.np
:SF font=9.Formats: &suppsys:eSF.
.do end
.ix '&* option'
.ix 'options' '&*'
.dm option end
.*
.* The directives below that are commented out are "overlay" related.
.*
.dir alias              ldalias.gml     all
.dir alignment          opalignm.gml    elf os2 win16 win32
.dir anonymousexport    ldanonym.gml    win16 win32
.if '&overlay' eq 'yes' .do begin
.dir area               oparea.gml      dos
.do end
.dir artificial         opartifi.gml    all
.if '&overlay' eq 'yes' .do begin
.dir autosection        ldautose.gml    dos
.do end
.dir autounload         opautoun.gml    netware
.if '&overlay' eq 'yes' .do begin
.dir begin              ldbegin.gml     dos
.do end
.dir cache              opcache.gml     all
.dir caseexact          opcaseex.gml    all
.dir check              opcheck.gml     netware
.dir checksum           opcsum.gml      win32
.dir comment            ldcmt.gml       all
.dir commit             ldcommit.gml    win32
.dir copyright          opcopyri.gml    netware
.dir custom             opcustom.gml    netware
.dir cvpack             opcvpack.gml    all
.dir debug              lddebug.gml     all
.dir description        opdescri.gml    os2 win16 win32
.dir disable            lddisabl.gml    all
.if '&overlay' eq 'yes' .do begin
.dir distribute         opdistri.gml    dos
.do end
.dir dosseg             opdosseg.gml    all
.if '&overlay' eq 'yes' .do begin
.dir dynamic            opdynami.gml    dos
.do end
.dir eliminate          opelimin.gml    all
.if '&overlay' eq 'yes' .do begin
.dir end                ldend.gml       dos
.do end
.dir endlink            ldendlin.gml    all
.dir exit               opexit.gml      netware
.dir export             ldexport.gml    elf netware os2 win16 win32
.dir farcalls           opfarcal.gml    all
.dir file               ldfile.gml      all
.dir fillchar           opfilchr.gml    all
.if '&overlay' eq 'yes' .do begin
.dir fixedlib           ldfixedl.gml    dos
.dir forcevector        ldforcev.gml    dos
.do end
.dir format             ldformat.gml    all
.dir fullheader         opfullh.gml     dos
.dir heapsize           opheap.gml      os2 qnx win16 win32
.dir help               ophelp.gml      netware
.dir hshift             ophshift.gml    dos os2 qnx win16
.dir impfile            opimpfil.gml    netware os2 win16 win32
.dir implib             opimplib.gml    netware os2 win16 win32
.dir import             ldimport.gml    elf netware os2 win16 win32
.dir include            ldinclud.gml    all
.dir incremental        opincrem.gml    elf os2 pharlap qnx win16 win32
.dir internalrelocs     opintern.gml    os2
.dir language           ldlangua.gml    all
.dir largeaddressaware  oplargaw.gml    win32
.dir libfile            ldlibfil.gml    all
.dir libpath            ldlibpat.gml    all
.dir library            ldlibrar.gml    all
.dir linearrelocs       oplinear.gml    qnx
.dir linkversion        oplnkver.gml    win32
.dir longlived          oplongli.gml    qnx
.dir manglednames       opmangle.gml    all
.dir manyautodata       opmanyau.gml    os2 win16
.dir map                opmap.gml       all
.dir maxdata            opmaxdat.gml    pharlap
.dir maxerrors          opmaxerr.gml    all
.dir messages           opmessag.gml    netware
.dir mindata            opmindat.gml    pharlap
.dir mixed1632          opmixed.gml     os2
.dir modname            opmodnam.gml    os2 win16 win32
.dir modfile            ldmodfil.gml    all
.dir modtrace           ldmodtrc.gml    all
.dir module             ldmodule.gml    elf netware
.dir multiload          opmultil.gml    netware
.dir name               ldname.gml      all
.dir namelen            opnamele.gml    all
.dir newfiles           opnewfil.gml    os2
.dir newsegment         ldnewseg.gml    dos os2 qnx win16
.dir nlmflags           opnlmfla.gml    netware
.dir noautodata         opnoauto.gml    os2 win16
.dir nodefaultlibs      opnodefa.gml    all
.dir noextension        opnoexte.gml    all
.if '&overlay' eq 'yes' .do begin
.dir noindirect         opnoindi.gml    dos
.do end
.* .dir nolargeaddressaware oplargaw.gml    win64
.dir norelocs           opnorelo.gml    qnx win32
.dir nostdcall          opnostdc.gml    win32
.dir nostub             opnostub.gml    os2 win16 win32
.if '&overlay' eq 'yes' .do begin
.dir novector           ldnovect.gml    dos
.do end
.dir objalign           opobjali.gml    elf win32
.dir oldlibrary         opoldlib.gml    os2 win16 win32
.dir offset             opoffset.gml    elf os2 pharlap qnx win32 raw
.dir oneautodata        oponeaut.gml    os2 win16
.dir option             ldoption.gml    all
.dir optlib             ldoptlib.gml    all
.dir order              ldorder.gml     all
.dir osdomain           oposdoma.gml    netware
.dir osname             oposname.gml    all
.dir osversion          oposver.gml     win32
.dir output             ldoutput.gml    all
.if '&overlay' eq 'yes' .do begin
.dir overlay            ldoverla.gml    dos
.do end
.dir packcode           oppackco.gml    dos os2 qnx win16
.dir packdata           oppackda.gml    dos os2 qnx win16
.dir path               ldpath.gml      all
.dir privilege          opprivil.gml    qnx
.dir protmode           opprotmo.gml    os2
.dir pseudopreemption   oppseudo.gml    netware
.dir quiet              opquiet.gml     all
.dir redefsok           opredefs.gml    all
.dir reentrant          opreentr.gml    netware
.dir reference          ldrefere.gml    all
.dir resource           ldresour.gml    win32
.dir resource           opresour.gml    os2 qnx win16 win32
.dir runtime            ldruntim.gml    pharlap win32 elf
.dir rwreloccheck       oprwrelo.gml    win16
.dir screenname         opscreen.gml    netware
.if '&overlay' eq 'yes' .do begin
.dir section            ldsectio.gml    dos
.do end
.dir segment            ldsegmen.gml    os2 qnx win16 win32
.dir sharelib           opsharel.gml    netware
.dir showdead           opshowde.gml    all
.if '&overlay' eq 'yes' .do begin
.dir small              opsmall.gml     dos
.do end
.dir sort               ldsort.gml      all
.dir stack              opstack.gml     all
.if '&overlay' eq 'yes' .do begin
.dir standard           opstanda.gml    dos
.do end
.dir start              opstart.gml     all
.dir startlink          ldstartl.gml    all
.dir statics            opstatic.gml    all
.dir stub               opstub.gml      os2 win16 win32
.dir symfile            opsymfil.gml    all
.dir symtrace           ldsymtrc.gml    all
.dir synchronize        opsynchr.gml    netware
.dir system             ldsystem.gml    all
.dir threadname         opthread.gml    netware
.dir togglerelocs       optoggle.gml    os2
.dir undefsok           opundefs.gml    all
.if '&overlay' eq 'yes' .do begin
.dir vector             ldvector.gml    dos
.do end
.dir verbose            opverbos.gml    all
.dir version            opversio.gml    netware os2 win16 win32
.dir vfremoval          opvfremo.gml    all
.dir xdcdata            opxdcdat.gml    netware
.*
:set symbol="headtxt0$" value="The Open Watcom Linker".
.*
