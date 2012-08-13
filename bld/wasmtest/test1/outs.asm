.386
.MODEL small
.data
foo db ?
bar dw ?
sam dd ?
.CODE
;    rep        outs dx,WORD PTR es:[si]

outsb
outsw
outsd

rep outsb
rep outsw
rep outsd

outs dx, foo
outs dx, bar
outs dx, sam

rep outs dx, foo
rep outs dx, bar
rep outs dx, sam

outs dx, byte ptr [eax]
outs dx, word ptr [eax]
outs dx, dword ptr [eax]

rep outs dx, byte ptr [eax]
rep outs dx, word ptr [eax]
rep outs dx, dword ptr [eax]

outs dx, byte ptr cs:[eax]
outs dx, word ptr cs:[eax]
outs dx, dword ptr cs:[eax]

rep outs dx, byte ptr cs:[eax]
rep outs dx, word ptr cs:[eax]
rep outs dx, dword ptr cs:[eax]

outs dx, byte ptr [bx]
outs dx, word ptr [bx]
outs dx, dword ptr [bx]

rep outs dx, byte ptr [bx]
rep outs dx, word ptr [bx]
rep outs dx, dword ptr [bx]

outs dx, byte ptr cs:[bx]
outs dx, word ptr cs:[bx]
outs dx, dword ptr cs:[bx]

rep outs dx, byte ptr cs:[bx]
rep outs dx, word ptr cs:[bx]
rep outs dx, dword ptr cs:[bx]

end
