.386
.MODEL small
.data
foo db ?
bar dw ?
sam dd ?
.CODE
;    rep        lods dx,WORD PTR es:[si]

lodsb
lodsw
lodsd

rep lodsb
rep lodsw
rep lodsd

lods byte ptr [eax]
lods word ptr [eax]
lods dword ptr [eax]

lods foo
lods bar
lods sam

rep lods byte ptr [eax]
rep lods word ptr [eax]
rep lods dword ptr [eax]

rep lods foo
rep lods bar
rep lods sam

end
