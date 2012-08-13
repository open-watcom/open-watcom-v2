.386p
.387
_TEXT		SEGMENT	DWORD USE32 'CODE'
		ASSUME CS:_TEXT
    mov         ax,ss:[bp+di]
    mov         ax,ss:[di+bp]
    mov         ax,ss:[bp+si]
    mov         ax,ss:[si+bp]
    mov         ax,ss:[bx+di]
    mov         ax,ss:[di+bx]
    mov         ax,ss:[bx+si]
    mov         ax,ss:[si+bx]
    mov         ax,ss:[bp][di]
    mov         ax,ss:[di][bp]
    mov         ax,ss:[bp][si]
    mov         ax,ss:[si][bp]
    mov         ax,ss:[bx][di]
    mov         ax,ss:[di][bx]
    mov         ax,ss:[bx][si]
    mov         ax,ss:[si][bx]
    mov         ax,ss:[bp]
    mov         ax,ss:[bx]
    mov         ax,ss:[si]
    mov         ax,ss:[di]

    mov         ax,ss:[ebp+ebx] ; MASM 6 bug
    mov         ax,ss:[ebx+ebp] ; MASM 6 bug
    mov         ax,ss:[ebp*2+ebx] ; MASM 6 bug
    mov         ax,ss:[ebx*2+ebp] ; MASM 6 bug
    mov         ax,ss:[ebp+ebx*2]
    mov         ax,ss:[ebx+ebp*2]
    mov         ax,ss:[ebp][ebx] ; MASM 6 bug
    mov         ax,ss:[ebx][ebp] ; MASM 6 bug
    mov         ax,ss:[ebp*2][ebx]
    mov         ax,ss:[ebx*2][ebp]
    mov         ax,ss:[ebp][ebx*2]
    mov         ax,ss:[ebx][ebp*2]
    mov         ax,ss:[ebp]
    mov         ax,ss:[ebx]
    mov         ax,ss:[ebx*2]
    mov         ax,ss:[ebp*2]

    mov         ax,ss:[esp+ebx]
    mov         ax,ss:[ebx+esp]
    mov         ax,ss:[ebx*2+esp] ; MASM 6 bug
    mov         ax,ss:[esp+ebx*2]
    mov         ax,ss:[esp][ebx]
    mov         ax,ss:[ebx][esp]
    mov         ax,ss:[ebx*2][esp]
    mov         ax,ss:[esp][ebx*2]
    mov         ax,ss:[esp]
    mov         ax,ss:[ebx]
    mov         ax,ss:[ebx*2]

    mov         ax,ds:[bp+di]
    mov         ax,ds:[di+bp]
    mov         ax,ds:[bp+si]
    mov         ax,ds:[si+bp]
    mov         ax,ds:[bx+di]
    mov         ax,ds:[di+bx]
    mov         ax,ds:[bx+si]
    mov         ax,ds:[si+bx]
    mov         ax,ds:[bp][di]
    mov         ax,ds:[di][bp]
    mov         ax,ds:[bp][si]
    mov         ax,ds:[si][bp]
    mov         ax,ds:[bx][di]
    mov         ax,ds:[di][bx]
    mov         ax,ds:[bx][si]
    mov         ax,ds:[si][bx]
    mov         ax,ds:[bp]
    mov         ax,ds:[bx]
    mov         ax,ds:[si]
    mov         ax,ds:[di]

    mov         ax,ds:[ebp+ebx] ; MASM 6 bug
    mov         ax,ds:[ebx+ebp] ; MASM 6 bug
    mov         ax,ds:[ebp*2+ebx] ; MASM 6 bug
    mov         ax,ds:[ebx*2+ebp] ; MASM 6 bug
    mov         ax,ds:[ebp+ebx*2]
    mov         ax,ds:[ebx+ebp*2]
    mov         ax,ds:[ebp][ebx] ; MASM 6 bug
    mov         ax,ds:[ebx][ebp] ; MASM 6 bug
    mov         ax,ds:[ebp*2][ebx]
    mov         ax,ds:[ebx*2][ebp]
    mov         ax,ds:[ebp][ebx*2]
    mov         ax,ds:[ebx][ebp*2]
    mov         ax,ds:[ebp]
    mov         ax,ds:[ebx]
    mov         ax,ds:[ebx*2]
    mov         ax,ds:[ebp*2]

    mov         ax,ds:[esp+ebx]
    mov         ax,ds:[ebx+esp]
    mov         ax,ds:[ebx*2+esp] ; MASM 6 bug
    mov         ax,ds:[esp+ebx*2]
    mov         ax,ds:[esp][ebx]
    mov         ax,ds:[ebx][esp]
    mov         ax,ds:[ebx*2][esp]
    mov         ax,ds:[esp][ebx*2]
    mov         ax,ds:[esp]
    mov         ax,ds:[ebx]
    mov         ax,ds:[ebx*2]
