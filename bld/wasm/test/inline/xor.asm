xor al,0fH
xor al,7fH
xor al,0ffH
xor ax,000fH
xor ax,007fH
xor ax,00ffH
xor ax,7fffH
xor ax,0ffffH
xor eax,0000000fH
xor eax,0000007fH
xor eax,000000ffH
xor eax,00007fffH
xor eax,0000ffffH
xor eax,7fffffffH
xor eax,0ffffffffH
xor dl,0fH
xor dl,7fH
xor dl,0ffH
xor dx,000fH
xor dx,007fH
xor dx,00ffH
xor dx,7fffH
xor dx,0ffffH
xor edx,0000000fH
xor edx,0000007fH
xor edx,000000ffH
xor edx,00007fffH
xor edx,0000ffffH
xor edx,7fffffffH
xor edx,0ffffffffH
xor byte ptr x,0fH
xor byte ptr x,7fH
xor byte ptr x,0ffH
xor word ptr x,000fH
xor word ptr x,007fH
xor word ptr x,00ffH
xor word ptr x,7fffH
xor word ptr x,0ffffH
xor dword ptr x,0000000fH
xor dword ptr x,0000007fH
xor dword ptr x,000000ffH
xor dword ptr x,00007fffH
xor dword ptr x,0000ffffH
xor dword ptr x,7fffffffH
xor dword ptr x,0ffffffffH
xor byte ptr x,dl
xor word ptr x,dx
xor dword ptr x,edx
xor dl,byte ptr x
xor dx,word ptr x
xor edx,dword ptr x
xor dl,bl
xor dx,bx
xor edx,ebx
xor al,bl
xor ax,bx
xor eax,ebx
xor dl,al
xor dx,ax
xor edx,eax
xor edx,ebx
xor edx,ecx
xor edx,edx
xor edx,esi
xor edx,edi
xor edx,esp
xor edx,ebp
xor edx,edi
xor dx,ax
xor dx,bx
xor dx,cx
xor dx,dx
xor dx,si
xor dx,di
xor dx,sp
xor dx,bp
xor dx,di
xor dl,al
xor dl,bl
xor dl,cl
xor dl,dl
xor dl,ah
xor dl,bh
xor dl,ch
xor dl,dh
xor eax,dword ptr [eax]
xor eax,dword ptr [edx]
xor eax,dword ptr [ebx]
xor eax,dword ptr [ecx]
xor eax,dword ptr [esi]
xor eax,dword ptr [edi]
xor eax,dword ptr [esp]
xor eax,dword ptr +0H[ebp]
xor eax,dword ptr +0fH[edx]
xor eax,dword ptr +7fH[edx]
xor eax,dword ptr +7fffH[edx]
xor eax,dword ptr +0ffffH[edx]
xor eax,dword ptr +7fffffffH[edx]
xor eax,dword ptr -1H[edx]
xor edx,dword ptr [eax]
xor edx,dword ptr [edx]
xor edx,dword ptr [ebx]
xor edx,dword ptr [ecx]
xor edx,dword ptr [esi]
xor edx,dword ptr [edi]
xor edx,dword ptr [esp]
xor edx,dword ptr +0H[ebp]
xor edx,dword ptr +0fH[edx]
xor edx,dword ptr +7fH[edx]
xor edx,dword ptr +7fffH[edx]
xor edx,dword ptr +0ffffH[edx]
xor edx,dword ptr +7fffffffH[edx]
xor edx,dword ptr -1H[edx]
xor edx,dword ptr [eax+esi]
xor edx,dword ptr [edx+esi]
xor edx,dword ptr [ebx+esi]
xor edx,dword ptr [ecx+esi]
xor edx,dword ptr [esi+esi]
xor edx,dword ptr [edi+esi]
xor edx,dword ptr [esp+esi]
xor edx,dword ptr +0H[ebp+esi]
xor edx,dword ptr +0fH[edx+esi]
xor edx,dword ptr +7fH[edx+esi]
xor edx,dword ptr +7fffH[edx+esi]
xor edx,dword ptr +0ffffH[edx+esi]
xor edx,dword ptr +7fffffffH[edx+esi]
xor edx,dword ptr -1H[edx+esi]
xor edx,dword ptr -0fH[edx+esi*2]
xor edx,dword ptr -7fffH[edx+esi*4]
xor edx,dword ptr [edx+esi*8]
