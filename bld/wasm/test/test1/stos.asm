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

stos byte ptr [eax]
stos word ptr [eax]
stos dword ptr [eax]

stos foo
stos bar
stos sam

rep stos byte ptr [eax]
rep stos word ptr [eax]
rep stos dword ptr [eax]

rep stos foo
rep stos bar
rep stos sam

end
