inc byte ptr x
inc al
inc byte ptr -0fH[esp+edi*4]
inc dx
inc word ptr +0fH[edx+ebp*2]
inc esi
inc dword ptr -0fH[eax+edx*8]
