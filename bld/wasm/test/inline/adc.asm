adc al,0fH
adc al,7fH
adc al,0ffH
adc ax,000fH
adc ax,007fH
adc ax,00ffH
adc ax,7fffH
adc ax,0ffffH
adc eax,0000000fH
adc eax,0000007fH
adc eax,000000ffH
adc eax,00007fffH
adc eax,0000ffffH
adc eax,7fffffffH
adc eax,0ffffffffH
adc dl,0fH
adc dl,7fH
adc dl,0ffH
adc dx,000fH
adc dx,007fH
adc dx,00ffH
adc dx,7fffH
adc dx,0ffffH
adc edx,0000000fH
adc edx,0000007fH
adc edx,000000ffH
adc edx,00007fffH
adc edx,0000ffffH
adc edx,7fffffffH
adc edx,0ffffffffH
adc byte ptr x,0fH
adc byte ptr x,7fH
adc byte ptr x,0ffH
adc word ptr x,000fH
adc word ptr x,007fH
adc word ptr x,00ffH
adc word ptr x,7fffH
adc word ptr x,0ffffH
adc dword ptr x,0000000fH
adc dword ptr x,0000007fH
adc dword ptr x,000000ffH
adc dword ptr x,00007fffH
adc dword ptr x,0000ffffH
adc dword ptr x,7fffffffH
adc dword ptr x,0ffffffffH
adc byte ptr x,dl
adc word ptr x,dx
adc dword ptr x,edx
adc dl,byte ptr x
adc dx,word ptr x
adc edx,dword ptr x
adc dl,bl
adc dx,bx
adc edx,ebx
adc al,bl
adc ax,bx
adc eax,ebx
adc dl,al
adc dx,ax
adc edx,eax
adc edx,ebx
adc edx,ecx
adc edx,edx
adc edx,esi
adc edx,edi
adc edx,esp
adc edx,ebp
adc edx,edi
adc dx,ax
adc dx,bx
adc dx,cx
adc dx,dx
adc dx,si
adc dx,di
adc dx,sp
adc dx,bp
adc dx,di
adc dl,al
adc dl,bl
adc dl,cl
adc dl,dl
adc dl,ah
adc dl,bh
adc dl,ch
adc dl,dh
adc eax,dword ptr [eax]
adc eax,dword ptr [edx]
adc eax,dword ptr [ebx]
adc eax,dword ptr [ecx]
adc eax,dword ptr [esi]
adc eax,dword ptr [edi]
adc eax,dword ptr [esp]
adc eax,dword ptr +0H[ebp]
adc eax,dword ptr +0fH[edx]
adc eax,dword ptr +7fH[edx]
adc eax,dword ptr +7fffH[edx]
adc eax,dword ptr +0ffffH[edx]
adc eax,dword ptr +7fffffffH[edx]
adc eax,dword ptr -1H[edx]
adc edx,dword ptr [eax]
adc edx,dword ptr [edx]
adc edx,dword ptr [ebx]
adc edx,dword ptr [ecx]
adc edx,dword ptr [esi]
adc edx,dword ptr [edi]
adc edx,dword ptr [esp]
adc edx,dword ptr +0H[ebp]
adc edx,dword ptr +0fH[edx]
adc edx,dword ptr +7fH[edx]
adc edx,dword ptr +7fffH[edx]
adc edx,dword ptr +0ffffH[edx]
adc edx,dword ptr +7fffffffH[edx]
adc edx,dword ptr -1H[edx]
adc edx,dword ptr [eax+esi]
adc edx,dword ptr [edx+esi]
adc edx,dword ptr [ebx+esi]
adc edx,dword ptr [ecx+esi]
adc edx,dword ptr [esi+esi]
adc edx,dword ptr [edi+esi]
adc edx,dword ptr [esp+esi]
adc edx,dword ptr +0H[ebp+esi]
adc edx,dword ptr +0fH[edx+esi]
adc edx,dword ptr +7fH[edx+esi]
adc edx,dword ptr +7fffH[edx+esi]
adc edx,dword ptr +0ffffH[edx+esi]
adc edx,dword ptr +7fffffffH[edx+esi]
adc edx,dword ptr -1H[edx+esi]
adc edx,dword ptr -0fH[edx+esi*2]
adc edx,dword ptr -7fffH[edx+esi*4]
adc edx,dword ptr [edx+esi*8]
