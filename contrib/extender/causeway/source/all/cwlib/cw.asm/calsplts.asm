
	include ..\cwlib.inc
	scode

;-------------------------------------------------------------------------
;
;Calculate splits for all windows.
;
;On Entry:
;
;nothing.
;
;On Exit:
;
;nothing.
;
;ALL registers preserved.
;
CalculateSplits:
	public CalculateSplits
_CalculateSplits proc syscall
	public _CalculateSplits
	local @@SplitX:dword, @@SplitY:dword, @@SplitW:dword, \
	@@SplitD:dword, @@Left:dword, @@Right:dword
	pushad
	;
	mov	esi,offset WinWindowList
	mov	ecx,MaxWinWindows
@@0:	cmp	d[esi],-1
	jz	@@8
	pushm	ecx,esi
	;
	mov	edi,[esi]
	lea	edx,Win_Splits[edi]
	mov	eax,Win_XCoord[edi]
	mov	Split_XCoord[edx],eax
	mov	eax,Win_YCoord[edi]
	mov	Split_YCoord[edx],eax
	mov	eax,Win_Wide[edi]
	mov	Split_Wide[edx],eax
	mov	eax,Win_Depth[edi]
	mov	Split_Depth[edx],eax
	add	edx,size Split
	mov	Split_Wide[edx],0
	;
	mov	esi,offset WinWindowList
	mov	ecx,MaxWinWindows
@@1:	cmp	d[esi],-1
	jz	@@7
	pushm	ecx,esi
	mov	esi,[esi]
	cmp	esi,edi
	jz	@@6
	mov	eax,Win_Priority[esi]
	cmp	eax,Win_Priority[edi]
	jc	@@6
	jz	@@6
;
;Clip EDI's splits against ESI.
;
	lea	edx,Win_Splits[edi]
@@2:	cmp	Split_Wide[edx],0
	jz	@@6
	push	esi
	;
	;Check if coords overlap.
	;
	mov	eax,Win_XCoord[esi]
	mov	ebx,Split_XCoord[edx]
	add	ebx,Split_Wide[edx]
	cmp	eax,ebx		;Left edge beyond right edge?
	jge	@@5
	add	eax,Win_Wide[esi]
	sub	ebx,Split_Wide[edx]
	cmp	eax,ebx		;Right edge below left edge?
	jle	@@5
	mov	eax,Win_YCoord[esi]
	mov	ebx,Split_YCoord[edx]
	add	ebx,Split_Depth[edx]
	cmp	eax,ebx		;Top edge beyond bottom edge?
	jge	@@5
	add	eax,Win_Depth[esi]
	sub	ebx,Split_Depth[edx]
	cmp	eax,ebx		;Bottom edge below Top edge?
	jle	@@5
	;
	;Copy source split into temp space.
	;
	mov	eax,Split_XCoord[edx]
	mov	@@SplitX,eax
	mov	eax,Split_YCoord[edx]
	mov	@@SplitY,eax
	mov	eax,Split_Wide[edx]
	mov	@@SplitW,eax
	mov	eax,Split_Depth[edx]
	mov	@@SplitD,eax
	;
	;Delete this entry.
	;
	lea	ecx,Win_Splits[edi]	;start of split list.
	sub	ecx,edx
	neg	ecx
	add	ecx,size Split
	sub	ecx,MaxSplits*(size Split)
	neg	ecx		;data left to shift.
	push	edi
	push	esi
	mov	esi,edx
	mov	edi,edx
	add	esi,size Split
	rep_movsb		;copy rest down.
	pop	esi
	pop	edi
	;
	;Find current end of the list.
	;
	push	edx
	lea	edx,Win_Splits[edi]
	mov	ecx,MaxSplits
@@3:	cmp	Split_Wide[edx],0
	jz	@@4
	add	edx,size Split
	loop	@@3
	;
@@4:	;Produce new splits.
	;
	mov	@@Left,0
	mov	@@Right,0
	;
	mov	eax,Win_XCoord[esi]	;Get starting X
	sub	eax,@@SplitX		;Get width of left side.
	js	@@NoLeft
	jz	@@NoLeft
	mov	@@Left,eax		;Store for Top & Bottom.
	mov	Split_Wide[edx],eax
	mov	eax,@@SplitX
	mov	Split_XCoord[edx],eax
	mov	eax,@@SplitY
	mov	Split_YCoord[edx],eax
	mov	eax,@@SplitD
	mov	Split_Depth[edx],eax
	add	edx,size Split
	;
@@NoLeft:	mov	eax,@@SplitX		;this one.
	add	eax,@@SplitW
	mov	ebx,Win_XCoord[esi]	;other.
	add	ebx,Win_Wide[esi]
	sub	eax,ebx
	js	@@NoRight
	jz	@@NoRight
	mov	@@Right,eax
	mov	ebx,@@SplitW
	sub	ebx,eax		;Get width differance.
	mov	Split_Wide[edx],eax	;set new width.
	mov	eax,@@SplitX
	add	eax,ebx
	mov	Split_XCoord[edx],eax
	mov	eax,@@SplitY
	mov	Split_YCoord[edx],eax
	mov	eax,@@SplitD
	mov	Split_Depth[edx],eax
	add	edx,size Split
	;
@@NoRight:	mov	eax,Win_YCoord[esi]	;Get starting Y
	sub	eax,@@SplitY		;Get depth of top.
	js	@@NoTop
	jz	@@NoTop
	mov	Split_Depth[edx],eax
	mov	eax,@@SplitX
	add	eax,@@Left
	mov	Split_XCoord[edx],eax
	mov	eax,@@SplitY
	mov	Split_YCoord[edx],eax
	mov	eax,@@SplitW
	sub	eax,@@Left
	sub	eax,@@Right
	mov	Split_Wide[edx],eax
	add	edx,size Split
	;
@@NoTop:	mov	eax,@@SplitY		;this one.
	add	eax,@@SplitD
	mov	ebx,Win_YCoord[esi]	;other.
	add	ebx,Win_Depth[esi]
	sub	eax,ebx
	js	@@NoBottom
	jz	@@NoBottom
	mov	ebx,@@SplitD
	sub	ebx,eax		;Get depth differance.
	mov	Split_Depth[edx],eax	;set new depth.
	mov	eax,@@SplitY
	add	eax,ebx
	mov	Split_YCoord[edx],eax
	mov	eax,@@SplitX
	add	eax,@@Left
	mov	Split_XCoord[edx],eax
	mov	eax,@@SplitW
	sub	eax,@@Left
	sub	eax,@@Right
	mov	Split_Wide[edx],eax
	add	edx,size Split
	;
@@NoBottom:	mov	Split_Wide[edx],0	;make sure list is terminated.
	pop	edx
	sub	edx,size Split
	;
@@5:	pop	esi
	add	edx,size Split
	jmp	@@2
	;
@@6:	popm	ecx,esi
@@7:	add	esi,4
	dec	ecx
	jnz	@@1
	popm	ecx,esi
@@8:	add	esi,4
	dec	ecx
	jnz	@@0
	;
	popad
	ret
_CalculateSplits endp


	efile
	end

