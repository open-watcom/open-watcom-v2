.386
.MODEL small
.data
foo db ?
bar dw ?
sam dd ?
.CODE
;    rep        scas dx,WORD PTR es:[si]

scasb
scasw
scasd

rep scasb
rep scasw
rep scasd

scas byte ptr [eax]
scas word ptr [eax]
scas dword ptr [eax]

scas foo
scas bar
scas sam

rep scas byte ptr [eax]
rep scas word ptr [eax]
rep scas dword ptr [eax]

rep scas foo
rep scas bar
rep scas sam

end
