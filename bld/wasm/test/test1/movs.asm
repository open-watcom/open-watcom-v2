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

movs foo, foo
movs bar, bar
movs sam, sam

rep movs foo, foo
rep movs bar, bar
rep movs sam, sam

movs byte ptr [eax], byte ptr [eax]
movs word ptr [eax], word ptr [eax]
movs dword ptr [eax], dword ptr [eax]

rep movs byte ptr [eax], byte ptr [eax]
rep movs word ptr [eax], word ptr [eax]
rep movs dword ptr [eax], dword ptr [eax]

movs byte ptr [eax], byte ptr cs:[eax]
movs word ptr [eax], word ptr cs:[eax]
movs dword ptr [eax], dword ptr cs:[eax]

rep movs byte ptr [eax], byte ptr cs:[eax]
rep movs word ptr [eax], word ptr cs:[eax]
rep movs dword ptr [eax], dword ptr cs:[eax]

movs byte ptr [bx], byte ptr [bx]
movs word ptr [bx], word ptr [bx]
movs dword ptr [bx], dword ptr [bx]

rep movs byte ptr [bx], byte ptr [bx]
rep movs word ptr [bx], word ptr [bx]
rep movs dword ptr [bx], dword ptr [bx]

movs byte ptr [bx], byte ptr cs:[bx]
movs word ptr [bx], word ptr cs:[bx]
movs dword ptr [bx], dword ptr cs:[bx]

rep movs byte ptr [bx], byte ptr cs:[bx]
rep movs word ptr [bx], word ptr cs:[bx]
rep movs dword ptr [bx], dword ptr cs:[bx]

end
