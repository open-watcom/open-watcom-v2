.386
.MODEL small
.data
foo db ?
bar dw ?
sam dd ?
.CODE
;    rep        ins dx,WORD PTR es:[si]

insb
insw
insd

rep insb
rep insw
rep insd

ins foo, dx
ins bar, dx
ins sam, dx

rep ins foo, dx
rep ins bar, dx
rep ins sam, dx

ins byte ptr [eax], dx
ins word ptr [eax], dx
ins dword ptr [eax], dx

rep ins byte ptr [eax], dx
rep ins word ptr [eax], dx
rep ins dword ptr [eax], dx

ins byte ptr [bx], dx
ins word ptr [bx], dx
ins dword ptr [bx], dx

rep ins byte ptr [bx], dx
rep ins word ptr [bx], dx
rep ins dword ptr [bx], dx

end
