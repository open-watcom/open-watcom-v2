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

outs dx, byte ptr [eax]
outs dx, word ptr [eax]
outs dx, dword ptr [eax]

outs dx, foo
outs dx, bar
outs dx, sam

rep outs dx, byte ptr [eax]
rep outs dx, word ptr [eax]
rep outs dx, dword ptr [eax]

rep outs dx, foo
rep outs dx, bar
rep outs dx, sam

end
