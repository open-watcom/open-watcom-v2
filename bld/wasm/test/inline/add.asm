 add al,0fH
 add al,7fH
 add al,0ffH
 add ax,0fH
 add ax,7fH
 add ax,0ffH
 add ax,7fffH
 add ax,0ffffH
 add eax,0fH
 add eax,7fH
 add eax,0ffH
 add eax,7fffH
 add eax,0ffffH
 add eax,7fffffffH
 add eax,0ffffffffH
 add dl,0fH
 add dl,7fH
 add dl,0ffH
 add dx,0fH
 add dx,7fH
 add dx,0ffH
 add dx,7fffH
 add dx,0ffffH
 add edx,0fH
 add edx,7fH
 add edx,0ffH
 add edx,7fffH
 add edx,0ffffH
 add edx,7fffffffH
 add edx,0ffffffffH
 add byte ptr x,0fH
 add byte ptr x,7fH
 add byte ptr x,0ffH
 add word ptr x,0fH
 add word ptr x,7fH
 add word ptr x,0ffH
 add word ptr x,7fffH
 add word ptr x,0ffffH
 add dword ptr x,0fH
 add dword ptr x,7fH
 add dword ptr x,0ffH
 add dword ptr x,7fffH
 add dword ptr x,0ffffH
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
 add edx,ecx
 add edx,edx
 add edx,esi
 add edx,edi
 add edx,esp
 add edx,ebp
 add dx,cx
 add dx,dx
 add dx,si
 add dx,di
 add dx,sp
 add dx,bp
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
 add eax,dword ptr [ebp]
 add eax,dword ptr 0fH[edx]
 add eax,dword ptr 7fH[edx]
 add eax,dword ptr 7fffH[edx]
 add eax,dword ptr 0ffffH[edx]
 add eax,dword ptr 7fffffffH[edx]
 add eax,dword ptr -1[edx]
 add edx,dword ptr [eax]
 add edx,dword ptr [edx]
 add edx,dword ptr [ebx]
 add edx,dword ptr [ecx]
 add edx,dword ptr [esi]
 add edx,dword ptr [edi]
 add edx,dword ptr [esp]
 add edx,dword ptr [ebp]
 add edx,dword ptr 0fH[edx]
 add edx,dword ptr 7fH[edx]
 add edx,dword ptr 7fffH[edx]
 add edx,dword ptr 0ffffH[edx]
 add edx,dword ptr 7fffffffH[edx]
 add edx,dword ptr -1[edx]
 add edx,dword ptr [eax+esi]
 add edx,dword ptr [edx+esi]
 add edx,dword ptr [ebx+esi]
 add edx,dword ptr [ecx+esi]
 add edx,dword ptr [esi+esi]
 add edx,dword ptr [edi+esi]
 add edx,dword ptr [esp+esi]
 add edx,dword ptr [ebp+esi]
 add edx,dword ptr 0fH[edx+esi]
 add edx,dword ptr 7fH[edx+esi]
 add edx,dword ptr 7fffH[edx+esi]
 add edx,dword ptr 0ffffH[edx+esi]
 add edx,dword ptr 7fffffffH[edx+esi]
 add edx,dword ptr -1[edx+esi]
 add edx,dword ptr -0fH[edx+esi*2]
 add edx,dword ptr -7fffH[edx+esi*4]
 add edx,dword ptr [edx+esi*8]
