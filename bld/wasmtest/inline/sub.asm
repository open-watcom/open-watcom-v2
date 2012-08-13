 sub al,0fH
 sub al,7fH
 sub al,0ffH
 sub ax,0fH
 sub ax,7fH
 sub ax,0ffH
 sub ax,7fffH
 sub ax,0ffffH
 sub eax,0fH
 sub eax,7fH
 sub eax,0ffH
 sub eax,7fffH
 sub eax,0ffffH
 sub eax,7fffffffH
 sub eax,0ffffffffH
 sub dl,0fH
 sub dl,7fH
 sub dl,0ffH
 sub dx,0fH
 sub dx,7fH
 sub dx,0ffH
 sub dx,7fffH
 sub dx,0ffffH
 sub edx,0fH
 sub edx,7fH
 sub edx,0ffH
 sub edx,7fffH
 sub edx,0ffffH
 sub edx,7fffffffH
 sub edx,0ffffffffH
 sub byte ptr x,0fH
 sub byte ptr x,7fH
 sub byte ptr x,0ffH
 sub word ptr x,0fH
 sub word ptr x,7fH
 sub word ptr x,0ffH
 sub word ptr x,7fffH
 sub word ptr x,0ffffH
 sub dword ptr x,0fH
 sub dword ptr x,7fH
 sub dword ptr x,0ffH
 sub dword ptr x,7fffH
 sub dword ptr x,0ffffH
 sub dword ptr x,7fffffffH
 sub dword ptr x,0ffffffffH
 sub byte ptr x,dl
 sub word ptr x,dx
 sub dword ptr x,edx
 sub dl,byte ptr x
 sub dx,word ptr x
 sub edx,dword ptr x
 sub dl,bl
 sub dx,bx
 sub edx,ebx
 sub al,bl
 sub ax,bx
 sub eax,ebx
 sub dl,al
 sub dx,ax
 sub edx,eax
 sub edx,ecx
 sub edx,edx
 sub edx,esi
 sub edx,edi
 sub edx,esp
 sub edx,ebp
 sub dx,cx
 sub dx,dx
 sub dx,si
 sub dx,di
 sub dx,sp
 sub dx,bp
 sub dl,cl
 sub dl,dl
 sub dl,ah
 sub dl,bh
 sub dl,ch
 sub dl,dh
 sub eax,dword ptr [eax]
 sub eax,dword ptr [edx]
 sub eax,dword ptr [ebx]
 sub eax,dword ptr [ecx]
 sub eax,dword ptr [esi]
 sub eax,dword ptr [edi]
 sub eax,dword ptr [esp]
 sub eax,dword ptr [ebp]
 sub eax,dword ptr 0fH[edx]
 sub eax,dword ptr 7fH[edx]
 sub eax,dword ptr 7fffH[edx]
 sub eax,dword ptr 0ffffH[edx]
 sub eax,dword ptr 7fffffffH[edx]
 sub eax,dword ptr -1[edx]
 sub edx,dword ptr [eax]
 sub edx,dword ptr [edx]
 sub edx,dword ptr [ebx]
 sub edx,dword ptr [ecx]
 sub edx,dword ptr [esi]
 sub edx,dword ptr [edi]
 sub edx,dword ptr [esp]
 sub edx,dword ptr [ebp]
 sub edx,dword ptr 0fH[edx]
 sub edx,dword ptr 7fH[edx]
 sub edx,dword ptr 7fffH[edx]
 sub edx,dword ptr 0ffffH[edx]
 sub edx,dword ptr 7fffffffH[edx]
 sub edx,dword ptr -1[edx]
 sub edx,dword ptr [eax+esi]
 sub edx,dword ptr [edx+esi]
 sub edx,dword ptr [ebx+esi]
 sub edx,dword ptr [ecx+esi]
 sub edx,dword ptr [esi+esi]
 sub edx,dword ptr [edi+esi]
 sub edx,dword ptr [esp+esi]
 sub edx,dword ptr [ebp+esi]
 sub edx,dword ptr 0fH[edx+esi]
 sub edx,dword ptr 7fH[edx+esi]
 sub edx,dword ptr 7fffH[edx+esi]
 sub edx,dword ptr 0ffffH[edx+esi]
 sub edx,dword ptr 7fffffffH[edx+esi]
 sub edx,dword ptr -1[edx+esi]
 sub edx,dword ptr -0fH[edx+esi*2]
 sub edx,dword ptr -7fffH[edx+esi*4]
 sub edx,dword ptr [edx+esi*8]
