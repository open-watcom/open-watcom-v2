        name    "OneBankvgaBitBlt"
 .386p
 extrn OneBankvgaBitBlt_:near
 _TEXT segment USE32 public 'CODE'
 public OneBankvgaBitBlt
 OneBankvgaBitBlt:
        push ecx
        push ebx
        push edx
        push [esp+9*4]
        push [esp+9*4]
        mov ecx,[esp+9*4]
        mov ebx,[esp+8*4]
        mov edx,[esp+7*4]
        mov eax,[esp+6*4]
        call OneBankvgaBitBlt_
        add  esp,8
        pop edx
        pop ebx
        pop ecx
        ret
 _TEXT ends
 end
