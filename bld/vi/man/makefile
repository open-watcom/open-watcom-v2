host_cpu = 386

!include cproj.mif

#
# There should be settings for supported building platforms
#
!ifeq %build_platform dos386
wgml = $(%OWROOT)\docs\gml\dos\wgml
!else ifeq %build_platform os2386
wgml = $(%OWROOT)\docs\gml\os2\wgml
!else ifeq %build_platform nt386
wgml = $(%OWROOT)\docs\gml\dos\wgml
!else
!error Unsupported building platform !!!!!!
!endif

optsfile=wgmlopts.tmp

.before
    set GMLLIB=$(%OWROOT)\docs\gml\syslib;
    set GMLINC=.;pict;$(%OWROOT)\docs\doc\gml;
    set GMLPAG=%TEMP%

all : vi.ps .SYMBOLIC
#all : vi.ps vi.hlp .SYMBOLIC
    %null

vi.ps : vi.gml settings.gml intro.gml basic.gml &
        inter.gml advan.gml vipref.gml &
        cmds.gml modes.gml &
        win_menu.gml regexp.gml env.gml scripts.gml &
        extra.gml &
        spmapkey.gml &
        oper.gml worddef.gml speckeys.gml &
        a_errcod.gml a_cmdmod.gml a_symkey.gml
    -$(wgml) vi ( file nb7x9ps index passes 2 cpinch 20 # verbose statistics
    # Bodge supplying missing functions
    bvi -s << vi.ps
/^\/
insert
/setSize { pop gsave } def
/startFig { /Courier findfont 8 scalefont setfont 0 192 moveto pop } def
/paintString { show pop } def
/nextline { 0 currentpoint 8 sub exch pop moveto } def
/endFig { grestore showpage } def
.
x
q
<<
#    -$(wgml) stat.vi.gml (file nb7x9ps index pass 1 cpi 20 stat verb
#    -$(wgml) vi.gml (file nb7x9ps index pass 1 cpi 20 stat verb
#    -$(wgml) vi.gml (file nb7x9ps pass 1 cpi 20 stat verb
#    -$(wgml) vi.gml (file nb7x9ps pass 1 cpi 20 stat verb dev term nopause out *.res

vi.hlp: vihelp.src toc vihlp
        @erase vitoc.hlp
        @erase toc.hlp
        @erase toc.ind
        @erase vitoc.src
        @erase toc.src
        @erase vihelp.src
        @erase links.hlp
        @%null

vihelp.src: vi.gml .SYMBOLIC
    @%create $(optsfile)
    @%append $(optsfile) ( linemode
    @%append $(optsfile) ( verbose
    @%append $(optsfile) ( statistics
    @%append $(optsfile) ( passes 2
    @%append $(optsfile) ( cpinch 10
    @%append $(optsfile) ( output vihelp.src
    @%append $(optsfile) ( setsymbol dohelp 1
    @%append $(optsfile) ( process vihelp
    @%append $(optsfile) ( file help
#   Next line fails: Fixing is non-trivial. W.Briscoe 2004-02-04.
   -$(wgml) vi ( file $(optsfile)
    $(vi) -s remtoc.vi vihelp.src
    @cp toc.src vitoc.src

vihelp2: .SYMBOLIC
    cp vihelp2.src vihelp.src
    $(vi) -s remtoc.vi vihelp.src
    @cp toc.src vitoc.src

toc: .SYMBOLIC
    egrep '^  ..[^ ]' vitoc.src >t1
    $(vi) -s tocind.vi t1
    $(vi) -s links.vi vitoc.src
    $(vi) -s toc.vi vitoc.src
#    erase t1

vihlp: toc .SYMBOLIC
    @copy toc.ind+toc.hlp+links.hlp+index.hlp+vihelp.src t2
    $(vi) -s help.vi t2
#    erase t2
#    sorthelp -w 80 -h 25 t1 t2 >sorthelp.out
#    sorthelp t1

clean: .SYMBOLIC
    rm -f sysusr*.* *.ps *.tmp *.lay *.src t1 t2
