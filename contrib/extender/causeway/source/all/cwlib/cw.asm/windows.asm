	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Initialise Window's stuff so base handler gets a lookin.
;
WinInitialise	proc	near
	call	CalculateSplits
	ret
WinInitialise	endp


;-------------------------------------------------------------------------
;
;Provides region clearing for the background when windows are closed.
;
BaseHandler	proc	near
	cmp	eax,WinMess_Draw
	jnz	@@9
	push	32767
	push	32767
	push	0
	push	0
	xor	eax,eax
	test	VideoModeFlags,8
	jz	@@0
	mov	ax,0720h
@@0:	push	eax
	push	0
	push	esi
	call	WinFillRectangle
	add	esp,4*7
@@9:	ret
BaseHandler	endp


	sdata

;-------------------------------------------------------------------------
;
;Some variables common to several routines.
;
BaseWindow	WIN <0,0,0,32767,32767,BaseHandler>
;
WinWindowList	label dword
	dd BaseWindow, MaxWinWindows dup (-1)
;
WindowRegionFlag dd 0	;Forced region clipping?
WindowRegionX	dd ?
WindowRegionY	dd ?
WindowRegionW	dd ?
WindowRegionD	dd ?
;
FontBuffer	label byte
	db 1024 dup (?)


	efile
	end

