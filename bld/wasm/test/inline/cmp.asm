cmp al,0fH
cmp al,7fH
cmp al,0ffH
cmp ax,000fH
cmp ax,007fH
cmp ax,00ffH
cmp ax,7fffH
cmp ax,0ffffH
cmp eax,0000000fH
cmp eax,0000007fH
cmp eax,000000ffH
cmp eax,00007fffH
cmp eax,0000ffffH
cmp eax,7fffffffH
cmp eax,0ffffffffH
cmp dl,0fH
cmp dl,7fH
cmp dl,0ffH
cmp dx,000fH
cmp dx,007fH
cmp dx,00ffH
cmp dx,7fffH
cmp dx,0ffffH
cmp edx,0000000fH
cmp edx,0000007fH
cmp edx,000000ffH
cmp edx,00007fffH
cmp edx,0000ffffH
cmp edx,7fffffffH
cmp edx,0ffffffffH
cmp byte ptr x,0fH
cmp byte ptr x,7fH
cmp byte ptr x,0ffH
cmp word ptr x,000fH
cmp word ptr x,007fH
cmp word ptr x,00ffH
cmp word ptr x,7fffH
cmp word ptr x,0ffffH
cmp dword ptr x,0000000fH
cmp dword ptr x,0000007fH
cmp dword ptr x,000000ffH
cmp dword ptr x,00007fffH
cmp dword ptr x,0000ffffH
cmp dword ptr x,7fffffffH
cmp dword ptr x,0ffffffffH
cmp byte ptr x,dl
cmp word ptr x,dx
cmp dword ptr x,edx
cmp dl,byte ptr x
cmp dx,word ptr x
cmp edx,dword ptr x
cmp dl,bl
cmp dx,bx
cmp edx,ebx
cmp al,bl
cmp ax,bx
cmp eax,ebx
cmp dl,al
cmp dx,ax
cmp edx,eax
cmp edx,ebx
cmp edx,ecx
cmp edx,edx
cmp edx,esi
cmp edx,edi
cmp edx,esp
cmp edx,ebp
cmp edx,edi
cmp dx,ax
cmp dx,bx
cmp dx,cx
cmp dx,dx
cmp dx,si
cmp dx,di
cmp dx,sp
cmp dx,bp
cmp dx,di
cmp dl,al
cmp dl,bl
cmp dl,cl
cmp dl,dl
cmp dl,ah
cmp dl,bh
cmp dl,ch
cmp dl,dh
cmp eax,dword ptr [eax]
cmp eax,dword ptr [edx]
cmp eax,dword ptr [ebx]
cmp eax,dword ptr [ecx]
cmp eax,dword ptr [esi]
cmp eax,dword ptr [edi]
cmp eax,dword ptr [esp]
cmp eax,dword ptr +0H[ebp]
cmp eax,dword ptr +0fH[edx]
cmp eax,dword ptr +7fH[edx]
cmp eax,dword ptr +7fffH[edx]
cmp eax,dword ptr +0ffffH[edx]
cmp eax,dword ptr +7fffffffH[edx]
cmp eax,dword ptr -1H[edx]
cmp edx,dword ptr [eax]
cmp edx,dword ptr [edx]
cmp edx,dword ptr [ebx]
cmp edx,dword ptr [ecx]
cmp edx,dword ptr [esi]
cmp edx,dword ptr [edi]
cmp edx,dword ptr [esp]
cmp edx,dword ptr +0H[ebp]
cmp edx,dword ptr +0fH[edx]
cmp edx,dword ptr +7fH[edx]
cmp edx,dword ptr +7fffH[edx]
cmp edx,dword ptr +0ffffH[edx]
cmp edx,dword ptr +7fffffffH[edx]
cmp edx,dword ptr -1H[edx]
cmp edx,dword ptr [eax+esi]
cmp edx,dword ptr [edx+esi]
cmp edx,dword ptr [ebx+esi]
cmp edx,dword ptr [ecx+esi]
cmp edx,dword ptr [esi+esi]
cmp edx,dword ptr [edi+esi]
cmp edx,dword ptr [esp+esi]
cmp edx,dword ptr +0H[ebp+esi]
cmp edx,dword ptr +0fH[edx+esi]
cmp edx,dword ptr +7fH[edx+esi]
cmp edx,dword ptr +7fffH[edx+esi]
cmp edx,dword ptr +0ffffH[edx+esi]
cmp edx,dword ptr +7fffffffH[edx+esi]
cmp edx,dword ptr -1H[edx+esi]
cmp edx,dword ptr -0fH[edx+esi*2]
cmp edx,dword ptr -7fffH[edx+esi*4]
cmp edx,dword ptr [edx+esi*8]
