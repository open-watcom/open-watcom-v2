sbb al,0fH
sbb al,7fH
sbb al,0ffH
sbb ax,000fH
sbb ax,007fH
sbb ax,00ffH
sbb ax,7fffH
sbb ax,0ffffH
sbb eax,0000000fH
sbb eax,0000007fH
sbb eax,000000ffH
sbb eax,00007fffH
sbb eax,0000ffffH
sbb eax,7fffffffH
sbb eax,0ffffffffH
sbb dl,0fH
sbb dl,7fH
sbb dl,0ffH
sbb dx,000fH
sbb dx,007fH
sbb dx,00ffH
sbb dx,7fffH
sbb dx,0ffffH
sbb edx,0000000fH
sbb edx,0000007fH
sbb edx,000000ffH
sbb edx,00007fffH
sbb edx,0000ffffH
sbb edx,7fffffffH
sbb edx,0ffffffffH
sbb byte ptr x,0fH
sbb byte ptr x,7fH
sbb byte ptr x,0ffH
sbb word ptr x,000fH
sbb word ptr x,007fH
sbb word ptr x,00ffH
sbb word ptr x,7fffH
sbb word ptr x,0ffffH
sbb dword ptr x,0000000fH
sbb dword ptr x,0000007fH
sbb dword ptr x,000000ffH
sbb dword ptr x,00007fffH
sbb dword ptr x,0000ffffH
sbb dword ptr x,7fffffffH
sbb dword ptr x,0ffffffffH
sbb byte ptr x,dl
sbb word ptr x,dx
sbb dword ptr x,edx
sbb dl,byte ptr x
sbb dx,word ptr x
sbb edx,dword ptr x
sbb dl,bl
sbb dx,bx
sbb edx,ebx
sbb al,bl
sbb ax,bx
sbb eax,ebx
sbb dl,al
sbb dx,ax
sbb edx,eax
sbb edx,ebx
sbb edx,ecx
sbb edx,edx
sbb edx,esi
sbb edx,edi
sbb edx,esp
sbb edx,ebp
sbb edx,edi
sbb dx,ax
sbb dx,bx
sbb dx,cx
sbb dx,dx
sbb dx,si
sbb dx,di
sbb dx,sp
sbb dx,bp
sbb dx,di
sbb dl,al
sbb dl,bl
sbb dl,cl
sbb dl,dl
sbb dl,ah
sbb dl,bh
sbb dl,ch
sbb dl,dh
sbb eax,dword ptr [eax]
sbb eax,dword ptr [edx]
sbb eax,dword ptr [ebx]
sbb eax,dword ptr [ecx]
sbb eax,dword ptr [esi]
sbb eax,dword ptr [edi]
sbb eax,dword ptr [esp]
sbb eax,dword ptr +0H[ebp]
sbb eax,dword ptr +0fH[edx]
sbb eax,dword ptr +7fH[edx]
sbb eax,dword ptr +7fffH[edx]
sbb eax,dword ptr +0ffffH[edx]
sbb eax,dword ptr +7fffffffH[edx]
sbb eax,dword ptr -1H[edx]
sbb edx,dword ptr [eax]
sbb edx,dword ptr [edx]
sbb edx,dword ptr [ebx]
sbb edx,dword ptr [ecx]
sbb edx,dword ptr [esi]
sbb edx,dword ptr [edi]
sbb edx,dword ptr [esp]
sbb edx,dword ptr +0H[ebp]
sbb edx,dword ptr +0fH[edx]
sbb edx,dword ptr +7fH[edx]
sbb edx,dword ptr +7fffH[edx]
sbb edx,dword ptr +0ffffH[edx]
sbb edx,dword ptr +7fffffffH[edx]
sbb edx,dword ptr -1H[edx]
sbb edx,dword ptr [eax+esi]
sbb edx,dword ptr [edx+esi]
sbb edx,dword ptr [ebx+esi]
sbb edx,dword ptr [ecx+esi]
sbb edx,dword ptr [esi+esi]
sbb edx,dword ptr [edi+esi]
sbb edx,dword ptr [esp+esi]
sbb edx,dword ptr +0H[ebp+esi]
sbb edx,dword ptr +0fH[edx+esi]
sbb edx,dword ptr +7fH[edx+esi]
sbb edx,dword ptr +7fffH[edx+esi]
sbb edx,dword ptr +0ffffH[edx+esi]
sbb edx,dword ptr +7fffffffH[edx+esi]
sbb edx,dword ptr -1H[edx+esi]
sbb edx,dword ptr -0fH[edx+esi*2]
sbb edx,dword ptr -7fffH[edx+esi*4]
sbb edx,dword ptr [edx+esi*8]
