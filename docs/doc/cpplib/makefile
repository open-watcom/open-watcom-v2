drive = d:

!ifdef __OS2__
wgml=wgml
!else
wgml=wgmlp
!endif

.before
    set gmlinc=$(drive)\dev\cdoc\gml;$(drive)\dev\vi\man
#    set gmlinc=$(drive)\dev\vi\man;$(drive)\dev\cdoc\gml
    set gmllib=.;$(drive)\dev\cdoc\gml\syslib
    set gmlpag=%ramdrv%\;$(drive)\tmp
!ifndef __OS2__
    @-clog cpplib.err
!endif

.after
!ifndef __OS2__
    @-clog
!endif

cpplib.ps : complex_.gml &
        cpplib.gml &
        cpx_op.gml &
        cpx_asgn.gml &
        filebuf_.gml &
        fs_ovflw.gml &
        fstrbse_.gml &
        fstream_.gml &
        fstr_ct1.gml &
        fstr_ct2.gml &
        fstr_ct3.gml &
        fstr_ct4.gml &
        fstr_dt.gml &
        fstr_op.gml &
        hdrs.gml &
        ifstrm_.gml &
        intro.gml &
        ios_.gml &
        ios_stat.gml &
        iosinit.gml &
        iostrm_.gml &
        iotypes.gml &
        ipword.gml &
        isstrm_.gml &
        istr_in.gml &
        istream_.gml &
        manip_.gml &
        ofstrm_.gml &
        osstrm_.gml &
        ostream_.gml &
        ostr_out.gml &
        predef.gml &
        preface.gml &
        sb_rgp.gml &
        sb_sbmpc.gml &
        sb_spkc.gml &
        seekdir.gml &
        seekmode.gml &
        ssb_desc.gml &
        sst_str.gml &
        sstrbse_.gml &
        sstrbuf_.gml &
        sstrm_.gml &
        std_desc.gml &
        stdiobu_.gml &
        str_b.gml &
        str_subs.gml &
        streamb_.gml &
        string_.gml
    -$(wgml) cpplib.gml (file nb7x9ps index pass 1 cpi 10 stat verb

full : .SYMBOLIC
    -$(wgml) cpplib.gml (file nb7x9ps index pass 2 cpi 10 stat verb

listing : .SYMBOLIC
    -$(wgml) cpplib.gml (file wbls index pass 1 cpi 10 stat verb

cpplib.hlp: vihelp.src toc vihlp
        @erase vitoc.hlp
        @erase toc.hlp
        @erase toc.ind
        @erase vitoc.src
        @erase toc.src
        @erase vihelp.src
        @%null

vihelp.src: vi.gml .SYMBOLIC
   -wgmlp 'vi' ( line verbose stat pass 2 &
        out vihelp.src proc vihelp file help
    vi -q -sremtoc.vi vihelp.src
    @copy toc.src vitoc.src

vihelp2: .SYMBOLIC
    copy vihelp2.src vihelp.src
    vi -q -sremtoc.vi vihelp.src
    @copy toc.src vitoc.src

toc: .SYMBOLIC
    egrep '^  ..[^ ]' vitoc.src >t1
    vi -q -s tocind.vi t1
    vi -q -s links.vi vitoc.src
    vi -q -s toc.vi vitoc.src
    erase t1

vihlp: toc .SYMBOLIC
    @copy toc.ind+toc.hlp+links.hlp+index.hlp+vihelp.src t2
    vi -q -s help.vi t2
    sorthelp -w 80 -h 25 t1 t2 >sorthelp.out
    vi -q -s colons.vi t2
    erase t2

wgmlst.cop: help.cop helpdrv.cop
    %null

help.cop: .SYMBOLIC
    gendev help

helpdrv.cop: .SYMBOLIC
    gendev helpdrv
