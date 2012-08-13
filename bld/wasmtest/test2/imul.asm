.386
.model small
.data
    extrn x:near
.code
    imul byte ptr x
    imul word ptr x
    imul dword ptr x
    imul di,word ptr x
    imul edx,dword ptr x
    imul cx,word ptr x,0fH
    imul cx,word ptr x,7fH
    imul cx,word ptr x,00ffH
    imul di,word ptr x,7fffH
    imul di,word ptr x,0ffffH
    imul edi,dword ptr x,7fffffffH
    imul edi,dword ptr x,0ffffffffH
    imul al
    imul byte ptr x
    imul dx
    imul word ptr x
    imul esi
    imul dword ptr x
    imul si,bx
    imul di,word ptr x
    imul esi,edx
    imul edx,dword ptr x
    imul cx,dx,0fH
    imul cx,dx,7fH
    imul cx,dx,0ffH
    imul cx,word ptr x,0fH
    imul cx,word ptr x,7fH
    imul cx,word ptr x,0ffH
    imul bx,0fH
    imul bx,7fH
    imul bx,0ffH
    imul ebx,0fH
    imul ebx,7fH
    imul ebx,0ffH
    imul di,dx,7fffH
    imul di,dx,0ffffH
    imul di,word ptr x,7fffH
    imul di,word ptr x,0ffffH
    imul edi,edx,7fffffffH
    imul edi,edx,0ffffffffH
    imul edi,dword ptr x,7fffffffH
    imul edi,dword ptr x,0ffffffffH
    imul si,7fffH
    imul si,0ffffH
    imul edx,7fffffffH
    imul edx,0ffffffffH
end
