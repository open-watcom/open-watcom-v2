.386
.MODEL small
.data
foo db ?
bar dw ?
sam dd ?
.CODE
;    rep        stos dx,WORD PTR es:[si]

stosb
stosw
stosd

rep stosb
rep stosw
rep stosd

stos foo
stos bar
stos sam

rep stos foo
rep stos bar
rep stos sam

stos byte ptr [eax]
stos word ptr [eax]
stos dword ptr [eax]

rep stos byte ptr [eax]
rep stos word ptr [eax]
rep stos dword ptr [eax]

stos byte ptr [bx]
stos word ptr [bx]
stos dword ptr [bx]

rep stos byte ptr [bx]
rep stos word ptr [bx]
rep stos dword ptr [bx]

end
