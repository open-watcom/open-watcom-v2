.before
    set gmlinc=pict;d:\doc\gml
    set gmllib=c:\gml
    set gmlpag=%RAMDRV%:\;c:\tmp
    @-clog vi.err

.after
    @-clog

vi.ps : vi.gml settings.gml intro.gml basic.gml &
        inter.gml advan.gml vipref.gml &
        cmds.gml modes.gml &
        win_menu.gml regexp.gml env.gml scripts.gml &
        extra.gml &
        spmapkey.gml &
        oper.gml worddef.gml speckeys.gml &
        a_errcod.gml a_cmdmod.gml a_symkey.gml

#    -wgml stat.vi.gml (file nb7x9ps index pass 1 cpi 20 stat verb
#    -wgml vi.gml (file nb7x9ps index pass 1 cpi 20 stat verb
#    -wgml vi.gml (file nb7x9ps pass 1 cpi 20 stat verb
    -wgmlp vi.gml (file nb7x9ps pass 1 cpi 20 stat verb dev term nopause out *.res

full : .SYMBOLIC
    -wgmlp vi.gml (file nb7x9ps index pass 3 cpi 20 stat verb

vi.hlp: wgmlst.cop vihelp.src toc vihlp
        @erase vitoc.hlp
        @erase toc.hlp
        @erase toc.ind
        @erase vitoc.src
        @erase toc.src
        @erase vihelp.src
        @erase links.hlp
        @%null

vihelp.src: vi.gml .SYMBOLIC
   -wgmlp 'vi' ( line verbose stat pass 2 cpi 10 &
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
    erase t2
#    sorthelp -w 80 -h 25 t1 t2 >sorthelp.out
#    sorthelp t1

wgmlst.cop: help.cop helpdrv.cop
    %null

help.cop: .SYMBOLIC
    gendev help

helpdrv.cop: .SYMBOLIC
    gendev helpdrv
