dec byte ptr x
dec al
dec byte ptr -0fH[esp+edi*4]
dec dx
dec word ptr +0fH[edx+ebp*2]
dec esi
dec dword ptr -0fH[eax+edx*8]
