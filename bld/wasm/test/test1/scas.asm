.386
.MODEL small
.data
foo db ?
bar dw ?
sam dd ?
.CODE
;    repe        scas dx,WORD PTR es:[si]

scasb
scasw
scasd

repe scasb
repe scasw
repe scasd

scas foo
scas bar
scas sam

repe scas foo
repe scas bar
repe scas sam

scas byte ptr [eax]
scas word ptr [eax]
scas dword ptr [eax]

repe scas byte ptr [eax]
repe scas word ptr [eax]
repe scas dword ptr [eax]

scas byte ptr [bx]
scas word ptr [bx]
scas dword ptr [bx]

repe scas byte ptr [bx]
repe scas word ptr [bx]
repe scas dword ptr [bx]

end
