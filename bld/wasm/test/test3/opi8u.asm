.386

_TEXT16 segment 'CODE' USE16
	in al,0ffh
	in ax,0ffh
	in eax,0ffh
	in al,7fh
	in ax,7fh
	in eax,7fh
	out 0ffh,al
	out 0ffh,ax
	out 0ffh,eax
	out 7fh,al
	out 7fh,ax
	out 7fh,eax
	int 0ffh
	int 7fh
	shl ax,0ffh
	shl ax,7fh
	shl eax,0ffh
	shl eax,7fh
_TEXT16 ends

_TEXT32 segment 'CODE' USE32
	in al,0ffh
	in ax,0ffh
	in eax,0ffh
	in al,7fh
	in ax,7fh
	in eax,7fh
	out 0ffh,al
	out 0ffh,ax
	out 0ffh,eax
	out 7fh,al
	out 7fh,ax
	out 7fh,eax
	int 0ffh
	int 7fh
	shl ax,0ffh
	shl ax,7fh
	shl eax,0ffh
	shl eax,7fh
_TEXT32 ends

end
