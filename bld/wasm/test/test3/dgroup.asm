.386p

                 assume  cs:_TEXT, ds:DGROUP

DGROUP group _DATA,_TEXT

_DATA segment word public use16 'DATA'
p_offset        dd      0
_DATA ends

public  DoReadWord_

_TEXT segment word public use16 'CODE'

DoReadWord_ proc        near
         mov     ax,word ptr gs:[ebx]
         int     3
DoReadWord_ endp

_TEXT ends

         end
