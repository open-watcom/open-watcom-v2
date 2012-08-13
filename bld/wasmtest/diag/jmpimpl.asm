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
	jmp c16an
	jmp c16af
;	jmp c16ab
	jmp c16aw
	jmp c16ad
	jmp c16ap
;	jmp c16aq
;	jmp c16at
	jz c16an
	jz c16af
;	jz c16ab
;	jz c16aw
;	jz c16ad
;	jz c16ap
;	jz c16aq
;	jz c16at
;	jmp c16bn
	jmp c16bf
;	jmp c16bb
;	jmp c16bw
;	jmp c16bd
;	jmp c16bp
;	jmp c16bq
;	jmp c16bt
;	jz c16bn
	jz c16bf
;	jz c16bb
;	jz c16bw
;	jz c16bd
;	jz c16bp
;	jz c16bq
;	jz c16bt
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
;	jmp c16an
	jmp c16af
;	jmp c16ab
;	jmp c16aw
;	jmp c16ad
;	jmp c16ap
;	jmp c16aq
;	jmp c16at
;	jz c16an
	jz c16af
;	jz c16ab
;	jz c16aw
;	jz c16ad
;	jz c16ap
;	jz c16aq
;	jz c16at
	jmp c16bn
	jmp c16bf
;	jmp c16bb
	jmp c16bw
	jmp c16bd
	jmp c16bp
;	jmp c16bq
;	jmp c16bt
	jz c16bn
	jz c16bf
;	jz c16bb
;	jz c16bw
;	jz c16bd
;	jz c16bp
;	jz c16bq
;	jz c16bt
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
	jmp c32an
	jmp c32af
;	jmp c32ab
	jmp c32aw
	jmp c32ad
	jmp c32ap
;	jmp c32aq
;	jmp c32at
	jz c32an
	jz c32af
;	jz c32ab
;	jz c32aw
;	jz c32ad
;	jz c32ap
;	jz c32aq
;	jz c32at
;	jmp c32bn
	jmp c32bf
;	jmp c32bb
;	jmp c32bw
;	jmp c32bd
;	jmp c32bp
;	jmp c32bq
;	jmp c32bt
;	jz c32bn
	jz c32bf
;	jz c32bb
;	jz c32bw
;	jz c32bd
;	jz c32bp
;	jz c32bq
;	jz c32bt
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
;	jmp c32an
	jmp c32af
;	jmp c32ab
;	jmp c32aw
;	jmp c32ad
;	jmp c32ap
;	jmp c32aq
;	jmp c32at
;	jz c32an
	jz c32af
;	jz c32ab
;	jz c32aw
;	jz c32ad
;	jz c32ap
;	jz c32aq
;	jz c32at
	jmp c32bn
	jmp c32bf
;	jmp c32bb
	jmp c32bw
	jmp c32bd
	jmp c32bp
;	jmp c32bq
;	jmp c32bt
	jz c32bn
	jz c32bf
;	jz c32bb
;	jz c32bw
;	jz c32bd
;	jz c32bp
;	jz c32bq
;	jz c32bt
_text32b ends

end
