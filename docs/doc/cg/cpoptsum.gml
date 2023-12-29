.if '&alpha' eq 'AXP' .do begin
.sr x86only='(x86 only)'
.sr 286only='(x86 16-bit only)'
.sr 386only='(x86 32-bit only)'
.sr AXPonly='(AXP only)'
.do end
.el .do begin
.sr x86only=''
.sr 286only='(16-bit only)'
.sr 386only='(32-bit only)'
.sr AXPonly='????should not appear????'
.do end
.*
.optdsp 0
.optdsp 1
.optdsp 2
.optdsp 3
.optdsp 4
.optdsp 5
.optdsp 6
.optdsp 3rs
.optdsp 4rs
.optdsp 5rs
.optdsp 6rs
.optdsp aa
.optdsp ad
.optdsp adbs
.optdsp add
.optdsp adhp
.optdsp adfs
.optdsp adt
.if '&alpha' eq 'AXP' .do begin
.optdsp as
.do end
.optdsp bc
.optdsp bd
.optdsp bg
.optdsp bm
.optdsp br
.optdsp bt
.optdsp bw
.optdsp d0
.optdsp d1
.optdsp d1pls
.optdsp d2
.optdsp d2i
.optdsp d2s
.optdsp d2t
.optdsp d3
.optdsp d3i
.optdsp d3s
.optdsp d
.optdsp dpls
.optdsp db
.optdsp e
.optdsp ecc
.optdsp ecd
.optdsp ecf
.* .optdsp eco
.optdsp ecp
.optdsp ecr
.optdsp ecs
.optdsp ecw
.optdsp ee
.optdsp ef
.optdsp ei
.optdsp em
.optdsp en
.optdsp ep
.optdsp eq
.optdsp er
.optdsp et
.optdsp ew
.optdsp ez
.optdsp fc
.optdsp fh
.optdsp fhd
.optdsp fhr
.optdsp fhw
.optdsp fhwe
.optdsp fi
.optdsp fo
.optdsp fo2
.optdsp fpc
.optdsp fpi
.optdsp fpix
.optdsp fpi87
.optdsp fpi87x
.optdsp fp2
.optdsp fp3
.optdsp fp5
.optdsp fp6
.optdsp fpd
.optdsp fpr
.optdsp fr
.optdsp ft
.optdsp fti
.optdsp fx
.optdsp fzh
.optdsp fzs
.optdsp g
.if '&alpha' eq 'AXP' .do begin
.optdsp hdc
.do end
.el .do begin
.optdsp hwdc
.do end
.optdsp i
.optdsp j
.optdsp k
.optdsp mfsmclh
(default is "ms" for 16-bit and Netware, "mf" for 32-bit)
.if &e'&wcldesc. ne 0 .do begin
.optdsp mt
.do end
.optdsp nc
.optdsp nd
.optdsp nm
.optdsp nt
.*
.note o{a,b,c,d,e,f,f+,h,i,i+,k,l,l+,m,n,o,p,r,s,t,u,x,z}
:CMT. .ix 'options' 'oa'
.sr $SWoa=1
:CMT. .ix 'options' 'ob'
.sr $SWob=1
:CMT. .ix 'options' 'oc'
.sr $SWoc=1
:CMT. .ix 'options' 'od'
.sr $SWod=1
:CMT. .ix 'options' 'oe'
.sr $SWoe=1
:CMT. .ix 'options' 'of'
.sr $SWof=1
:CMT. .ix 'options' 'of+'
.sr $SWofpls=1
:CMT. .ix 'options' 'oh'
.sr $SWoh=1
:CMT. .ix 'options' 'oi'
.sr $SWoi=1
:CMT. .ix 'options' 'oi+'
.sr $SWoipls=1
:CMT. .ix 'options' 'ok'
.sr $SWok=1
:CMT. .ix 'options' 'ol'
.sr $SWol=1
:CMT. .ix 'options' 'ol+'
.sr $SWolpls=1
:CMT. .ix 'options' 'om'
.sr $SWom=1
:CMT. .ix 'options' 'on'
.sr $SWon=1
:CMT. .ix 'options' 'oo'
.sr $SWoo=1
:CMT. .ix 'options' 'op'
.sr $SWop=1
:CMT. .ix 'options' 'or'
.sr $SWor=1
:CMT. .ix 'options' 'os'
.sr $SWos=1
:CMT. .ix 'options' 'ot'
.sr $SWot=1
:CMT. .ix 'options' 'ou'
.sr $SWou=1
:CMT. .ix 'options' 'ox'
.sr $SWox=1
:CMT. .ix 'options' 'oz'
.sr $SWoz=1
.*
&x86only.
control optimization
:optref refid='SWoa'.
:optref refid='SWof'.
.*
.if '&alpha' eq 'AXP' .do begin
.note o{a,d,e,i,l,n,o,r,s,t,u,x,z}
:CMT. .ix 'options' 'oa'
.sr $SWoa=1
:CMT. .ix 'options' 'od'
.sr $SWod=1
:CMT. .ix 'options' 'oe'
.sr $SWoe=1
:CMT. .ix 'options' 'oi'
.sr $SWoi=1
:CMT. .ix 'options' 'ol'
.sr $SWol=1
:CMT. .ix 'options' 'on'
.sr $SWon=1
:CMT. .ix 'options' 'oo'
.sr $SWoo=1
:CMT. .ix 'options' 'or'
.sr $SWor=1
:CMT. .ix 'options' 'os'
.sr $SWos=1
:CMT. .ix 'options' 'ot'
.sr $SWot=1
:CMT. .ix 'options' 'ou'
.sr $SWou=1
:CMT. .ix 'options' 'ox'
.sr $SWox=1
:CMT. .ix 'options' 'oz'
.sr $SWoz=1
&AXPonly.
control optimization
:optref refid='SWoa'.
.do end
.*
.optdsp pil
.optdsp p
.optdsp q
.optdsp r
.optdsp ri
.optdsp s
.optdsp sg
.if '&alpha' eq 'AXP' .do begin
.optdsp si
.do end
.optdsp st
.optdsp t
.optdsp u
.optdsp v
.*
.note vc...
:CMT. .ix 'options' 'vc'
(C++ only)
VC++ compatibility options
:optref refid='SWvc'.
.*
.optdsp w
.optdsp wcd
.optdsp wce
.optdsp we
.optdsp wo
.optdsp wx
.optdsp x
.optdsp xd
.optdsp xdt
.optdsp xds
.optdsp xr
.optdsp xs
.optdsp xst
.optdsp xss
.optdsp xx
.optdsp za
.optdsp ze
.optdsp zam
.optdsp zat
.optdsp zc
.optdsp zdfp
.optdsp zdl
.optdsp zev
.optdsp zf
.optdsp zffp
.optdsp zfw
.optdsp zg
.optdsp zgfp
.optdsp zk0
.optdsp zk0u
.optdsp zk1
.optdsp zk2
.optdsp zkl
.optdsp zku
.optdsp zl
.optdsp zld
.optdsp zlf
.optdsp zls
.optdsp zm
.optdsp zmf
.* .optdsp zo
.optdsp zp
.optnote zp1 zp2 zp4 zp8 zp16
.if '&alpha' eq 'AXP' .do begin
.optdsp zps
.do end
.optdsp zpw
.optdsp zq
.optdsp zri
.optdsp zro
.optdsp zs
.optdsp zt
.optdsp zu
.optdsp zv
.optdsp zw
.optdsp zW
.optdsp zWs
.optdsp zz
.*
