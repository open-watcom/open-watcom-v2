; Test different type of the LSx xxx,memory instruction

.386	; default use to use16

extern extern32_fword:fword
extern extern32_dword:dword

DATA16	SEGMENT	USE16

data16_fword	df	?
data16_dword	dd	?

DATA16	ENDS

TEXT16	SEGMENT	USE16 'CODE'
	ASSUME CS:TEXT16,DS:DATA16

    lds bx, extern32_dword
    lds bx, data16_dword
    lds ebx, extern32_fword
    lds ebx, data16_fword
    lds dx,dword ptr[bx + 4]
    lds dx,dword ptr[ebx + 4]
    lds edx,fword ptr[bx + 4]
    lds edx,fword ptr[ebx + 4]

    lss bx, extern32_dword
    lss bx, data16_dword
    lss ebx, extern32_fword
    lss ebx, data16_fword
    lss dx,dword ptr[bx + 4]
    lss dx,dword ptr[ebx + 4]
    lss edx,fword ptr[bx + 4]
    lss edx,fword ptr[ebx + 4]

    les bx, extern32_dword
    les bx, data16_dword
    les ebx, extern32_fword
    les ebx, data16_fword
    les dx,dword ptr[bx + 4]
    les dx,dword ptr[ebx + 4]
    les edx,fword ptr[bx + 4]
    les edx,fword ptr[ebx + 4]

    lfs bx, extern32_dword
    lfs bx, data16_dword
    lfs ebx, extern32_fword
    lfs ebx, data16_fword
    lfs dx,dword ptr[bx + 4]
    lfs dx,dword ptr[ebx + 4]
    lfs edx,fword ptr[bx + 4]
    lfs edx,fword ptr[ebx + 4]

    lgs bx, extern32_dword
    lgs bx, data16_dword
    lgs ebx, extern32_fword
    lgs ebx, data16_fword
    lgs dx,dword ptr[bx + 4]
    lgs dx,dword ptr[ebx + 4]
    lgs edx,fword ptr[bx + 4]
    lgs edx,fword ptr[ebx + 4]

    ret

TEXT16	ENDS

TEXT32	SEGMENT	USE32 'CODE'
	ASSUME CS:TEXT32,DS:DATA16

    lds bx, extern32_dword
    lds bx, data16_dword
    lds ebx, extern32_fword
    lds ebx, data16_fword
    lds dx,dword ptr[bx + 4]
    lds dx,dword ptr[ebx + 4]
    lds edx,fword ptr[bx + 4]
    lds edx,fword ptr[ebx + 4]

    lss bx, extern32_dword
    lss bx, data16_dword
    lss ebx, extern32_fword
    lss ebx, data16_fword
    lss dx,dword ptr[bx + 4]
    lss dx,dword ptr[ebx + 4]
    lss edx,fword ptr[bx + 4]
    lss edx,fword ptr[ebx + 4]

    les bx, extern32_dword
    les bx, data16_dword
    les ebx, extern32_fword
    les ebx, data16_fword
    les dx,dword ptr[bx + 4]
    les dx,dword ptr[ebx + 4]
    les edx,fword ptr[bx + 4]
    les edx,fword ptr[ebx + 4]

    lfs bx, extern32_dword
    lfs bx, data16_dword
    lfs ebx, extern32_fword
    lfs ebx, data16_fword
    lfs dx,dword ptr[bx + 4]
    lfs dx,dword ptr[ebx + 4]
    lfs edx,fword ptr[bx + 4]
    lfs edx,fword ptr[ebx + 4]

    lgs bx, extern32_dword
    lgs bx, data16_dword
    lgs ebx, extern32_fword
    lgs ebx, data16_fword
    lgs dx,dword ptr[bx + 4]
    lgs dx,dword ptr[ebx + 4]
    lgs edx,fword ptr[bx + 4]
    lgs edx,fword ptr[ebx + 4]

    ret

TEXT32	ENDS

end
