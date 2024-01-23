.se __optidx=0
.*
.dm optadd begin
.se optfnd=&'vecpos(&*.,__optid)
.if '&optfnd.' eq '0' .do begin
.   .se __optidx=&__optidx.+1
.   .se __optid(&__optidx.)=&*1
.   .se __optlst(&__optidx.)=&*2
.   .se __optstr(&__optidx.)=&*3
.do end
.el .do begin
.   .ty *** &*. - already defined ***
.do end
.dm optadd end
.*
.* create a wide character entry (e.g., wcscmp)
.* corresponding to regular entry (e.g., strcmp)
.*
.dm optget begin
.se optfnd=&'vecpos(&*.,__optid)
.if '&optfnd.' eq '0' .do begin
.   .ty *** &*. - referenced but not defined ***
.do end
.el .do begin
.   .if &e'&dohelp eq 0 .do begin
.   .   .se optid=&__optid(&optfnd.).
.   .do end
.   .   .se optid=SW&__optid(&optfnd.).
.   .el .do begin
.   .do end
.   .se optlst=&__optlst(&optfnd.).
.   .se optstr=&__optstr(&optfnd.).
.   .se optmac=__SW_&'upper(&__optid(&optfnd.).)
.do end
.dm optget end
.*
.* .optadd 0 0 '&286only. 8088 and 8086 instructions (default for 16-bit)'
.* .optadd 1 1 '&286only. 188 and 186 instructions'
.* .optadd 2 2 '&286only. 286 instructions'
.* .optadd 3 3 '&286only. 386 instructions'
.* .optadd 3r 3r '&386only. 386 instructions and use register-based argument passing conventions'
.* .optadd 3s 3s '&386only. 386 instructions and use stack-based argument passing conventions'
.* .optadd 4 4 '&286only. 486 instructions'
.* .optadd 4r 4r '&386only. 486 instructions and use register-based argument passing conventions'
.* .optadd 4s 4s '&386only. 486 instructions and use stack-based argument passing conventions'
.* .optadd 5 5 '&286only. Pentium instructions'
.* .optadd 5r 5r '&386only. Pentium instructions and use register-based argument passing conventions'
.* .optadd 5s 5s '&386only. Pentium instructions and use stack-based argument passing conventions'
.* .optadd 6 6 '&286only. Pentium Pro instructions'
.* .optadd 6r 6r '&386only. Pentium Pro instructions and use register-based argument passing conventions'
.* .optadd 6s 6s '&386only. Pentium Pro instructions and use stack-based argument passing conventions'
.* .optadd aa aa '(C only) allow non-constant initializers for local aggregates or unions'
.* .optadd ad 'ad[=<file name>]' 'generate make style automatic dependency file'
.* .optadd add 'add[=<file name>]' 'specify source dependency name generated in make style auto-dependency file'
.* .optadd adhp 'adhp[=<file name>]' 'specify path to use for headers with no path given'
.* .optadd adt 'adt[=<target_name>]' 'specify target name generated in make style auto-dependency file'
.* .optadd adbs adbs 'force path separators generated in auto-dependency files to backslashes'
.* .optadd adfs adfs 'force path separators generated in auto-dependency files to forward slashes'
.* .optadd as as '&AXPonly. assume short integers are aligned'
.* .optadd bc bc 'build target is a console application'
.* .optadd bd bd 'build target is a Dynamic Link Library (DLL)'
.* .optadd bg bg 'build target is a GUI application'
.* .optadd bm bm
.* .optadd br br
.* .optadd bt bt
.* .optadd bw bw
.* .optadd d0 d0
.* .optadd d1 d1
.* .optadd d1pls d1+
.* .optadd d2 d2
.* .optadd d2i d2i
.* .optadd d2s d2s
.* .optadd d2t d2t
.* .optadd d3 d3
.* .optadd d3i d3i
.* .optadd d3s d3s
.* .optadd d 'd<name>[=text]'
.* .optadd dpls d+
.* .optadd db db
.* .optadd e 'e<number>'
.* .optadd ecc ecc
.* .optadd ecd ecd
.* .optadd ecf ecf
.* .optadd eco eco
.* .optadd ecp ecp
.* .optadd ecr ecr
.* .optadd ecs ecs
.* .optadd ecw ecw
.* .optadd ee ee
.* .optadd ef ef
.* .optadd ei ei
.* .optadd em em
.* .optadd en en
.* .optadd ep 'ep[<number>]'
.* .optadd eq eq
.* .optadd er er
.* .optadd et et
.* .optadd ew ew
.* .optadd ez ez
.* .optadd fc 'fc=<file name>'
.* .optadd fh 'fh[=<file name>]'
.* .optadd fhd fhd
.* .optadd fhr fhr
.* .optadd fhw fhw
.* .optadd fhwe fhwe
.* .optadd fi 'fi=<file name>'
.* .optadd fo 'fo=<file name>'
.* .optadd fo2 fo2
.* .optadd fpc fpc
.* .optadd fpi fpi
.* .optadd fpi87 fpi87
.* .optadd fp2 fp2
.* .optadd fp3 fp3
.* .optadd fp5 fp5
.* .optadd fp6 fp6
.* .optadd fpd fpd
.* .optadd fpr fpr
.* .optadd fr 'fr=<file name>'
.* .optadd ft ft
.* .optadd fti fti
.* .optadd fx fx
.* .optadd fzh fzh
.* .optadd fzs fzs
.* .optadd g 'g=<codegroup>'
.* .optadd hw hw
.* .optadd hd hd
.* .optadd hc hc
.* .optadd i 'i=<directory>'
.* .optadd j j
.* .optadd k k
.* .optadd mf mf
.* .optadd ms ms
.* .optadd mm mm
.* .optadd mc mc
.* .optadd ml ml
.* .optadd mh mh
.* .optadd mt mt
.* .optadd nc 'nc=<name>'
.* .optadd nd 'nd=<name>'
.* .optadd nm 'nm=<name>'
.* .optadd nt 'nt=<name>'
.* .optadd oa oa
.* .optadd ob ob
.* .optadd oc oc
.* .optadd od od
.* .optadd oe oe
.* .optadd of of
.* .optadd ofpls of+
.* .optadd oh oh
.* .optadd oi oi
.* .optadd oipls oi+
.* .optadd ok ok
.* .optadd ol ol
.* .optadd olpls ol+
.* .optadd om om
.* .optadd on on
.* .optadd oo oo
.* .optadd op op
.* .optadd or or
.* .optadd os os
.* .optadd ot ot
.* .optadd ou ou
.* .optadd ox ox
.* .optadd oz oz
.* .optadd pil pil
.* .optadd pe pe
.* .optadd pl pl
.* .optadd pc pc
.* .optadd pw 'pw=<num>'
.* .optadd q q
.* .optadd r r
.* .optadd ri ri
.* .optadd s s
.* .optadd sg sg
.* .optadd si si
.* .optadd st st
.* .optadd t 't=<num>'
.* .optadd u 'u<name>'
.* .optadd v v
.* .optadd vc vc
.* .optadd w 'w<num>'
.* .optadd wcd 'wcd=<num>'
.* .optadd wce 'wce=<num>'
.* .optadd we we
.* .optadd wo wo
.* .optadd wx wx
.* .optadd x x
.* .optadd xd xd
.* .optadd xdt xdt
.* .optadd xds xds
.* .optadd xr xr
.* .optadd xs xs
.* .optadd xst xst
.* .optadd xss xss
.* .optadd xx xx
.* .optadd za za
.* .optadd zam zam
.* .optadd zat zat
.* .optadd ze ze
.* .optadd zc zc
.* .optadd zdf zdf
.* .optadd zdp zdp
.* .optadd zdl zdl
.* .optadd zev zev
.* .optadd zf zf
.* .optadd zff zff
.* .optadd zfp zfp
.* .optadd zfw zfw
.* .optadd zg zg
.* .optadd zgf zgf
.* .optadd zgp zgp
.* .optadd zk0 zk0
.* .optadd zk0u zk0u
.* .optadd zk1 zk1
.* .optadd zk2 zk2
.* .optadd zkl zkl
.* .optadd zku 'zku=<codpage>'
.* .optadd zl zl
.* .optadd zld zld
.* .optadd zlf zlf
.* .optadd zls zls
.* .optadd zm zm
.* .optadd zmf zmf
.* .optadd zo zo
.* .optadd zp zp
.* .optadd zp1 zp1
.* .optadd zp2 zp2
.* .optadd zp4 zp4
.* .optadd zp8 zp8
.* .optadd zps zps
.* .optadd zpw zpw
.* .optadd zq zq
.* .optadd zri zri
.* .optadd zro zro
.* .optadd zs zs
.* .optadd zt 'zt<num>'
.* .optadd zu zu
.* .optadd zv zv
.* .optadd zw zw
.* .optadd zW zW
.* .optadd zWs zWs
.* .optadd zz zz
