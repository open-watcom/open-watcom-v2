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
end
