add al,0fH
add al,7fH
add al,0ffH
add ax,000fH
add ax,007fH
add ax,00ffH
add ax,7fffH
add ax,0ffffH
add eax,0000000fH
add eax,0000007fH
add eax,000000ffH
add eax,00007fffH
add eax,0000ffffH
add eax,7fffffffH
add eax,0ffffffffH
add dl,0fH
add dl,7fH
add dl,0ffH
add dx,000fH
add dx,007fH
add dx,00ffH
add dx,7fffH
add dx,0ffffH
add edx,0000000fH
add edx,0000007fH
add edx,000000ffH
add edx,00007fffH
add edx,0000ffffH
add edx,7fffffffH
add edx,0ffffffffH
add byte ptr x,0fH
add byte ptr x,7fH
add byte ptr x,0ffH
add word ptr x,000fH
add word ptr x,007fH
add word ptr x,00ffH
add word ptr x,7fffH
add word ptr x,0ffffH
add dword ptr x,0000000fH
add dword ptr x,0000007fH
add dword ptr x,000000ffH
add dword ptr x,00007fffH
add dword ptr x,0000ffffH
add dword ptr x,7fffffffH
add dword ptr x,0ffffffffH
add byte ptr x,dl
add word ptr x,dx
add dword ptr x,edx
add dl,byte ptr x
add dx,word ptr x
add edx,dword ptr x
add dl,bl
add dx,bx
add edx,ebx
add al,bl
add ax,bx
add eax,ebx
add dl,al
add dx,ax
add edx,eax
add edx,ebx
add edx,ecx
add edx,edx
add edx,esi
add edx,edi
add edx,esp
add edx,ebp
add edx,edi
add dx,ax
add dx,bx
add dx,cx
add dx,dx
add dx,si
add dx,di
add dx,sp
add dx,bp
add dx,di
add dl,al
add dl,bl
add dl,cl
add dl,dl
add dl,ah
add dl,bh
add dl,ch
add dl,dh
add eax,dword ptr [eax]
add eax,dword ptr [edx]
add eax,dword ptr [ebx]
add eax,dword ptr [ecx]
add eax,dword ptr [esi]
add eax,dword ptr [edi]
add eax,dword ptr [esp]
add eax,dword ptr +0H[ebp]
add eax,dword ptr +0fH[edx]
add eax,dword ptr +7fH[edx]
add eax,dword ptr +7fffH[edx]
add eax,dword ptr +0ffffH[edx]
add eax,dword ptr +7fffffffH[edx]
add eax,dword ptr -1H[edx]
add edx,dword ptr [eax]
add edx,dword ptr [edx]
add edx,dword ptr [ebx]
add edx,dword ptr [ecx]
add edx,dword ptr [esi]
add edx,dword ptr [edi]
add edx,dword ptr [esp]
add edx,dword ptr +0H[ebp]
add edx,dword ptr +0fH[edx]
add edx,dword ptr +7fH[edx]
add edx,dword ptr +7fffH[edx]
add edx,dword ptr +0ffffH[edx]
add edx,dword ptr +7fffffffH[edx]
add edx,dword ptr -1H[edx]
add edx,dword ptr [eax+esi]
add edx,dword ptr [edx+esi]
add edx,dword ptr [ebx+esi]
add edx,dword ptr [ecx+esi]
add edx,dword ptr [esi+esi]
add edx,dword ptr [edi+esi]
add edx,dword ptr [esp+esi]
add edx,dword ptr +0H[ebp+esi]
add edx,dword ptr +0fH[edx+esi]
add edx,dword ptr +7fH[edx+esi]
add edx,dword ptr +7fffH[edx+esi]
add edx,dword ptr +0ffffH[edx+esi]
add edx,dword ptr +7fffffffH[edx+esi]
add edx,dword ptr -1H[edx+esi]
add edx,dword ptr -0fH[edx+esi*2]
add edx,dword ptr -7fffH[edx+esi*4]
add edx,dword ptr [edx+esi*8]
