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

lods foo
lods bar
lods sam

rep lods foo
rep lods bar
rep lods sam

lods byte ptr [eax]
lods word ptr [eax]
lods dword ptr [eax]

rep lods byte ptr [eax]
rep lods word ptr [eax]
rep lods dword ptr [eax]

lods byte ptr cs:[eax]
lods word ptr cs:[eax]
lods dword ptr cs:[eax]

rep lods byte ptr cs:[eax]
rep lods word ptr cs:[eax]
rep lods dword ptr cs:[eax]

lods byte ptr [bx]
lods word ptr [bx]
lods dword ptr [bx]

rep lods byte ptr [bx]
rep lods word ptr [bx]
rep lods dword ptr [bx]

lods byte ptr cs:[bx]
lods word ptr cs:[bx]
lods dword ptr cs:[bx]

rep lods byte ptr cs:[bx]
rep lods word ptr cs:[bx]
rep lods dword ptr cs:[bx]

end
