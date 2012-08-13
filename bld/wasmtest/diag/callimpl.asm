.386

_text16a segment use16 'CODE'
assume cs:_text16a
	nop
	nop
	nop
c16an label near
c16af label far
c16ab label byte
c16aw label word
c16ad label dword
c16ap label fword
c16aq label qword
c16at label tbyte
	nop
	call c16an
	call c16af
;	call c16ab
	call c16aw
	call c16ad
	call c16ap
;	call c16aq
;	call c16at
;	call c16bn
	call c16bf
;	call c16bb
;	call c16bw
;	call c16bd
;	call c16bp
;	call c16bq
;	call c16bt
_text16a ends

_text16b segment use16 'CODE'
assume cs:_text16b
	nop
	nop
	nop
c16bn label near
c16bf label far
c16bb label byte
c16bw label word
c16bd label dword
c16bp label fword
c16bq label qword
c16bt label tbyte
	nop
;	call c16an
	call c16af
;	call c16ab
;	call c16aw
;	call c16ad
;	call c16ap
;	call c16aq
;	call c16at
	call c16bn
	call c16bf
;	call c16bb
	call c16bw
	call c16bd
	call c16bp
;	call c16bq
;	call c16bt
_text16b ends

_text32a segment use32 'CODE'
assume cs:_text32a
	nop
	nop
	nop
c32an label near
c32af label far
c32ab label byte
c32aw label word
c32ad label dword
c32ap label fword
c32aq label qword
c32at label tbyte
	nop
	call c32an
	call c32af
;	call c32ab
	call c32aw
	call c32ad
	call c32ap
;	call c32aq
;	call c32at
;	call c32bn
	call c32bf
;	call c32bb
;	call c32bw
;	call c32bd
;	call c32bp
;	call c32bq
;	call c32bt
_text32a ends

_text32b segment use32 'CODE'
assume cs:_text32b
	nop
	nop
	nop
c32bn label near
c32bf label far
c32bb label byte
c32bw label word
c32bd label dword
c32bp label fword
c32bq label qword
c32bt label tbyte
	nop
;	call c32an
	call c32af
;	call c32ab
;	call c32aw
;	call c32ad
;	call c32ap
;	call c32aq
;	call c32at
	call c32bn
	call c32bf
;	call c32bb
	call c32bw
	call c32bd
	call c32bp
;	call c32bq
;	call c32bt
_text32b ends

end