_TEXT		ENDS
_TEXT1		SEGMENT	DWORD USE16 'CODE'
		ASSUME CS:_TEXT1
    mov         ax,ss:[bp+di]
    mov         ax,ss:[di+bp]
    mov         ax,ss:[bp+si]
    mov         ax,ss:[si+bp]
    mov         ax,ss:[bx+di]
    mov         ax,ss:[di+bx]
    mov         ax,ss:[bx+si]
    mov         ax,ss:[si+bx]
    mov         ax,ss:[bp][di]
    mov         ax,ss:[di][bp]
    mov         ax,ss:[bp][si]
    mov         ax,ss:[si][bp]
    mov         ax,ss:[bx][di]
    mov         ax,ss:[di][bx]
    mov         ax,ss:[bx][si]
    mov         ax,ss:[si][bx]
    mov         ax,ss:[bp]
    mov         ax,ss:[bx]
    mov         ax,ss:[si]
    mov         ax,ss:[di]

    mov         ax,ss:[ebp+ebx] ; MASM 6 bug
    mov         ax,ss:[ebx+ebp] ; MASM 6 bug
    mov         ax,ss:[ebp*2+ebx] ; MASM 6 bug
    mov         ax,ss:[ebx*2+ebp] ; MASM 6 bug
    mov         ax,ss:[ebp+ebx*2]
    mov         ax,ss:[ebx+ebp*2]
    mov         ax,ss:[ebp][ebx] ; MASM 6 bug
    mov         ax,ss:[ebx][ebp] ; MASM 6 bug
    mov         ax,ss:[ebp*2][ebx]
    mov         ax,ss:[ebx*2][ebp]
    mov         ax,ss:[ebp][ebx*2]
    mov         ax,ss:[ebx][ebp*2]
    mov         ax,ss:[ebp]
    mov         ax,ss:[ebx]
    mov         ax,ss:[ebx*2]
    mov         ax,ss:[ebp*2]

    mov         ax,ss:[esp+ebx]
    mov         ax,ss:[ebx+esp]
    mov         ax,ss:[ebx*2+esp] ; MASM 6 bug
    mov         ax,ss:[esp+ebx*2]
    mov         ax,ss:[esp][ebx]
    mov         ax,ss:[ebx][esp]
    mov         ax,ss:[ebx*2][esp]
    mov         ax,ss:[esp][ebx*2]
    mov         ax,ss:[esp]
    mov         ax,ss:[ebx]
    mov         ax,ss:[ebx*2]

    mov         ax,ds:[bp+di]
    mov         ax,ds:[di+bp]
    mov         ax,ds:[bp+si]
    mov         ax,ds:[si+bp]
    mov         ax,ds:[bx+di]
    mov         ax,ds:[di+bx]
    mov         ax,ds:[bx+si]
    mov         ax,ds:[si+bx]
    mov         ax,ds:[bp][di]
    mov         ax,ds:[di][bp]
    mov         ax,ds:[bp][si]
    mov         ax,ds:[si][bp]
    mov         ax,ds:[bx][di]
    mov         ax,ds:[di][bx]
    mov         ax,ds:[bx][si]
    mov         ax,ds:[si][bx]
    mov         ax,ds:[bp]
    mov         ax,ds:[bx]
    mov         ax,ds:[si]
    mov         ax,ds:[di]

    mov         ax,ds:[ebp+ebx] ; MASM 6 bug
    mov         ax,ds:[ebx+ebp] ; MASM 6 bug
    mov         ax,ds:[ebp*2+ebx] ; MASM 6 bug
    mov         ax,ds:[ebx*2+ebp] ; MASM 6 bug
    mov         ax,ds:[ebp+ebx*2]
    mov         ax,ds:[ebx+ebp*2]
    mov         ax,ds:[ebp][ebx] ; MASM 6 bug
    mov         ax,ds:[ebx][ebp] ; MASM 6 bug
    mov         ax,ds:[ebp*2][ebx]
    mov         ax,ds:[ebx*2][ebp]
    mov         ax,ds:[ebp][ebx*2]
    mov         ax,ds:[ebx][ebp*2]
    mov         ax,ds:[ebp]
    mov         ax,ds:[ebx]
    mov         ax,ds:[ebx*2]
    mov         ax,ds:[ebp*2]

    mov         ax,ds:[esp+ebx]
    mov         ax,ds:[ebx+esp]
    mov         ax,ds:[ebx*2+esp] ; MASM 6 bug
    mov         ax,ds:[esp+ebx*2]
    mov         ax,ds:[esp][ebx]
    mov         ax,ds:[ebx][esp]
    mov         ax,ds:[ebx*2][esp]
    mov         ax,ds:[esp][ebx*2]
    mov         ax,ds:[esp]
    mov         ax,ds:[ebx]
    mov         ax,ds:[ebx*2]
_TEXT1		ENDS
		END
