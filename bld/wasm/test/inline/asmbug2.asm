 in al,dx
 lds si,dword ptr x
 lds esi,fword ptr x
 mov edi,cr2
 mov cr2,esi
 mov esi,dr2
 mov dr2,ecx
 mov esi,tr4
 mov tr4,ebx
 movsx ax,byte ptr x
 movsx eax,byte ptr x
 movzx ax,byte ptr x
 movzx eax,byte ptr x
