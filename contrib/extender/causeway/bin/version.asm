;
;Example to detect if running under CauseWay and to get version if so.
;

	.model small
	.386
	.stack 1024

	.code

start	proc	near
	mov	ax,DGROUP
	mov	ds,ax

;
;Get CauseWay API interrupt vector address.
;
	mov	ax,3531h
	int	21h
;
;Check segment/selector isn't zero.
;
	mov	ax,es
	or	ax,ax
	jz	@@NotCauseway
;
;Move back to where the CauseWay ID should be and check for it.
;
;CauseWay API handler is preceded by:
;
; DB "CAUSEWAY"
; DB MajorVersion
; DB MinorVersion
;
;MajorVersion is in the range 0-255
;MinorVersion is in the range 0-99
;
;Both version numbers are binary, ie, NOT ASCII or BCD.
;
	sub	bx,10
	cmp	es:dword ptr[bx],"SUAC"
	jnz	@@NotCauseway
	cmp	es:dword ptr[bx+4],"YAWE"
	jnz	@@NotCauseway
;
;Running under CauseWay so let the user know.
;
	mov	dx,offset IsCauseWayText
	mov	ah,9
	int	21h
	jmp	@@Done
;
;Not running under CauseWay so let the user know.
;
@@NotCauseway:	mov	dx,offset NotCauseWayText
	mov	ah,9
	int	21h
;
;Exit to DOS/CauseWay
;
@@Done:	mov	ax,4c00h
	int	21h
start	endp

	.data

IsCauseWayText	db "Running under CauseWay.",13,10,"$"
NotCauseWayText db "Not running under CauseWay.",13,10,"$"

	end start
