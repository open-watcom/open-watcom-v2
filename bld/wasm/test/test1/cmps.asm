.386
.MODEL small
.data
foo db ?
bar dw ?
sam dd ?
.CODE
;    rep        cmps dx,WORD PTR es:[si]

cmpsb
cmpsw
cmpsd

repe cmpsb
repe cmpsw
repe cmpsd

cmps foo, foo
cmps bar, bar
cmps sam, sam

repe cmps foo, foo
repe cmps bar, bar
repe cmps sam, sam

cmps byte ptr [eax], byte ptr [eax]
cmps word ptr [eax], word ptr [eax]
cmps dword ptr [eax], dword ptr [eax]

cmps byte ptr cs:[eax], byte ptr [eax]
cmps word ptr cs:[eax], word ptr [eax]
cmps dword ptr cs:[eax], dword ptr [eax]

cmps byte ptr cs:[eax],[eax]
cmps word ptr cs:[eax],[eax]
cmps dword ptr cs:[eax],[eax]

repe cmps byte ptr [eax], byte ptr [eax]
repe cmps word ptr [eax], word ptr [eax]
repe cmps dword ptr [eax], dword ptr [eax]

repe cmps byte ptr cs:[eax], byte ptr [eax]
repe cmps word ptr cs:[eax], word ptr [eax]
repe cmps dword ptr cs:[eax], dword ptr [eax]

repe cmps byte ptr cs:[eax],[eax]
repe cmps word ptr cs:[eax],[eax]
repe cmps dword ptr cs:[eax],[eax]

cmps byte ptr [bx], byte ptr [bx]
cmps word ptr [bx], word ptr [bx]
cmps dword ptr [bx], dword ptr [bx]

cmps byte ptr cs:[bx], byte ptr [bx]
cmps word ptr cs:[bx], word ptr [bx]
cmps dword ptr cs:[bx], dword ptr [bx]

cmps byte ptr cs:[bx],[bx]
cmps word ptr cs:[bx],[bx]
cmps dword ptr cs:[bx],[bx]

repe cmps byte ptr [bx], byte ptr [bx]
repe cmps word ptr [bx], word ptr [bx]
repe cmps dword ptr [bx], dword ptr [bx]

repe cmps byte ptr cs:[bx], byte ptr [bx]
repe cmps word ptr cs:[bx], word ptr [bx]
repe cmps dword ptr cs:[bx], dword ptr [bx]

repe cmps byte ptr cs:[bx],[bx]
repe cmps word ptr cs:[bx],[bx]
repe cmps dword ptr cs:[bx],[bx]

repe cmps byte ptr cs:[si],word ptr es:[di]
repe cmps dword ptr cs:[esi],word ptr es:[edi]

end
