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

rep cmpsb
rep cmpsw
rep cmpsd

cmps byte ptr [eax], byte ptr [eax]
cmps word ptr [eax], word ptr [eax]
cmps dword ptr [eax], dword ptr [eax]

cmps foo, foo
cmps bar, bar
cmps sam, sam

rep cmps byte ptr [eax], byte ptr [eax]
rep cmps word ptr [eax], word ptr [eax]
rep cmps dword ptr [eax], dword ptr [eax]

rep cmps foo, foo
rep cmps bar, bar
rep cmps sam, sam

end
