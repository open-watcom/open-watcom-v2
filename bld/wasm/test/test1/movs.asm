.386
.MODEL small
.data
foo db ?
bar dw ?
sam dd ?
.CODE
;    rep        movs dx,WORD PTR es:[si]

movsb
movsw
movsd

rep movsb
rep movsw
rep movsd

movs byte ptr [eax], byte ptr [eax]
movs word ptr [eax], word ptr [eax]
movs dword ptr [eax], dword ptr [eax]

movs foo, foo
movs bar, bar
movs sam, sam

rep movs byte ptr [eax], byte ptr [eax]
rep movs word ptr [eax], word ptr [eax]
rep movs dword ptr [eax], dword ptr [eax]

rep movs foo, foo
rep movs bar, bar
rep movs sam, sam

end
