.before
    set gmlinc=pict;d:\gml\gml
    set gmllib=.;d:\gml
    set gmlpag=%RAMDRV%:\;c:\tmp
    @-clog imged.err

.after
    @-clog

imged.ps : imged.gml about.gml intro.gml basic.gml colours.gml edit.gml &
        file.gml tools.gml

#    -wgml stat.vi.gml (file nb7x9ps index pass 1 cpi 20 stat verb
#    -wgml vi.gml (file nb7x9ps index pass 1 cpi 20 stat verb
    -wgml imged.gml (file nb7x9ps pass 1 cpi 20 stat verb

full : .SYMBOLIC
    -wgml imged.gml (file nb7x9ps index pass 3 cpi 20 stat verb

imged.hlp: wgmlst.cop vihelp.src toc vihlp
        @erase vitoc.hlp
        @erase toc.hlp
        @erase toc.ind
        @erase vitoc.src
        @erase toc.src
        @erase vihelp.src
        @erase links.hlp
        @%null

vihelp.src: imged.gml .SYMBOLIC
   -wgmlp 'imged' ( line verbose stat pass 2 cpi 10 &
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
