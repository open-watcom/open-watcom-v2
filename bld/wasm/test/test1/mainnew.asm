retf            MACRO   POP_COUNT
                LOCAL   DUMMY
                DUMMY   PROC FAR
                RET     POP_COUNT
                DUMMY   ENDP
                ENDM
                NAME    main
                EXTRN   __argc :BYTE
                EXTRN   _big_code_ :BYTE
                EXTRN   _cstart_ :BYTE
                EXTRN   InputQueueFile_ :BYTE
                EXTRN   DoDebugMsg_ :BYTE
                EXTRN   atoi_ :BYTE
                EXTRN   InputQueueLine_ :BYTE
                EXTRN   AsmWarn_ :BYTE
                EXTRN   stricmp_ :BYTE
                EXTRN   strupr_ :BYTE
                EXTRN   __osmode :BYTE
                EXTRN   cpu_directive_ :BYTE
                EXTRN   _Code :BYTE
                EXTRN   MsgPrintf1_ :BYTE
                EXTRN   StoreConstant_ :BYTE
                EXTRN   AsmError_ :BYTE
                EXTRN   strchr_ :BYTE
                EXTRN   AsmAlloc_ :BYTE
                EXTRN   _makepath_ :BYTE
                EXTRN   _splitpath2_ :BYTE
                EXTRN   MsgGet_ :BYTE
                EXTRN   DelErrFile_ :BYTE
                EXTRN   ObjWriteOpen_ :BYTE
                EXTRN   Fatal_ :BYTE
                EXTRN   fopen_ :BYTE
                EXTRN   MemFini_ :BYTE
                EXTRN   AsmShutDown_ :BYTE
                EXTRN   GenMSOmfFini_ :BYTE
                EXTRN   GenMSOmfInit_ :BYTE
                EXTRN   ObjRecInit_ :BYTE
                EXTRN   MemInit_ :BYTE
                EXTRN   AsmFree_ :BYTE
                EXTRN   printf_ :BYTE
                EXTRN   PrintfUsage_ :BYTE
                EXTRN   MsgFini_ :BYTE
                EXTRN   PrintStats_ :BYTE
                EXTRN   WriteObjModule_ :BYTE
                EXTRN   PushLineQueue_ :BYTE
                EXTRN   AsmInit_ :BYTE
                EXTRN   exit_ :BYTE
                EXTRN   MsgInit_ :BYTE
                EXTRN   AddStringToIncludePath_ :BYTE
                EXTRN   getenv_ :BYTE
                EXTRN   strcat_ :BYTE
                EXTRN   strcpy_ :BYTE
                EXTRN   stackavail_ :BYTE
                EXTRN   strlen_ :BYTE
                EXTRN   tolower_ :BYTE
                EXTRN   __IsTable :BYTE
                EXTRN   __STK :BYTE
DGROUP          GROUP   CONST,CONST2,_DATA,_BSS
main_TEXT       SEGMENT BYTE PUBLIC  'CODE'
                ASSUME  CS:main_TEXT ,DS:DGROUP,SS:DGROUP
                PUBLIC  do_init_stuff_
                PUBLIC  main_
                PUBLIC  trademark_
                PUBLIC  AsmQueryExternal_
                PUBLIC  AsmQueryType_
L1              DB      57H,41H,54H,43H,4fH,4dH,20H,69H
                DB      73H,20H,61H,20H,74H,72H,61H,64H
                DB      65H,6dH,61H,72H,6bH,20H,6fH,66H
                DB      20H,57H,41H,54H,43H,4fH,4dH,20H
                DB      49H,6eH,74H,65H,72H,6eH,61H,74H
                DB      69H,6fH,6eH,61H,6cH,20H,43H,6fH
                DB      72H,70H,2eH,00H
L2              DB      43H,6fH,70H,79H,72H,69H,67H,68H
                DB      74H,20H,62H,79H,20H,57H,41H,54H
                DB      43H,4fH,4dH,20H,49H,6eH,74H,65H
                DB      72H,6eH,61H,74H,69H,6fH,6eH,61H
                DB      6cH,20H,43H,6fH,72H,70H,2eH,20H
                DB      31H,39H,39H,32H,2cH,20H,31H,39H
                DB      39H,33H,2eH,20H,41H,6cH,6cH,20H
                DB      72H,69H,67H,68H,74H,73H,20H,72H
                DB      65H,73H,65H,72H,76H,65H,64H,2eH
                DB      00H
L3              DB      57H,41H,54H,43H,4fH,4dH,20H,41H
                DB      73H,73H,65H,6dH,62H,6cH,65H,72H
                DB      20H,56H,65H,72H,73H,69H,6fH,6eH
                DB      20H,30H,2eH,39H,00H
L4              DB      64H,65H,62H,75H,67H,67H,69H,6eH
                DB      67H,20H,6fH,75H,74H,70H,75H,74H
                DB      20H,6fH,6eH,20H,0aH,00H
L5              DB      5fH,5fH,57H,49H,4eH,44H,4fH,57H
                DB      53H,5fH,33H,38H,36H,5fH,5fH,00H
L6              DB      5fH,5fH,4eH,45H,54H,57H,41H,52H
                DB      45H,5fH,33H,38H,36H,5fH,5fH,00H
L7              DB      5fH,5fH,52H,45H,47H,49H,53H,54H
                DB      45H,52H,5fH,5fH,00H
L8              DB      5fH,5fH,46H,50H,49H,38H,37H,5fH
                DB      5fH,00H
L9              DB      5fH,5fH,4dH,53H,44H,4fH,53H,5fH
                DB      5fH,00H
L10             DB      5fH,5fH,53H,54H,41H,43H,4bH,5fH
                DB      5fH,00H
L11             DB      50H,45H,4eH,50H,4fH,49H,4eH,54H
                DB      00H
L12             DB      5fH,5fH,57H,41H,53H,4dH,5fH,5fH
                DB      00H
L13             DB      5fH,49H,4eH,43H,4cH,55H,44H,45H
                DB      00H
L14             DB      5fH,5fH,46H,50H,43H,5fH,5fH,00H
L15             DB      5fH,5fH,46H,50H,49H,5fH,5fH,00H
L16             DB      2eH,4dH,4fH,44H,45H,4cH,20H,00H
L17             DB      43H,4fH,4dH,50H,41H,43H,54H,00H
L18             DB      57H,49H,4eH,44H,4fH,57H,53H,00H
L19             DB      4eH,45H,54H,57H,41H,52H,45H,00H
L20             DB      49H,4eH,43H,4cH,55H,44H,45H,00H
L21             DB      4dH,45H,44H,49H,55H,4dH,00H
L22             DB      66H,70H,69H,38H,37H,00H
L23             DB      53H,4dH,41H,4cH,4cH,00H
L24             DB      4cH,41H,52H,47H,45H,00H
L25             DB      54H,49H,4eH,59H,00H
L26             DB      48H,55H,47H,45H,00H
L27             DB      46H,4cH,41H,54H,00H
L28             DB      57H,41H,53H,4dH,00H
L29             DB      66H,70H,63H,00H
L30             DB      66H,70H,69H,00H
L31             DB      51H,4eH,58H,00H
L32             DB      4fH,53H
L33             DB      32H,00H
L34             DB      44H,4fH
L35             DB      53H,00H
L36             DB      65H,72H,72H,00H
L37             DB      6fH,62H,6aH,00H
L38             DB      61H,73H,6dH,00H
L39             DB      25H,73H,0aH,00H
L40             DB      2fH,6dH,00H
L41             DB      4eH,54H,00H
L42             DB      5fH,5fH,00H
L43             DB      37H,00H
L44             DB      31H,00H
L45             DB      72H,00H
L46             DB      20H,00H
isvalidident_:  push    bp
                mov     bp,sp
                push    ax
                mov     ax,0018H
                call    far ptr __STK
                pop     ax
                push    bx
                push    cx
                push    si
                push    di
                sub     sp,000cH
                mov     word ptr -14H[bp],ax
                mov     word ptr -12H[bp],dx
                lds     bx,dword ptr -14H[bp]
                mov     bl,byte ptr [bx]
                inc     bl
                xor     bh,bh
                test    byte ptr ss:__IsTable[bx],20H
                je      short L47
                mov     word ptr -0cH[bp],0ffffH
                jmp     near ptr L65
L47:            mov     bx,word ptr -14H[bp]
                mov     word ptr -10H[bp],bx
                mov     bx,word ptr -12H[bp]
                mov     word ptr -0eH[bp],bx
L48:            lds     bx,dword ptr -10H[bp]
                cmp     byte ptr [bx],00H
                jne     short L50
                jmp     near ptr L64
L49:            mov     dx,word ptr -10H[bp]
                mov     ax,word ptr -0eH[bp]
                inc     word ptr -10H[bp]
                jmp     short L48
L50:            lds     bx,dword ptr -10H[bp]
                mov     al,byte ptr [bx]
                xor     ah,ah
                call    far ptr tolower_
                mov     byte ptr -0aH[bp],al
                cmp     byte ptr -0aH[bp],5fH
                je      short L51
                cmp     byte ptr -0aH[bp],2eH
                jne     short L52
L51:            jmp     short L53
L52:            cmp     byte ptr -0aH[bp],24H
                jne     short L54
L53:            jmp     short L55
L54:            cmp     byte ptr -0aH[bp],40H
                jne     short L56
L55:            jmp     short L57
L56:            cmp     byte ptr -0aH[bp],3fH
                jne     short L58
L57:            jmp     short L59
L58:            mov     bl,byte ptr -0aH[bp]
                inc     bl
                xor     bh,bh
                test    byte ptr ss:__IsTable[bx],20H
                je      short L60
L59:            jmp     short L61
L60:            mov     bl,byte ptr -0aH[bp]
                inc     bl
                xor     bh,bh
                test    byte ptr ss:__IsTable[bx],80H
                je      short L62
L61:            jmp     short L63
L62:            mov     word ptr -0cH[bp],0ffffH
                jmp     short L65
L63:            jmp     short L49
L64:            mov     word ptr -0cH[bp],0001H
L65:            mov     ax,word ptr -0cH[bp]
                lea     sp,-8H[bp]
                pop     di
                pop     si
                pop     cx
                pop     bx
                pop     bp
                ret
get_os_include_: push    bp
                mov     bp,sp
                mov     ax,001aH
                call    far ptr __STK
                push    bx
                push    cx
                push    dx
                push    si
                push    di
                sub     sp,000cH
                mov     bx,word ptr ss:_Options+0cH
                mov     dx,word ptr ss:_Options+0eH
                mov     ax,bx
                call    far ptr strlen_
                add     ax,000bH
                and     al,0feH
                mov     dx,ax
                call    far ptr stackavail_
                cmp     dx,ax
                jae     short L66
                mov     bx,word ptr ss:_Options+0cH
                mov     dx,word ptr ss:_Options+0eH
                mov     ax,bx
                call    far ptr strlen_
                add     ax,000bH
                and     al,0feH
                sub     sp,ax
                mov     ax,sp
                mov     dx,ss
                mov     bx,dx
                mov     word ptr -16H[bp],ax
                mov     word ptr -14H[bp],bx
                jmp     short L67
L66:            mov     word ptr -16H[bp],0000H
                mov     word ptr -14H[bp],0000H
L67:            mov     ax,word ptr -16H[bp]
                mov     word ptr -12H[bp],ax
                mov     ax,word ptr -14H[bp]
                mov     word ptr -10H[bp],ax
                mov     si,word ptr ss:_Options+0cH
                mov     cx,word ptr ss:_Options+0eH
                mov     bx,word ptr -12H[bp]
                mov     dx,word ptr -10H[bp]
                mov     ax,bx
                mov     bx,si
                call    far ptr strcpy_
                mov     cx,cs
                mov     ax,offset L13
                mov     si,ax
                mov     bx,word ptr -12H[bp]
                mov     dx,word ptr -10H[bp]
                mov     ax,bx
                mov     bx,si
                call    far ptr strcat_
                mov     bx,word ptr -12H[bp]
                mov     dx,word ptr -10H[bp]
                mov     ax,bx
                call    far ptr getenv_
                mov     bx,ax
                mov     ax,dx
                mov     word ptr -0eH[bp],bx
                mov     word ptr -0cH[bp],ax
                mov     dx,word ptr -0eH[bp]
                mov     ax,word ptr -0cH[bp]
                xor     cx,cx
                xor     bx,bx
                cmp     ax,bx
                jne     short L68
                cmp     dx,cx
                je      short L69
L68:            mov     bx,word ptr -0eH[bp]
                mov     dx,word ptr -0cH[bp]
                mov     ax,bx
                call    far ptr AddStringToIncludePath_
L69:            lea     sp,-0aH[bp]
                pop     di
                pop     si
                pop     dx
                pop     cx
                pop     bx
                pop     bp
                ret
do_init_stuff_: inc     bp
                push    bp
                mov     bp,sp
                push    ax
                mov     ax,0016H
                call    far ptr __STK
                pop     ax
                push    dx
                push    si
                push    di
                sub     sp,000aH
                mov     word ptr -8H[bp],ax
                mov     word ptr -10H[bp],bx
                mov     word ptr -0eH[bp],cx
                lds     bx,dword ptr -10H[bp]
                mov     bx,word ptr [bx]
                lds     si,dword ptr -10H[bp]
                mov     dx,word ptr +2H[si]
                mov     ax,bx
                call    far ptr MsgInit_
                test    ax,ax
                je      short L70
                mov     ax,0001H
                jmp     far ptr exit_
L70:            mov     bx,0ffffH
                mov     dx,0ffffH
                mov     ax,0ffffH
                call    far ptr AsmInit_
                mov     dx,cs
                mov     ax,offset L12
                mov     bx,ax
                mov     ax,bx
                call    near ptr add_constant_
                xor     cx,cx
                mov     dx,cs
                mov     ax,offset L28
                mov     bx,ax
                mov     ax,bx
                mov     bx,cx
                call    near ptr do_envvar_cmdline_
                mov     dx,word ptr -10H[bp]
                mov     cx,word ptr -0eH[bp]
                mov     ax,word ptr -8H[bp]
                mov     bx,dx
                call    near ptr parse_cmdline_
                call    near ptr set_build_target_
                call    near ptr get_os_include_
                mov     dx,cs
                mov     ax,offset L20
                mov     bx,ax
                mov     ax,bx
                call    far ptr getenv_
                mov     bx,ax
                mov     ax,dx
                mov     word ptr -0cH[bp],bx
                mov     word ptr -0aH[bp],ax
                mov     cx,word ptr -0cH[bp]
                mov     bx,word ptr -0aH[bp]
                xor     dx,dx
                xor     ax,ax
                cmp     bx,ax
                jne     short L71
                cmp     cx,dx
                je      short L72
L71:            mov     bx,word ptr -0cH[bp]
                mov     dx,word ptr -0aH[bp]
                mov     ax,bx
                call    far ptr AddStringToIncludePath_
L72:            cmp     byte ptr ss:_Options+2H,00H
                jne     short L73
                cmp     byte ptr ss:_Options+3H,00H
                je      short L74
L73:            jmp     short L75
L74:            mov     byte ptr ss:_Options+3H,01H
                push    cs
                call    near ptr trademark_
L75:            call    near ptr open_files_
                call    far ptr PushLineQueue_
                lea     sp,-6H[bp]
                pop     di
                pop     si
                pop     dx
                pop     bp
                dec     bp
                retf
main_:          inc     bp
                push    bp
                mov     bp,sp
                push    ax
                mov     ax,0014H
                call    far ptr __STK
                pop     ax
                push    dx
                push    si
                push    di
                sub     sp,0008H
                mov     word ptr -0aH[bp],ax
                mov     word ptr -0eH[bp],bx
                mov     word ptr -0cH[bp],cx
                call    near ptr main_init_
                mov     dx,word ptr -0eH[bp]
                mov     cx,word ptr -0cH[bp]
                mov     ax,word ptr -0aH[bp]
                mov     bx,dx
                push    cs
                call    near ptr do_init_stuff_
                call    far ptr WriteObjModule_
                cmp     byte ptr ss:_Options+2H,00H
                jne     short L76
                call    far ptr PrintStats_
L76:            call    far ptr MsgFini_
                call    near ptr main_fini_
                mov     al,byte ptr ss:_Options+7H
                xor     ah,ah
                mov     word ptr -8H[bp],ax
                mov     ax,word ptr -8H[bp]
                lea     sp,-6H[bp]
                pop     di
                pop     si
                pop     dx
                pop     bp
                dec     bp
                retf
usage_msg_:     push    bp
                mov     bp,sp
                mov     ax,000eH
                call    far ptr __STK
                push    bx
                push    cx
                push    dx
                push    si
                push    di
                sub     sp,0000H
                mov     ax,07d0H
                call    far ptr PrintfUsage_
                mov     ax,0001H
                jmp     far ptr exit_
trademark_:     inc     bp
                push    bp
                mov     bp,sp
                mov     ax,001aH
                call    far ptr __STK
                push    bx
                push    cx
                push    dx
                push    si
                push    di
                sub     sp,0002H
                mov     word ptr -0cH[bp],0000H
                cmp     byte ptr ss:_Options+2H,00H
                jne     short L79
L77:            mov     bx,word ptr -0cH[bp]
                shl     bx,1
                shl     bx,1
                mov     cx,word ptr ss:L307[bx]
                mov     bx,word ptr ss:L308[bx]
                xor     dx,dx
                xor     ax,ax
                cmp     bx,ax
                jne     short L78
                cmp     cx,dx
                je      short L79
L78:            mov     bx,word ptr -0cH[bp]
                inc     word ptr -0cH[bp]
                shl     bx,1
                shl     bx,1
                push    word ptr ss:L308[bx]
                push    word ptr ss:L307[bx]
                mov     ax,cs
                mov     dx,offset L39
                push    ax
                push    dx
                call    far ptr printf_
                add     sp,0008H
                jmp     short L77
L79:            lea     sp,-0aH[bp]
                pop     di
                pop     si
                pop     dx
                pop     cx
                pop     bx
                pop     bp
                dec     bp
                retf
free_names_:    push    bp
                mov     bp,sp
                mov     ax,000eH
                call    far ptr __STK
                push    bx
                push    cx
                push    dx
                push    si
                push    di
                sub     sp,0000H
                mov     cx,word ptr ss:_Options+0cH
                mov     bx,word ptr ss:_Options+0eH
                xor     dx,dx
                xor     ax,ax
                cmp     bx,ax
                jne     short L80
                cmp     cx,dx
                je      short L81
L80:            mov     bx,word ptr ss:_Options+0cH
                mov     dx,word ptr ss:_Options+0eH
                mov     ax,bx
                call    far ptr AsmFree_
L81:            mov     cx,word ptr ss:_Options+10H
                mov     bx,word ptr ss:_Options+12H
                xor     dx,dx
                xor     ax,ax
                cmp     bx,ax
                jne     short L82
                cmp     cx,dx
                je      short L83
L82:            mov     bx,word ptr ss:_Options+10H
                mov     dx,word ptr ss:_Options+12H
                mov     ax,bx
                call    far ptr AsmFree_
L83:            mov     cx,word ptr ss:_Options+14H
                mov     bx,word ptr ss:_Options+16H
                xor     dx,dx
                xor     ax,ax
                cmp     bx,ax
                jne     short L84
                cmp     cx,dx
                je      short L85
L84:            mov     bx,word ptr ss:_Options+14H
                mov     dx,word ptr ss:_Options+16H
                mov     ax,bx
                call    far ptr AsmFree_
L85:            mov     cx,word ptr ss:_Options+1cH
                mov     bx,word ptr ss:_Options+1eH
                xor     dx,dx
                xor     ax,ax
                cmp     bx,ax
                jne     short L86
                cmp     cx,dx
                je      short L87
L86:            mov     bx,word ptr ss:_Options+1cH
                mov     dx,word ptr ss:_Options+1eH
                mov     ax,bx
                call    far ptr AsmFree_
L87:            mov     cx,word ptr ss:_Options+18H
                mov     bx,word ptr ss:_Options+1aH
                xor     dx,dx
                xor     ax,ax
                cmp     bx,ax
                jne     short L88
                cmp     cx,dx
                je      short L89
L88:            mov     bx,word ptr ss:_Options+18H
                mov     dx,word ptr ss:_Options+1aH
                mov     ax,bx
                call    far ptr AsmFree_
L89:            lea     sp,-0aH[bp]
                pop     di
                pop     si
                pop     dx
                pop     cx
                pop     bx
                pop     bp
                ret
free_file_:     push    bp
                mov     bp,sp
                mov     ax,000eH
                call    far ptr __STK
                push    bx
                push    cx
                push    dx
                push    si
                push    di
                sub     sp,0000H
                mov     bx,word ptr ss:_AsmFiles+0cH
                mov     dx,word ptr ss:_AsmFiles+0eH
                mov     ax,bx
                call    far ptr AsmFree_
                mov     bx,word ptr ss:_AsmFiles+10H
                mov     dx,word ptr ss:_AsmFiles+12H
                mov     ax,bx
                call    far ptr AsmFree_
                mov     bx,word ptr ss:_AsmFiles+14H
                mov     dx,word ptr ss:_AsmFiles+16H
                mov     ax,bx
                call    far ptr AsmFree_
                lea     sp,-0aH[bp]
                pop     di
                pop     si
                pop     dx
                pop     cx
                pop     bx
                pop     bp
                ret
main_init_:     push    bp
                mov     bp,sp
                mov     ax,0010H
                call    far ptr __STK
                push    bx
                push    cx
                push    dx
                push    si
                push    di
                sub     sp,0002H
                call    far ptr MemInit_
                mov     word ptr -0cH[bp],0000H
L90:            cmp     word ptr -0cH[bp],0002H
                jle     short L92
                jmp     short L93
L91:            mov     ax,word ptr -0cH[bp]
                inc     word ptr -0cH[bp]
                jmp     short L90
L92:            mov     bx,word ptr -0cH[bp]
                shl     bx,1
                shl     bx,1
                mov     word ptr ss:_AsmFiles[bx],0000H
                mov     word ptr ss:_AsmFiles+2H[bx],0000H
                mov     bx,word ptr -0cH[bp]
                shl     bx,1
                shl     bx,1
                mov     word ptr ss:_AsmFiles+0cH[bx],0000H
                mov     word ptr ss:_AsmFiles+0eH[bx],0000H
                jmp     short L91
L93:            call    far ptr ObjRecInit_
                call    far ptr GenMSOmfInit_
                lea     sp,-0aH[bp]
                pop     di
                pop     si
                pop     dx
                pop     cx
                pop     bx
                pop     bp
                ret
main_fini_:     push    bp
                mov     bp,sp
                mov     ax,000eH
                call    far ptr __STK
                push    bx
                push    cx
                push    dx
                push    si
                push    di
                sub     sp,0000H
                call    near ptr free_file_
                call    near ptr free_names_
                call    far ptr GenMSOmfFini_
                call    far ptr AsmShutDown_
                call    far ptr MemFini_
                lea     sp,-0aH[bp]
                pop     di
                pop     si
                pop     dx
                pop     cx
                pop     bx
                pop     bp
                ret
open_files_:    push    bp
                mov     bp,sp
                mov     ax,0014H
                call    far ptr __STK
                push    bx
                push    cx
                push    dx
                push    si
                push    di
                sub     sp,0000H
                mov     cx,cs
                mov     dx,offset L45
                mov     si,dx
                mov     bx,word ptr ss:_AsmFiles+0cH
                mov     dx,word ptr ss:_AsmFiles+0eH
                mov     ax,bx
                mov     bx,si
                call    far ptr fopen_
                mov     cx,ax
                mov     bx,dx
                mov     word ptr ss:_AsmFiles,cx
                mov     word ptr ss:_AsmFiles+2H,bx
                mov     bx,word ptr ss:_AsmFiles
                mov     dx,word ptr ss:_AsmFiles+2H
                xor     si,si
                xor     cx,cx
                cmp     dx,cx
                jne     short L94
                cmp     bx,si
                jne     short L94
                push    word ptr ss:_AsmFiles+0eH
                push    word ptr ss:_AsmFiles+0cH
                mov     ax,0001H
                push    ax
                call    far ptr Fatal_
                add     sp,0006H
L94:            mov     bx,word ptr ss:_AsmFiles+14H
                mov     dx,word ptr ss:_AsmFiles+16H
                mov     ax,bx
                call    far ptr ObjWriteOpen_
                mov     bx,dx
                mov     word ptr ss:_pobjState+1H,ax
                mov     word ptr ss:_pobjState+3H,bx
                mov     cx,word ptr ss:_pobjState+1H
                mov     bx,word ptr ss:_pobjState+3H
                xor     dx,dx
                xor     ax,ax
                cmp     bx,ax
                jne     short L95
                cmp     cx,dx
                jne     short L95
                push    word ptr ss:_AsmFiles+16H
                push    word ptr ss:_AsmFiles+14H
                mov     ax,0001H
                push    ax
                call    far ptr Fatal_
                add     sp,0006H
L95:            mov     byte ptr ss:_pobjState,02H
                call    far ptr DelErrFile_
                lea     sp,-0aH[bp]
                pop     di
                pop     si
                pop     dx
                pop     cx
                pop     bx
                pop     bp
                ret
get_fname_:     push    bp
                mov     bp,sp
                push    ax
                mov     ax,03a8H
                call    far ptr __STK
                pop     ax
                push    cx
                push    si
                push    di
                sub     sp,038eH
                mov     word ptr -2cH[bp],ax
                mov     word ptr -2aH[bp],dx
                mov     word ptr -8H[bp],bx
                cmp     word ptr -8H[bp],0000H
                je      short L96
                jmp     near ptr L103
L96:            mov     cx,word ptr -2cH[bp]
                mov     bx,word ptr -2aH[bp]
                xor     dx,dx
                xor     ax,ax
                cmp     bx,ax
                jne     short L97
                cmp     cx,dx
                jne     short L97
                mov     cx,ss
                lea     ax,-7cH[bp]
                mov     dx,ax
                mov     ax,043fH
                mov     bx,dx
                call    far ptr MsgGet_
                mov     dx,ss
                lea     cx,-7cH[bp]
                push    dx
                push    cx
                mov     ax,0001H
                push    ax
                call    far ptr Fatal_
                add     sp,0006H
L97:            mov     cx,word ptr ss:_AsmFiles+0cH
                mov     bx,word ptr ss:_AsmFiles+0eH
                xor     dx,dx
                xor     ax,ax
                cmp     bx,ax
                jne     short L98
                cmp     cx,dx
                je      short L99
L98:            mov     ax,0005H
                push    ax
                call    far ptr Fatal_
                add     sp,0002H
L99:            mov     dx,ss
                lea     bx,-24H[bp]
                push    dx
                push    bx
                mov     dx,ss
                lea     bx,-20H[bp]
                push    dx
                push    bx
                mov     dx,ss
                lea     bx,-28H[bp]
                push    dx
                push    bx
                mov     dx,ss
                lea     bx,-18H[bp]
                push    dx
                push    bx
                mov     cx,ss
                lea     dx,-288H[bp]
                mov     si,dx
                mov     bx,word ptr -2cH[bp]
                mov     dx,word ptr -2aH[bp]
                mov     ax,bx
                mov     bx,si
                call    far ptr _splitpath2_
                lds     bx,dword ptr -24H[bp]
                cmp     byte ptr [bx],00H
                jne     short L100
                mov     word ptr -22H[bp],cs
                mov     word ptr -24H[bp],offset L38
L100:           mov     dx,word ptr -24H[bp]
                mov     ax,word ptr -22H[bp]
                push    ax
                push    dx
                mov     dx,word ptr -20H[bp]
                mov     ax,word ptr -1eH[bp]
                push    ax
                push    dx
                mov     dx,word ptr -28H[bp]
                mov     ax,word ptr -26H[bp]
                push    ax
                push    dx
                mov     si,word ptr -18H[bp]
                mov     cx,word ptr -16H[bp]
                mov     dx,ss
                lea     ax,-180H[bp]
                mov     bx,ax
                mov     ax,bx
                mov     bx,si
                call    far ptr _makepath_
                mov     dx,ss
                lea     bx,-180H[bp]
                mov     cx,bx
                mov     ax,cx
                call    far ptr strlen_
                inc     ax
                call    far ptr AsmAlloc_
                mov     si,ax
                mov     cx,dx
                mov     word ptr ss:_AsmFiles+0cH,si
                mov     word ptr ss:_AsmFiles+0eH,cx
                mov     cx,ss
                lea     dx,-180H[bp]
                mov     si,dx
                mov     bx,word ptr ss:_AsmFiles+0cH
                mov     dx,word ptr ss:_AsmFiles+0eH
                mov     ax,bx
                mov     bx,si
                call    far ptr strcpy_
                mov     dx,word ptr ss:_AsmFiles+14H
                mov     ax,word ptr ss:_AsmFiles+16H
                xor     cx,cx
                xor     bx,bx
                cmp     ax,bx
                jne     short L101
                cmp     dx,cx
                jne     short L101
                mov     word ptr -22H[bp],cs
                mov     word ptr -24H[bp],offset L37
                mov     dx,word ptr -24H[bp]
                mov     ax,word ptr -22H[bp]
                push    ax
                push    dx
                mov     dx,word ptr -20H[bp]
                mov     ax,word ptr -1eH[bp]
                push    ax
                push    dx
                xor     ax,ax
                push    ax
                xor     ax,ax
                push    ax
                xor     si,si
                xor     cx,cx
                mov     dx,ss
                lea     ax,-180H[bp]
                mov     bx,ax
                mov     ax,bx
                mov     bx,si
                call    far ptr _makepath_
                mov     dx,ss
                lea     cx,-180H[bp]
                mov     bx,cx
                mov     ax,bx
                call    far ptr strlen_
                inc     ax
                call    far ptr AsmAlloc_
                mov     cx,ax
                mov     bx,dx
                mov     word ptr ss:_AsmFiles+14H,cx
                mov     word ptr ss:_AsmFiles+16H,bx
                mov     cx,ss
                lea     dx,-180H[bp]
                mov     si,dx
                mov     bx,word ptr ss:_AsmFiles+14H
                mov     dx,word ptr ss:_AsmFiles+16H
                mov     ax,bx
                mov     bx,si
                call    far ptr strcpy_
L101:           mov     cx,word ptr ss:_AsmFiles+10H
                mov     bx,word ptr ss:_AsmFiles+12H
                xor     dx,dx
                xor     ax,ax
                cmp     bx,ax
                jne     short L102
                cmp     cx,dx
                jne     short L102
                mov     word ptr -22H[bp],cs
                mov     word ptr -24H[bp],offset L36
                mov     dx,word ptr -24H[bp]
                mov     ax,word ptr -22H[bp]
                push    ax
                push    dx
                mov     dx,word ptr -20H[bp]
                mov     ax,word ptr -1eH[bp]
                push    ax
                push    dx
                xor     ax,ax
                push    ax
                xor     ax,ax
                push    ax
                xor     si,si
                xor     cx,cx
                mov     dx,ss
                lea     ax,-180H[bp]
                mov     bx,ax
                mov     ax,bx
                mov     bx,si
                call    far ptr _makepath_
                mov     dx,ss
                lea     cx,-180H[bp]
                mov     bx,cx
                mov     ax,bx
                call    far ptr strlen_
                inc     ax
                call    far ptr AsmAlloc_
                mov     cx,ax
                mov     bx,dx
                mov     word ptr ss:_AsmFiles+10H,cx
                mov     word ptr ss:_AsmFiles+12H,bx
                mov     cx,ss
                lea     dx,-180H[bp]
                mov     si,dx
                mov     bx,word ptr ss:_AsmFiles+10H
                mov     dx,word ptr ss:_AsmFiles+12H
                mov     ax,bx
                mov     bx,si
                call    far ptr strcpy_
L102:           jmp     near ptr L108
L103:           mov     dx,ss
                lea     bx,-24H[bp]
                push    dx
                push    bx
                mov     dx,ss
                lea     bx,-20H[bp]
                push    dx
                push    bx
                mov     dx,ss
                lea     bx,-28H[bp]
                push    dx
                push    bx
                mov     dx,ss
                lea     bx,-18H[bp]
                push    dx
                push    bx
                mov     cx,ss
                lea     dx,-288H[bp]
                mov     si,dx
                mov     bx,word ptr -2cH[bp]
                mov     dx,word ptr -2aH[bp]
                mov     ax,bx
                mov     bx,si
                call    far ptr _splitpath2_
                mov     dx,ss
                lea     bx,-14H[bp]
                push    dx
                push    bx
                mov     dx,ss
                lea     bx,-10H[bp]
                push    dx
                push    bx
                mov     dx,ss
                lea     bx,-1cH[bp]
                push    dx
                push    bx
                mov     dx,ss
                lea     bx,-0cH[bp]
                push    dx
                push    bx
                mov     si,ss
                lea     dx,-390H[bp]
                mov     di,dx
                mov     cx,word ptr ss:_AsmFiles+0cH
                mov     dx,word ptr ss:_AsmFiles+0eH
                mov     ax,cx
                mov     bx,di
                mov     cx,si
                call    far ptr _splitpath2_
                lds     bx,dword ptr -24H[bp]
                cmp     byte ptr [bx],00H
                jne     short L106
                cmp     word ptr -8H[bp],0001H
                jne     short L104
                mov     word ptr -392H[bp],cs
                mov     word ptr -394H[bp],offset L36
                jmp     short L105
L104:           mov     word ptr -392H[bp],cs
                mov     word ptr -394H[bp],offset L37
L105:           mov     bx,word ptr -394H[bp]
                mov     word ptr -24H[bp],bx
                mov     bx,word ptr -392H[bp]
                mov     word ptr -22H[bp],bx
L106:           lds     bx,dword ptr -20H[bp]
                cmp     byte ptr [bx],00H
                jne     short L107
                mov     bx,word ptr -10H[bp]
                mov     word ptr -20H[bp],bx
                mov     bx,word ptr -0eH[bp]
                mov     word ptr -1eH[bp],bx
L107:           mov     dx,word ptr -24H[bp]
                mov     ax,word ptr -22H[bp]
                push    ax
                push    dx
                mov     dx,word ptr -20H[bp]
                mov     ax,word ptr -1eH[bp]
                push    ax
                push    dx
                mov     dx,word ptr -28H[bp]
                mov     ax,word ptr -26H[bp]
                push    ax
                push    dx
                mov     si,word ptr -18H[bp]
                mov     cx,word ptr -16H[bp]
                mov     dx,ss
                lea     ax,-180H[bp]
                mov     bx,ax
                mov     ax,bx
                mov     bx,si
                call    far ptr _makepath_
                mov     dx,ss
                lea     bx,-180H[bp]
                mov     cx,bx
                mov     ax,cx
                call    far ptr strlen_
                inc     ax
                call    far ptr AsmAlloc_
                mov     si,ax
                mov     cx,dx
                mov     bx,word ptr -8H[bp]
                shl     bx,1
                shl     bx,1
                mov     word ptr ss:_AsmFiles+0cH[bx],si
                mov     word ptr ss:_AsmFiles+0eH[bx],cx
                mov     cx,ss
                lea     ax,-180H[bp]
                mov     si,ax
                mov     di,word ptr -8H[bp]
                shl     di,1
                shl     di,1
                mov     bx,word ptr ss:_AsmFiles+0cH[di]
                mov     dx,word ptr ss:_AsmFiles+0eH[di]
                mov     ax,bx
                mov     bx,si
                call    far ptr strcpy_
L108:           lea     sp,-6H[bp]
                pop     di
                pop     si
                pop     cx
                pop     bp
                ret
do_envvar_cmdline_:
                push    bp
                mov     bp,sp
                push    ax
                mov     ax,001cH
                call    far ptr __STK
                pop     ax
                push    cx
                push    si
                push    di
                sub     sp,0012H
                mov     word ptr -18H[bp],ax
                mov     word ptr -16H[bp],dx
                mov     word ptr -8H[bp],bx
                cmp     word ptr -8H[bp],000aH
                jl      short L109
                jmp     near ptr L126
L109:           mov     bx,word ptr -18H[bp]
                mov     dx,word ptr -16H[bp]
                mov     ax,bx
                call    far ptr getenv_
                mov     bx,dx
                mov     word ptr -0cH[bp],ax
                mov     word ptr -0aH[bp],bx
                mov     cx,word ptr -0cH[bp]
                mov     bx,word ptr -0aH[bp]
                xor     dx,dx
                xor     ax,ax
                cmp     bx,ax
                jne     short L110
                cmp     cx,dx
                jne     short L110
                jmp     near ptr L126
L110:           lds     bx,dword ptr -0cH[bp]
                cmp     byte ptr [bx],20H
                je      short L111
                lds     bx,dword ptr -0cH[bp]
                mov     al,byte ptr [bx]
                xor     ah,ah
                cmp     ax,2f74H
                jne     short L112
L111:           jmp     short L113
L112:           lds     bx,dword ptr -0cH[bp]
                cmp     byte ptr [bx],00H
                jne     short L114
L113:           mov     dx,word ptr -0cH[bp]
                mov     ax,word ptr -0aH[bp]
                inc     word ptr -0cH[bp]
                jmp     short L110
L114:           mov     bx,word ptr -0cH[bp]
                mov     word ptr -14H[bp],bx
                mov     bx,word ptr -0aH[bp]
                mov     word ptr -12H[bp],bx
                mov     bx,word ptr -0cH[bp]
                mov     word ptr -10H[bp],bx
                mov     bx,word ptr -0aH[bp]
                mov     word ptr -0eH[bp],bx
L115:           lds     bx,dword ptr -14H[bp]
                cmp     byte ptr [bx],00H
                jne     short L116
                jmp     near ptr L126
L116:           lds     bx,dword ptr -10H[bp]
                cmp     byte ptr [bx],20H
                je      short L117
                lds     bx,dword ptr -10H[bp]
                mov     al,byte ptr [bx]
                xor     ah,ah
                cmp     ax,2f74H
                jne     short L118
L117:           jmp     short L119
L118:           lds     bx,dword ptr -10H[bp]
                cmp     byte ptr [bx],00H
                jne     short L120
L119:           jmp     short L121
L120:           mov     dx,word ptr -10H[bp]
                mov     ax,word ptr -0eH[bp]
                inc     word ptr -10H[bp]
                jmp     short L116
L121:           lds     bx,dword ptr -10H[bp]
                cmp     byte ptr [bx],00H
                jne     short L122
                mov     cx,word ptr -8H[bp]
                inc     cx
                mov     bx,word ptr -14H[bp]
                mov     dx,word ptr -12H[bp]
                mov     ax,bx
                mov     bx,cx
                call    near ptr parse_token_
                jmp     short L126
L122:           lds     bx,dword ptr -10H[bp]
                mov     byte ptr [bx],00H
                mov     bx,word ptr -10H[bp]
                mov     dx,word ptr -0eH[bp]
                inc     word ptr -10H[bp]
                mov     cx,word ptr -8H[bp]
                inc     cx
                mov     bx,word ptr -14H[bp]
                mov     dx,word ptr -12H[bp]
                mov     ax,bx
                mov     bx,cx
                call    near ptr parse_token_
L123:           lds     bx,dword ptr -10H[bp]
                cmp     byte ptr [bx],20H
                je      short L124
                lds     bx,dword ptr -10H[bp]
                mov     al,byte ptr [bx]
                xor     ah,ah
                cmp     ax,2f74H
                jne     short L125
L124:           mov     dx,word ptr -10H[bp]
                mov     ax,word ptr -0eH[bp]
                inc     word ptr -10H[bp]
                jmp     short L123
L125:           lds     bx,dword ptr -10H[bp]
                cmp     byte ptr [bx],00H
                je      short L126
                mov     bx,word ptr -10H[bp]
                mov     word ptr -14H[bp],bx
                mov     bx,word ptr -0eH[bp]
                mov     word ptr -12H[bp],bx
                jmp     near ptr L115
L126:           lea     sp,-6H[bp]
                pop     di
                pop     si
                pop     cx
                pop     bp
                ret
add_constant_:  push    bp
                mov     bp,sp
                push    ax
                mov     ax,001aH
                call    far ptr __STK
                pop     ax
                push    bx
                push    cx
                push    si
                push    di
                sub     sp,000cH
                mov     word ptr -14H[bp],ax
                mov     word ptr -12H[bp],dx
                mov     word ptr -0eH[bp],cs
                mov     word ptr -10H[bp],offset L44
                mov     cx,003dH
                mov     bx,word ptr -14H[bp]
                mov     dx,word ptr -12H[bp]
                mov     ax,bx
                mov     bx,cx
                call    far ptr strchr_
                mov     bx,dx
                mov     word ptr -0cH[bp],ax
                mov     word ptr -0aH[bp],bx
                mov     dx,word ptr -0cH[bp]
                mov     ax,word ptr -0aH[bp]
                xor     cx,cx
                xor     bx,bx
                cmp     ax,bx
                jne     short L129
                cmp     dx,cx
                jne     short L129
                mov     cx,0023H
                mov     bx,word ptr -14H[bp]
                mov     dx,word ptr -12H[bp]
                mov     ax,bx
                mov     bx,cx
                call    far ptr strchr_
                mov     bx,dx
                mov     word ptr -0cH[bp],ax
                mov     word ptr -0aH[bp],bx
                mov     dx,word ptr -0cH[bp]
                mov     ax,word ptr -0aH[bp]
                xor     cx,cx
                xor     bx,bx
                cmp     ax,bx
                jne     short L127
                cmp     dx,cx
                jne     short L127
                mov     ax,word ptr -10H[bp]
                mov     word ptr -0cH[bp],ax
                mov     ax,word ptr -0eH[bp]
                mov     word ptr -0aH[bp],ax
                jmp     short L128
L127:           lds     bx,dword ptr -0cH[bp]
                mov     byte ptr [bx],00H
                mov     dx,word ptr -0cH[bp]
                mov     ax,word ptr -0aH[bp]
                inc     word ptr -0cH[bp]
L128:           jmp     short L130
L129:           lds     bx,dword ptr -0cH[bp]
                mov     byte ptr [bx],00H
                mov     dx,word ptr -0cH[bp]
                mov     ax,word ptr -0aH[bp]
                inc     word ptr -0cH[bp]
L130:           mov     bx,word ptr -14H[bp]
                mov     dx,word ptr -12H[bp]
                mov     ax,bx
                call    near ptr isvalidident_
                cmp     ax,0ffffH
                jne     short L131
                mov     ax,0033H
                call    far ptr AsmError_
                jmp     short L132
L131:           xor     dx,dx
                push    dx
                mov     si,word ptr -0cH[bp]
                mov     cx,word ptr -0aH[bp]
                mov     bx,word ptr -14H[bp]
                mov     dx,word ptr -12H[bp]
                mov     ax,bx
                mov     bx,si
                call    far ptr StoreConstant_
L132:           lea     sp,-8H[bp]
                pop     di
                pop     si
                pop     cx
                pop     bx
                pop     bp
                ret
set_processor_type_:
                push    bp
                mov     bp,sp
                push    ax
                mov     ax,0026H
                call    far ptr __STK
                pop     ax
                push    bx
                push    cx
                push    si
                push    di
                sub     sp,001aH
                mov     word ptr -14H[bp],ax
                mov     word ptr -12H[bp],dx
                mov     byte ptr -0aH[bp],00H
                mov     ax,word ptr -12H[bp]
                mov     word ptr -0eH[bp],ax
                mov     ax,word ptr -14H[bp]
                inc     ax
                mov     word ptr -10H[bp],ax
                mov     ax,word ptr -12H[bp]
                mov     word ptr -0eH[bp],ax
                mov     ax,word ptr -14H[bp]
                inc     ax
                mov     word ptr -10H[bp],ax
L133:           lds     bx,dword ptr -10H[bp]
                cmp     byte ptr [bx],00H
                je      short L134
                lds     bx,dword ptr -10H[bp]
                mov     bl,byte ptr [bx]
                inc     bl
                xor     bh,bh
                test    byte ptr ss:__IsTable[bx],02H
                je      short L136
L134:           jmp     near ptr L145
L135:           mov     dx,word ptr -10H[bp]
                mov     ax,word ptr -0eH[bp]
                inc     word ptr -10H[bp]
                jmp     short L133
L136:           lds     bx,dword ptr -14H[bp]
                cmp     byte ptr [bx],66H
                jne     short L137
                jmp     near ptr L145
L137:           lds     bx,dword ptr -10H[bp]
                cmp     byte ptr [bx],72H
                jne     short L138
                mov     byte ptr ss:_Options+5H,01H
                mov     dx,cs
                mov     ax,offset L7
                mov     bx,ax
                mov     ax,bx
                call    near ptr add_constant_
                jmp     short L144
L138:           lds     bx,dword ptr -10H[bp]
                cmp     byte ptr [bx],73H
                jne     short L139
                mov     dx,cs
                mov     ax,offset L10
                mov     bx,ax
                mov     ax,bx
                call    near ptr add_constant_
                mov     byte ptr ss:_Options+5H,00H
                jmp     short L144
L139:           lds     bx,dword ptr -10H[bp]
                cmp     byte ptr [bx],5fH
                jne     short L142
                cmp     byte ptr ss:_Options+5H,00H
                jne     short L140
                mov     byte ptr ss:_Options+5H,02H
                jmp     short L141
L140:           mov     byte ptr ss:_Options+5H,00H
L141:           jmp     short L144
L142:           lds     bx,dword ptr -10H[bp]
                cmp     byte ptr [bx],70H
                jne     short L143
                mov     byte ptr -0aH[bp],01H
                jmp     short L144
L143:           mov     dx,word ptr -14H[bp]
                mov     cx,word ptr -12H[bp]
                mov     ax,044eH
                mov     bx,dx
                call    far ptr MsgPrintf1_
                mov     ax,0001H
                jmp     far ptr exit_
L144:           jmp     near ptr L135
L145:           lds     bx,dword ptr -14H[bp]
                mov     al,byte ptr [bx]
                mov     byte ptr -16H[bp],al
                cmp     byte ptr -16H[bp],33H
                jb      short L152
                cmp     byte ptr -16H[bp],33H
                ja      short L146
                jmp     near ptr L159
L146:           cmp     byte ptr -16H[bp],35H
                jb      short L151
                cmp     byte ptr -16H[bp],35H
                ja      short L147
                jmp     near ptr L165
L147:           cmp     byte ptr -16H[bp],37H
                jb      short L150
                cmp     byte ptr -16H[bp],37H
                ja      short L148
                jmp     near ptr L168
L148:           cmp     byte ptr -16H[bp],66H
                jne     short L149
                jmp     near ptr L175
L149:           jmp     near ptr L183
L150:           jmp     near ptr L183
L151:           jmp     short L162
L152:           cmp     byte ptr -16H[bp],31H
                jb      short L153
                cmp     byte ptr -16H[bp],31H
                jbe     short L155
                jmp     short L156
L153:           cmp     byte ptr -16H[bp],30H
                je      short L154
                jmp     near ptr L183
L154:           mov     word ptr -0cH[bp],000cH
                jmp     near ptr L183
L155:           mov     word ptr -0cH[bp],0000H
                jmp     near ptr L183
L156:           cmp     byte ptr -0aH[bp],00H
                je      short L157
                mov     word ptr -18H[bp],0003H
                jmp     short L158
L157:           mov     word ptr -18H[bp],0001H
L158:           mov     ax,word ptr -18H[bp]
                mov     word ptr -0cH[bp],ax
                jmp     near ptr L183
L159:           cmp     byte ptr -0aH[bp],00H
                je      short L160
                mov     word ptr -1aH[bp],0006H
                jmp     short L161
L160:           mov     word ptr -1aH[bp],0005H
L161:           mov     ax,word ptr -1aH[bp]
                mov     word ptr -0cH[bp],ax
                jmp     near ptr L183
L162:           cmp     byte ptr -0aH[bp],00H
                je      short L163
                mov     word ptr -1cH[bp],0009H
                jmp     short L164
L163:           mov     word ptr -1cH[bp],0008H
L164:           mov     ax,word ptr -1cH[bp]
                mov     word ptr -0cH[bp],ax
                jmp     near ptr L183
L165:           cmp     byte ptr -0aH[bp],00H
                je      short L166
                mov     word ptr -1eH[bp],000bH
                jmp     short L167
L166:           mov     word ptr -1eH[bp],000aH
L167:           mov     ax,word ptr -1eH[bp]
                mov     word ptr -0cH[bp],ax
                jmp     near ptr L183
L168:           lds     bx,dword ptr ss:_Code
                mov     bl,byte ptr +0eH[bx]
                and     bl,0f0H
                mov     byte ptr -20H[bp],bl
                cmp     byte ptr -20H[bp],20H
                jb      short L170
                cmp     byte ptr -20H[bp],20H
                jbe     short L171
                cmp     byte ptr -20H[bp],40H
                jb      short L169
                cmp     byte ptr -20H[bp],40H
                jbe     short L172
                cmp     byte ptr -20H[bp],50H
                je      short L172
                jmp     short L173
L169:           cmp     byte ptr -20H[bp],30H
                je      short L172
                jmp     short L173
L170:           cmp     byte ptr -20H[bp],00H
                jbe     short L173
                cmp     byte ptr -20H[bp],10H
                je      short L173
                jmp     short L173
                jmp     short L173
L171:           mov     word ptr -0cH[bp],0004H
                jmp     short L174
L172:           mov     word ptr -0cH[bp],0007H
                jmp     short L174
L173:           mov     word ptr -0cH[bp],000dH
L174:           jmp     short L183
L175:           lds     bx,dword ptr -14H[bp]
                mov     al,byte ptr +2H[bx]
                mov     byte ptr -22H[bp],al
                cmp     byte ptr -22H[bp],33H
                jb      short L177
                cmp     byte ptr -22H[bp],33H
                jbe     short L182
                cmp     byte ptr -22H[bp],35H
                jb      short L176
                cmp     byte ptr -22H[bp],35H
                jbe     short L182
                cmp     byte ptr -22H[bp],63H
                je      short L179
                jmp     short L183
L176:           jmp     short L183
L177:           cmp     byte ptr -22H[bp],30H
                jb      short L178
                cmp     byte ptr -22H[bp],30H
                jbe     short L180
                cmp     byte ptr -22H[bp],32H
                je      short L181
                jmp     short L183
L178:           jmp     short L183
L179:           mov     word ptr -0cH[bp],018fH
                jmp     short L183
L180:           mov     word ptr -0cH[bp],000dH
                jmp     short L183
L181:           mov     word ptr -0cH[bp],0004H
                jmp     short L183
L182:           mov     word ptr -0cH[bp],0007H
L183:           mov     ax,word ptr -0cH[bp]
                call    far ptr cpu_directive_
                lea     sp,-8H[bp]
                pop     di
                pop     si
                pop     cx
                pop     bx
                pop     bp
                ret
set_build_target_:
                push    bp
                mov     bp,sp
                mov     ax,0022H
                call    far ptr __STK
                push    bx
                push    cx
                push    dx
                push    si
                push    di
                sub     sp,0010H
                mov     word ptr -12H[bp],cs
                mov     word ptr -14H[bp],offset L42
                mov     cx,word ptr ss:_Options+0cH
                mov     bx,word ptr ss:_Options+0eH
                xor     dx,dx
                xor     ax,ax
                cmp     bx,ax
                jne     short L186
                cmp     cx,dx
                jne     short L186
                mov     ax,0004H
                call    far ptr AsmAlloc_
                mov     bx,ax
                mov     ax,dx
                mov     word ptr ss:_Options+0cH,bx
                mov     word ptr ss:_Options+0eH,ax
                mov     al,byte ptr ss:__osmode
                mov     byte ptr -16H[bp],al
                cmp     byte ptr -16H[bp],00H
                jbe     short L184
                cmp     byte ptr -16H[bp],01H
                je      short L185
                jmp     short L186
                jmp     short L186
L184:           lds     bx,dword ptr ss:_Options+0cH
                mov     dx,word ptr cs:L34
                mov     ax,word ptr cs:L35
                mov     word ptr [bx],dx
                mov     word ptr +2H[bx],ax
                jmp     short L186
L185:           lds     bx,dword ptr ss:_Options+0cH
                mov     dx,word ptr cs:L32
                mov     ax,word ptr cs:L33
                mov     word ptr [bx],dx
                mov     word ptr +2H[bx],ax
L186:           mov     bx,word ptr ss:_Options+0cH
                mov     dx,word ptr ss:_Options+0eH
                mov     ax,bx
                call    far ptr strlen_
                add     ax,0006H
                and     al,0feH
                mov     dx,ax
                call    far ptr stackavail_
                cmp     dx,ax
                jae     short L187
                mov     bx,word ptr ss:_Options+0cH
                mov     dx,word ptr ss:_Options+0eH
                mov     ax,bx
                call    far ptr strlen_
                add     ax,0006H
                and     al,0feH
                sub     sp,ax
                mov     ax,sp
                mov     dx,ss
                mov     bx,dx
                mov     word ptr -1aH[bp],ax
                mov     word ptr -18H[bp],bx
                jmp     short L188
L187:           mov     word ptr -1aH[bp],0000H
                mov     word ptr -18H[bp],0000H
L188:           mov     ax,word ptr -1aH[bp]
                mov     word ptr -10H[bp],ax
                mov     ax,word ptr -18H[bp]
                mov     word ptr -0eH[bp],ax
                mov     si,word ptr -14H[bp]
                mov     cx,word ptr -12H[bp]
                mov     bx,word ptr -10H[bp]
                mov     dx,word ptr -0eH[bp]
                mov     ax,bx
                mov     bx,si
                call    far ptr strcpy_
                mov     si,word ptr ss:_Options+0cH
                mov     cx,word ptr ss:_Options+0eH
                mov     bx,word ptr -10H[bp]
                mov     dx,word ptr -0eH[bp]
                mov     ax,bx
                mov     bx,si
                call    far ptr strcat_
                mov     si,word ptr -14H[bp]
                mov     cx,word ptr -12H[bp]
                mov     bx,word ptr -10H[bp]
                mov     dx,word ptr -0eH[bp]
                mov     ax,bx
                mov     bx,si
                call    far ptr strcat_
                mov     bx,word ptr -10H[bp]
                mov     dx,word ptr -0eH[bp]
                mov     ax,bx
                call    far ptr strupr_
                mov     bx,word ptr -10H[bp]
                mov     dx,word ptr -0eH[bp]
                mov     ax,bx
                call    near ptr add_constant_
                mov     cx,cs
                mov     ax,offset L34
                mov     si,ax
                mov     bx,word ptr ss:_Options+0cH
                mov     dx,word ptr ss:_Options+0eH
                mov     ax,bx
                mov     bx,si
                call    far ptr stricmp_
                test    ax,ax
                jne     short L189
                mov     dx,cs
                mov     ax,offset L9
                mov     bx,ax
                mov     ax,bx
                call    near ptr add_constant_
                jmp     near ptr L198
L189:           mov     cx,cs
                mov     dx,offset L31
                mov     si,dx
                mov     bx,word ptr ss:_Options+0cH
                mov     dx,word ptr ss:_Options+0eH
                mov     ax,bx
                mov     bx,si
                call    far ptr stricmp_
                test    ax,ax
                jne     short L190
                jmp     near ptr L198
L190:           mov     cx,cs
                mov     dx,offset L32
                mov     si,dx
                mov     bx,word ptr ss:_Options+0cH
                mov     dx,word ptr ss:_Options+0eH
                mov     ax,bx
                mov     bx,si
                call    far ptr stricmp_
                test    ax,ax
                jne     short L191
                jmp     near ptr L198
L191:           mov     cx,cs
                mov     dx,offset L41
                mov     si,dx
                mov     bx,word ptr ss:_Options+0cH
                mov     dx,word ptr ss:_Options+0eH
                mov     ax,bx
                mov     bx,si
                call    far ptr stricmp_
                test    ax,ax
                jne     short L192
                jmp     near ptr L198
L192:           mov     cx,cs
                mov     dx,offset L11
                mov     si,dx
                mov     bx,word ptr ss:_Options+0cH
                mov     dx,word ptr ss:_Options+0eH
                mov     ax,bx
                mov     bx,si
                call    far ptr stricmp_
                test    ax,ax
                jne     short L193
                jmp     near ptr L198
L193:           mov     cx,cs
                mov     dx,offset L19
                mov     si,dx
                mov     bx,word ptr ss:_Options+0cH
                mov     dx,word ptr ss:_Options+0eH
                mov     ax,bx
                mov     bx,si
                call    far ptr stricmp_
                test    ax,ax
                jne     short L195
                lds     bx,dword ptr ss:_Code
                test    byte ptr +0eH[bx],70H
                je      short L194
                mov     dx,cs
                mov     ax,offset L6
                mov     bx,ax
                mov     ax,bx
                call    near ptr add_constant_
L194:           jmp     short L198
L195:           mov     cx,cs
                mov     dx,offset L18
                mov     si,dx
                mov     bx,word ptr ss:_Options+0cH
                mov     dx,word ptr ss:_Options+0eH
                mov     ax,bx
                mov     bx,si
                call    far ptr stricmp_
                test    ax,ax
                jne     short L197
                lds     bx,dword ptr ss:_Code
                test    byte ptr +0eH[bx],70H
                je      short L196
                mov     dx,cs
                mov     ax,offset L5
                mov     bx,ax
                mov     ax,bx
                call    near ptr add_constant_
L196:           jmp     short L198
L197:           mov     ax,045eH
                push    ax
                mov     ax,0002H
                push    ax
                call    far ptr AsmWarn_
                add     sp,0004H
L198:           mov     word ptr -0cH[bp],0001H
                mov     ax,word ptr -0cH[bp]
                lea     sp,-0aH[bp]
                pop     di
                pop     si
                pop     dx
                pop     cx
                pop     bx
                pop     bp
                ret
set_mem_type_:  push    bp
                mov     bp,sp
                push    ax
                mov     ax,002aH
                call    far ptr __STK
                pop     ax
                push    bx
                push    cx
                push    dx
                push    si
                push    di
                sub     sp,001cH
                mov     byte ptr -0cH[bp],al
                jmp     short L200
L199            DW      L201
                DW      L208
                DW      L208
                DW      L202
                DW      L208
                DW      L203
                DW      L208
                DW      L208
                DW      L208
                DW      L204
                DW      L205
                DW      L208
                DW      L208
                DW      L208
                DW      L208
                DW      L208
                DW      L206
                DW      L207
L200:           mov     al,byte ptr -0cH[bp]
                sub     al,63H
                mov     byte ptr -26H[bp],al
                cmp     byte ptr -26H[bp],11H
                ja      short L208
                mov     al,byte ptr -26H[bp]
                xor     ah,ah
                shl     ax,1
                mov     bx,ax
                jmp     word ptr cs:L199[bx]
L201:           mov     word ptr -0eH[bp],cs
                mov     word ptr -10H[bp],offset L17
                jmp     near ptr L209
L202:           mov     word ptr -0eH[bp],cs
                mov     word ptr -10H[bp],offset L27
                jmp     short L209
L203:           mov     word ptr -0eH[bp],cs
                mov     word ptr -10H[bp],offset L26
                jmp     short L209
L204:           mov     word ptr -0eH[bp],cs
                mov     word ptr -10H[bp],offset L24
                jmp     short L209
L205:           mov     word ptr -0eH[bp],cs
                mov     word ptr -10H[bp],offset L21
                jmp     short L209
L206:           mov     word ptr -0eH[bp],cs
                mov     word ptr -10H[bp],offset L23
                jmp     short L209
L207:           mov     word ptr -0eH[bp],cs
                mov     word ptr -10H[bp],offset L25
                jmp     short L209
L208:           mov     ax,ss
                mov     es,ax
                lea     di,-24H[bp]
                mov     ax,cs
                mov     ds,ax
                mov     si,offset L40
                movsw
                movsb
                mov     cx,ss
                lea     ax,-0cH[bp]
                mov     si,ax
                mov     dx,ss
                lea     ax,-24H[bp]
                mov     bx,ax
                mov     ax,bx
                mov     bx,si
                call    far ptr strcat_
                mov     cx,ss
                lea     ax,-24H[bp]
                mov     dx,ax
                mov     ax,044eH
                mov     bx,dx
                call    far ptr MsgPrintf1_
                mov     ax,0001H
                jmp     far ptr exit_
L209:           mov     ax,ss
                mov     es,ax
                lea     di,-24H[bp]
                mov     ax,cs
                mov     ds,ax
                mov     si,offset L16
                movsw
                movsw
                movsw
                movsw
                mov     si,word ptr -10H[bp]
                mov     cx,word ptr -0eH[bp]
                mov     dx,ss
                lea     ax,-24H[bp]
                mov     bx,ax
                mov     ax,bx
                mov     bx,si
                call    far ptr strcat_
                mov     dx,ss
                lea     ax,-24H[bp]
                mov     bx,ax
                mov     ax,bx
                call    far ptr InputQueueLine_
                lea     sp,-0aH[bp]
                pop     di
                pop     si
                pop     dx
                pop     cx
                pop     bx
                pop     bp
                ret
set_some_kinda_name_:
                push    bp
                mov     bp,sp
                push    ax
                mov     ax,0018H
                call    far ptr __STK
                pop     ax
                push    dx
                push    si
                push    di
                sub     sp,000eH
                mov     byte ptr -8H[bp],al
                mov     word ptr -0eH[bp],bx
                mov     word ptr -0cH[bp],cx
                mov     bx,word ptr -0eH[bp]
                mov     dx,word ptr -0cH[bp]
                mov     ax,bx
                call    far ptr strlen_
                inc     ax
                mov     word ptr -0aH[bp],ax
                mov     al,byte ptr -8H[bp]
                mov     byte ptr -14H[bp],al
                cmp     byte ptr -14H[bp],64H
                jb      short L211
                cmp     byte ptr -14H[bp],64H
                jbe     short L212
                cmp     byte ptr -14H[bp],6dH
                jb      short L210
                cmp     byte ptr -14H[bp],6dH
                jbe     short L213
                cmp     byte ptr -14H[bp],74H
                je      short L214
                jmp     short L215
L210:           jmp     short L215
L211:           cmp     byte ptr -14H[bp],63H
                jne     short L215
                mov     word ptr -10H[bp],ss
                mov     word ptr -12H[bp],offset DGROUP:_Options+10H
                jmp     short L216
L212:           mov     word ptr -10H[bp],ss
                mov     word ptr -12H[bp],offset DGROUP:_Options+14H
                jmp     short L216
L213:           mov     word ptr -10H[bp],ss
                mov     word ptr -12H[bp],offset DGROUP:_Options+1cH
                jmp     short L216
L214:           mov     word ptr -10H[bp],ss
                mov     word ptr -12H[bp],offset DGROUP:_Options+18H
                jmp     short L216
L215:           jmp     short L217
L216:           mov     ax,word ptr -0aH[bp]
                call    far ptr AsmAlloc_
                mov     cx,ax
                mov     bx,dx
                lds     si,dword ptr -12H[bp]
                mov     word ptr [si],cx
                lds     si,dword ptr -12H[bp]
                mov     word ptr +2H[si],bx
                mov     si,word ptr -0eH[bp]
                mov     cx,word ptr -0cH[bp]
                lds     bx,dword ptr -12H[bp]
                mov     bx,word ptr [bx]
                lds     di,dword ptr -12H[bp]
                mov     dx,word ptr +2H[di]
                mov     ax,bx
                mov     bx,si
                call    far ptr strcpy_
L217:           lea     sp,-6H[bp]
                pop     di
                pop     si
                pop     dx
                pop     bp
                ret
parse_token_:   push    bp
                mov     bp,sp
                push    ax
                mov     ax,0020H
                call    far ptr __STK
                pop     ax
                push    cx
                push    si
                push    di
                sub     sp,0012H
                mov     word ptr -0cH[bp],ax
                mov     word ptr -0aH[bp],dx
                mov     word ptr -8H[bp],bx
                lds     bx,dword ptr -0cH[bp]
                mov     al,byte ptr [bx]
                mov     byte ptr -0eH[bp],al
                cmp     byte ptr -0eH[bp],2fH
                jb      short L220
                cmp     byte ptr -0eH[bp],2fH
                jbe     short L224
                cmp     byte ptr -0eH[bp],3fH
                jb      short L219
                cmp     byte ptr -0eH[bp],3fH
                jbe     short L222
                cmp     byte ptr -0eH[bp],40H
                jne     short L218
                jmp     near ptr L298
L218:           jmp     near ptr L299
L219:           cmp     byte ptr -0eH[bp],3dH
                je      short L223
                jmp     near ptr L299
L220:           cmp     byte ptr -0eH[bp],23H
                jb      short L221
                cmp     byte ptr -0eH[bp],23H
                jbe     short L223
                cmp     byte ptr -0eH[bp],2dH
                je      short L224
                jmp     near ptr L299
L221:           jmp     near ptr L299
L222:           call    near ptr usage_msg_
                jmp     near ptr L300
L223:           mov     ax,043dH
                call    far ptr AsmError_
L224:           mov     dx,word ptr -0cH[bp]
                mov     ax,word ptr -0aH[bp]
                inc     word ptr -0cH[bp]
                lds     bx,dword ptr -0cH[bp]
                mov     al,byte ptr [bx]
                xor     ah,ah
                call    far ptr tolower_
                mov     word ptr -10H[bp],ax
                cmp     word ptr -10H[bp],0068H
                jb      short L237
                cmp     word ptr -10H[bp],0068H
                ja      short L225
                jmp     near ptr L259
L225:           cmp     word ptr -10H[bp],006eH
                jb      short L233
                cmp     word ptr -10H[bp],006eH
                ja      short L226
                jmp     near ptr L255
L226:           cmp     word ptr -10H[bp],0073H
                jb      short L231
                cmp     word ptr -10H[bp],0073H
                ja      short L227
                jmp     near ptr L269
L227:           cmp     word ptr -10H[bp],0077H
                jb      short L230
                cmp     word ptr -10H[bp],0077H
                ja      short L228
                jmp     near ptr L266
L228:           cmp     word ptr -10H[bp],007aH
                jne     short L229
                jmp     near ptr L295
L229:           jmp     near ptr L296
L230:           jmp     near ptr L296
L231:           cmp     word ptr -10H[bp],0071H
                jne     short L232
                jmp     near ptr L294
L232:           jmp     near ptr L296
L233:           cmp     word ptr -10H[bp],006aH
                jb      short L236
                cmp     word ptr -10H[bp],006aH
                ja      short L234
                jmp     near ptr L269
L234:           cmp     word ptr -10H[bp],006dH
                jne     short L235
                jmp     near ptr L254
L235:           jmp     near ptr L296
L236:           jmp     near ptr L260
L237:           cmp     word ptr -10H[bp],0062H
                jb      short L242
                cmp     word ptr -10H[bp],0062H
                jbe     short L247
                cmp     word ptr -10H[bp],0065H
                jb      short L240
                cmp     word ptr -10H[bp],0065H
                ja      short L238
                jmp     near ptr L263
L238:           cmp     word ptr -10H[bp],0066H
                jne     short L239
                jmp     near ptr L276
L239:           jmp     near ptr L296
L240:           cmp     word ptr -10H[bp],0064H
                jne     short L241
                jmp     near ptr L270
L241:           jmp     near ptr L296
L242:           cmp     word ptr -10H[bp],0037H
                jb      short L244
                cmp     word ptr -10H[bp],0037H
                jbe     short L246
                cmp     word ptr -10H[bp],003fH
                jne     short L243
                jmp     near ptr L259
L243:           jmp     near ptr L296
L244:           cmp     word ptr -10H[bp],0030H
                jb      short L245
                cmp     word ptr -10H[bp],0035H
                jbe     short L246
                jmp     near ptr L296
L245:           jmp     near ptr L296
L246:           mov     bx,word ptr -0cH[bp]
                mov     dx,word ptr -0aH[bp]
                mov     ax,bx
                call    near ptr set_processor_type_
                jmp     near ptr L297
L247:           lds     bx,dword ptr -0cH[bp]
                cmp     byte ptr +1H[bx],74H
                je      short L248
                jmp     near ptr L253
L248:           add     word ptr -0cH[bp],0002H
                lds     bx,dword ptr -0cH[bp]
                cmp     byte ptr [bx],3dH
                je      short L249
                lds     bx,dword ptr -0cH[bp]
                cmp     byte ptr [bx],23H
                jne     short L250
L249:           mov     dx,word ptr -0cH[bp]
                mov     ax,word ptr -0aH[bp]
                inc     word ptr -0cH[bp]
L250:           mov     cx,word ptr ss:_Options+0cH
                mov     bx,word ptr ss:_Options+0eH
                xor     dx,dx
                xor     ax,ax
                cmp     bx,ax
                jne     short L251
                cmp     cx,dx
                je      short L252
L251:           mov     bx,word ptr ss:_Options+0cH
                mov     dx,word ptr ss:_Options+0eH
                mov     ax,bx
                call    far ptr AsmFree_
L252:           mov     bx,word ptr -0cH[bp]
                mov     dx,word ptr -0aH[bp]
                mov     ax,bx
                call    far ptr strlen_
                inc     ax
                call    far ptr AsmAlloc_
                mov     bx,dx
                mov     word ptr ss:_Options+0cH,ax
                mov     word ptr ss:_Options+0eH,bx
                mov     si,word ptr -0cH[bp]
                mov     cx,word ptr -0aH[bp]
                mov     bx,word ptr ss:_Options+0cH
                mov     dx,word ptr ss:_Options+0eH
                mov     ax,bx
                mov     bx,si
                call    far ptr strcpy_
                jmp     near ptr L297
L253:           mov     dx,word ptr -0cH[bp]
                mov     cx,word ptr -0aH[bp]
                mov     ax,044eH
                mov     bx,dx
                call    far ptr MsgPrintf1_
                mov     ax,0001H
                jmp     far ptr exit_
L254:           lds     bx,dword ptr -0cH[bp]
                mov     al,byte ptr +1H[bx]
                xor     ah,ah
                call    near ptr set_mem_type_
                jmp     near ptr L297
L255:           lds     bx,dword ptr -0cH[bp]
                cmp     byte ptr +2H[bx],3dH
                je      short L256
                lds     bx,dword ptr -0cH[bp]
                cmp     byte ptr +2H[bx],23H
                jne     short L257
L256:           mov     cx,word ptr -0aH[bp]
                mov     ax,word ptr -0cH[bp]
                add     ax,0003H
                mov     dx,ax
                lds     bx,dword ptr -0cH[bp]
                mov     al,byte ptr +1H[bx]
                xor     ah,ah
                mov     bx,dx
                call    near ptr set_some_kinda_name_
                jmp     short L258
L257:           mov     cx,word ptr -0aH[bp]
                mov     ax,word ptr -0cH[bp]
                add     ax,0002H
                mov     dx,ax
                lds     bx,dword ptr -0cH[bp]
                mov     al,byte ptr +1H[bx]
                xor     ah,ah
                mov     bx,dx
                call    near ptr set_some_kinda_name_
L258:           jmp     near ptr L297
L259:           call    near ptr usage_msg_
                jmp     near ptr L297
L260:           mov     dx,word ptr -0cH[bp]
                mov     ax,word ptr -0aH[bp]
                inc     word ptr -0cH[bp]
                lds     bx,dword ptr -0cH[bp]
                cmp     byte ptr [bx],3dH
                je      short L261
                lds     bx,dword ptr -0cH[bp]
                cmp     byte ptr [bx],23H
                jne     short L262
L261:           mov     dx,word ptr -0cH[bp]
                mov     ax,word ptr -0aH[bp]
                inc     word ptr -0cH[bp]
L262:           mov     bx,word ptr -0cH[bp]
                mov     dx,word ptr -0aH[bp]
                mov     ax,bx
                call    far ptr AddStringToIncludePath_
                jmp     near ptr L297
L263:           lds     bx,dword ptr -0cH[bp]
                mov     bl,byte ptr +1H[bx]
                inc     bl
                xor     bh,bh
                test    byte ptr ss:__IsTable[bx],20H
                je      short L264
                mov     dx,word ptr -0aH[bp]
                mov     ax,word ptr -0cH[bp]
                inc     ax
                mov     bx,ax
                mov     ax,bx
                call    far ptr atoi_
                mov     byte ptr ss:_Options+9H,al
                jmp     short L265
L264:           lds     bx,dword ptr -0cH[bp]
                cmp     byte ptr +1H[bx],00H
                jne     short L265
                mov     byte ptr ss:_Options+1H,01H
L265:           jmp     near ptr L297
L266:           lds     bx,dword ptr -0cH[bp]
                mov     bl,byte ptr +1H[bx]
                inc     bl
                xor     bh,bh
                test    byte ptr ss:__IsTable[bx],20H
                je      short L267
                mov     dx,word ptr -0aH[bp]
                mov     ax,word ptr -0cH[bp]
                inc     ax
                mov     bx,ax
                mov     ax,bx
                call    far ptr atoi_
                mov     byte ptr ss:_Options+0aH,al
                jmp     short L268
L267:           lds     bx,dword ptr -0cH[bp]
                cmp     byte ptr +1H[bx],65H
                jne     short L268
                mov     byte ptr ss:_Options+0bH,01H
L268:           jmp     near ptr L297
L269:           mov     byte ptr ss:_Options,01H
                jmp     near ptr L297
L270:           lds     bx,dword ptr -0cH[bp]
                mov     al,byte ptr +1H[bx]
                mov     byte ptr -12H[bp],al
                cmp     byte ptr -12H[bp],31H
                jb      short L271
                cmp     byte ptr -12H[bp],32H
                jbe     short L272
                cmp     byte ptr -12H[bp],36H
                je      short L273
                jmp     short L274
L271:           jmp     short L274
L272:           mov     byte ptr ss:_Options+4H,01H
                jmp     near ptr L300
L273:           mov     byte ptr ss:_Options+20H,01H
                mov     ax,cs
                mov     dx,offset L4
                push    ax
                push    dx
                call    far ptr DoDebugMsg_
                add     sp,0004H
                jmp     near ptr L300
L274:           mov     dx,word ptr -0aH[bp]
                mov     ax,word ptr -0cH[bp]
                inc     ax
                mov     bx,ax
                mov     ax,bx
                call    near ptr add_constant_
                jmp     near ptr L297
                cld
L275            DW      L284
                DW      L293
                DW      L293
                DW      L293
                DW      L278
                DW      L293
                DW      L293
                DW      L293
                DW      L293
                DW      L293
                DW      L281
                DW      L287
L276:           lds     bx,dword ptr -0cH[bp]
                mov     bl,byte ptr +1H[bx]
                sub     bl,65H
                mov     byte ptr -14H[bp],bl
                cmp     byte ptr -14H[bp],0bH
                jbe     short L277
                jmp     near ptr L293
L277:           mov     al,byte ptr -14H[bp]
                xor     ah,ah
                shl     ax,1
                mov     bx,ax
                jmp     word ptr cs:L275[bx]
L278:           add     word ptr -0cH[bp],0002H
                lds     bx,dword ptr -0cH[bp]
                cmp     byte ptr [bx],3dH
                je      short L279
                lds     bx,dword ptr -0cH[bp]
                cmp     byte ptr [bx],23H
                jne     short L280
L279:           mov     dx,word ptr -0cH[bp]
                mov     ax,word ptr -0aH[bp]
                inc     word ptr -0cH[bp]
L280:           mov     bx,word ptr -0cH[bp]
                mov     dx,word ptr -0aH[bp]
                mov     ax,bx
                call    far ptr InputQueueFile_
                jmp     near ptr L300
L281:           add     word ptr -0cH[bp],0002H
                lds     bx,dword ptr -0cH[bp]
                cmp     byte ptr [bx],3dH
                je      short L282
                lds     bx,dword ptr -0cH[bp]
                cmp     byte ptr [bx],23H
                jne     short L283
L282:           mov     dx,word ptr -0cH[bp]
                mov     ax,word ptr -0aH[bp]
                inc     word ptr -0cH[bp]
L283:           mov     cx,0002H
                mov     bx,word ptr -0cH[bp]
                mov     dx,word ptr -0aH[bp]
                mov     ax,bx
                mov     bx,cx
                call    near ptr get_fname_
                jmp     near ptr L300
L284:           add     word ptr -0cH[bp],0002H
                lds     bx,dword ptr -0cH[bp]
                cmp     byte ptr [bx],3dH
                je      short L285
                lds     bx,dword ptr -0cH[bp]
                cmp     byte ptr [bx],23H
                jne     short L286
L285:           mov     dx,word ptr -0cH[bp]
                mov     ax,word ptr -0aH[bp]
                inc     word ptr -0cH[bp]
L286:           mov     cx,0001H
                mov     bx,word ptr -0cH[bp]
                mov     dx,word ptr -0aH[bp]
                mov     ax,bx
                mov     bx,cx
                call    near ptr get_fname_
                jmp     near ptr L300
L287:           mov     cx,cs
                mov     dx,offset L30
                mov     si,dx
                mov     bx,word ptr -0cH[bp]
                mov     dx,word ptr -0aH[bp]
                mov     ax,bx
                mov     bx,si
                call    far ptr stricmp_
                test    ax,ax
                jne     short L288
                mov     byte ptr ss:_Options+6H,00H
                mov     dx,cs
                mov     ax,offset L15
                mov     bx,ax
                mov     ax,bx
                call    near ptr add_constant_
                mov     dx,cs
                mov     ax,offset L43
                mov     bx,ax
                mov     ax,bx
                call    near ptr set_processor_type_
                jmp     near ptr L300
L288:           mov     cx,cs
                mov     dx,offset L22
                mov     si,dx
                mov     bx,word ptr -0cH[bp]
                mov     dx,word ptr -0aH[bp]
                mov     ax,bx
                mov     bx,si
                call    far ptr stricmp_
                test    ax,ax
                jne     short L289
                mov     byte ptr ss:_Options+6H,01H
                mov     dx,cs
                mov     ax,offset L8
                mov     bx,ax
                mov     ax,bx
                call    near ptr add_constant_
                mov     dx,cs
                mov     ax,offset L43
                mov     bx,ax
                mov     ax,bx
                call    near ptr set_processor_type_
                jmp     near ptr L300
L289:           mov     cx,cs
                mov     dx,offset L29
                mov     si,dx
                mov     bx,word ptr -0cH[bp]
                mov     dx,word ptr -0aH[bp]
                mov     ax,bx
                mov     bx,si
                call    far ptr stricmp_
                test    ax,ax
                jne     short L291
                mov     byte ptr ss:_Options+6H,02H
                mov     dx,cs
                mov     ax,offset L14
                mov     bx,ax
                mov     ax,bx
                call    near ptr add_constant_
                mov     bx,word ptr -0cH[bp]
                mov     dx,word ptr -0aH[bp]
                mov     ax,bx
                call    near ptr set_processor_type_
                jmp     near ptr L300
L290            DW      L292
                DW      L293
                DW      L292
                DW      L292
                DW      L293
                DW      L292
L291:           lds     bx,dword ptr -0cH[bp]
                mov     bl,byte ptr +2H[bx]
                sub     bl,30H
                mov     byte ptr -16H[bp],bl
                cmp     byte ptr -16H[bp],05H
                ja      short L293
                mov     al,byte ptr -16H[bp]
                xor     ah,ah
                shl     ax,1
                mov     bx,ax
                jmp     word ptr cs:L290[bx]
L292:           mov     bx,word ptr -0cH[bp]
                mov     dx,word ptr -0aH[bp]
                mov     ax,bx
                call    near ptr set_processor_type_
                jmp     short L300
L293:           mov     dx,word ptr -0cH[bp]
                mov     cx,word ptr -0aH[bp]
                mov     ax,044eH
                mov     bx,dx
                call    far ptr MsgPrintf1_
                mov     ax,0001H
                jmp     far ptr exit_
L294:           mov     byte ptr ss:_Options+2H,01H
                jmp     short L297
L295:           lds     bx,dword ptr -0cH[bp]
                mov     al,byte ptr +1H[bx]
                mov     byte ptr -18H[bp],al
                cmp     byte ptr -18H[bp],71H
                jne     short L296
                mov     byte ptr ss:_Options+2H,01H
                jmp     short L300
L296:           mov     dx,word ptr -0cH[bp]
                mov     cx,word ptr -0aH[bp]
                mov     ax,044eH
                mov     bx,dx
                call    far ptr MsgPrintf1_
                mov     ax,0001H
                jmp     far ptr exit_
L297:           jmp     short L300
L298:           mov     cx,word ptr -8H[bp]
                mov     dx,word ptr -0aH[bp]
                mov     ax,word ptr -0cH[bp]
                inc     ax
                mov     bx,ax
                mov     ax,bx
                mov     bx,cx
                call    near ptr do_envvar_cmdline_
                jmp     short L300
L299:           xor     cx,cx
                mov     bx,word ptr -0cH[bp]
                mov     dx,word ptr -0aH[bp]
                mov     ax,bx
                mov     bx,cx
                call    near ptr get_fname_
L300:           lea     sp,-6H[bp]
                pop     di
                pop     si
                pop     cx
                pop     bp
                ret
parse_cmdline_: push    bp
                mov     bp,sp
                push    ax
                mov     ax,0068H
                call    far ptr __STK
                pop     ax
                push    dx
                push    si
                push    di
                sub     sp,0058H
                mov     word ptr -0aH[bp],ax
                mov     word ptr -0eH[bp],bx
                mov     word ptr -0cH[bp],cx
                cmp     word ptr -0aH[bp],0001H
                jne     short L301
                call    near ptr usage_msg_
L301:           mov     word ptr -8H[bp],0001H
L302:           mov     ax,word ptr -8H[bp]
                cmp     ax,word ptr -0aH[bp]
                jl      short L304
                jmp     short L305
L303:           mov     ax,word ptr -8H[bp]
                inc     word ptr -8H[bp]
                jmp     short L302
L304:           xor     cx,cx
                mov     si,word ptr -8H[bp]
                shl     si,1
                shl     si,1
                mov     ds,word ptr -0cH[bp]
                add     si,word ptr -0eH[bp]
                mov     bx,word ptr [si]
                mov     dx,word ptr +2H[si]
                mov     ax,bx
                mov     bx,cx
                call    near ptr parse_token_
                jmp     short L303
L305:           mov     cx,word ptr ss:_AsmFiles+0cH
                mov     bx,word ptr ss:_AsmFiles+0eH
                xor     dx,dx
                xor     ax,ax
                cmp     bx,ax
                jne     short L306
                cmp     cx,dx
                jne     short L306
                mov     cx,ss
                lea     ax,-5eH[bp]
                mov     dx,ax
                mov     ax,0440H
                mov     bx,dx
                call    far ptr MsgGet_
                mov     dx,ss
                lea     cx,-5eH[bp]
                push    dx
                push    cx
                mov     ax,0001H
                push    ax
                call    far ptr Fatal_
                add     sp,0006H
L306:           lea     sp,-6H[bp]
                pop     di
                pop     si
                pop     dx
                pop     bp
                ret
AsmQueryExternal_:
                inc     bp
                push    bp
                mov     bp,sp
                push    ax
                mov     ax,0014H
                call    far ptr __STK
                pop     ax
                push    bx
                push    cx
                push    si
                push    di
                sub     sp,0006H
                mov     word ptr -0eH[bp],ax
                mov     word ptr -0cH[bp],dx
                mov     byte ptr -0aH[bp],00H
                mov     al,byte ptr -0aH[bp]
                lea     sp,-8H[bp]
                pop     di
                pop     si
                pop     cx
                pop     bx
                pop     bp
                dec     bp
                retf
AsmQueryType_:  inc     bp
                push    bp
                mov     bp,sp
                push    ax
                mov     ax,0014H
                call    far ptr __STK
                pop     ax
                push    bx
                push    cx
                push    si
                push    di
                sub     sp,0006H
                mov     word ptr -0eH[bp],ax
                mov     word ptr -0cH[bp],dx
                mov     byte ptr -0aH[bp],00H
                mov     al,byte ptr -0aH[bp]
                lea     sp,-8H[bp]
                pop     di
                pop     si
                pop     cx
                pop     bx
                pop     bp
                dec     bp
                retf
main_TEXT       ENDS

CONST           SEGMENT WORD PUBLIC  'DATA'
CONST           ENDS

CONST2          SEGMENT WORD PUBLIC  'DATA'
CONST2          ENDS

_DATA           SEGMENT WORD PUBLIC  'DATA'
                PUBLIC  _Options
_Options        DB      00H,00H,00H,00H,00H,00H,02H,00H
                DB      00H,14H,02H,00H,00H,00H,00H,00H
                DB      00H,00H,00H,00H,00H,00H,00H,00H
                DB      00H,00H,00H,00H,00H,00H,00H,00H
                DB      00H
L307            DD      L3
                DD      L2
                DD      L1
                DD      L46
                DB      00H,00H,00H,00H
                ORG     00000023H
L308            LABEL   BYTE
_DATA           ENDS

_BSS            SEGMENT WORD PUBLIC  'BSS'
                PUBLIC  _AsmFiles
                PUBLIC  _pobjState
                ORG     00000000H
_AsmFiles       LABEL   BYTE
                ORG     00000018H
_pobjState      LABEL   BYTE
                ORG     0000001dH
_BSS            ENDS

$$SYMBOLS       SEGMENT BYTE  'DEBSYM'
                DB      0fH,10H
                DD      DGROUP:_AsmFiles
                DB      32H,41H,73H,6dH,46H,69H,6cH,65H
                DB      73H,10H,10H
                DD      DGROUP:_pobjState
                DB      38H,70H,6fH,62H,6aH,53H,74H,61H
                DB      74H,65H,0eH,10H
                DD      DGROUP:_Options
                DB      3dH,4fH,70H,74H,69H,6fH,6eH,73H
                DB      06H,31H
                DD      isvalidident_
                DB      1eH,21H,00H,00H,0c7H,00H,00H,00H
                DB      15H,09H,02H,00H,3eH,48H,01H,31H
                DB      08H,0bH,69H,73H,76H,61H,6cH,69H
                DB      64H,69H,64H,65H,6eH,74H,06H,11H
                DB      10H,0f0H,11H,73H,0dH,11H,10H,0f6H
                DB      08H,6cH,77H,72H,5fH,63H,68H,61H
                DB      72H,07H,11H,10H,0ecH,11H,69H,64H
                DB      1dH,21H,0c7H,00H,0e0H,00H,00H,00H
                DB      14H,0aH,02H,00H,3fH,00H,00H,67H
                DB      65H,74H,5fH,6fH,73H,5fH,69H,6eH
                DB      63H,6cH,75H,64H,65H,08H,11H,10H
                DB      0f2H,11H,65H,6eH,76H,08H,11H,10H
                DB      0eeH,11H,74H,6dH,70H,20H,22H,0a7H
                DB      01H,0deH,00H,00H,00H,15H,09H,02H
                DB      00H,41H,00H,02H,48H,31H,09H,0aH
                DB      64H,6fH,5fH,69H,6eH,69H,74H,5fH
                DB      73H,74H,75H,66H,66H,08H,11H,10H
                DB      0f4H,11H,65H,6eH,76H,09H,11H,10H
                DB      0f0H,40H,61H,72H,67H,76H,09H,11H
                DB      10H,0f8H,06H,61H,72H,67H,63H,17H
                DB      22H,85H,02H,5fH,00H,00H,00H,15H
                DB      09H,02H,00H,42H,48H,02H,48H,31H
                DB      09H,0aH,6dH,61H,69H,6eH,09H,11H
                DB      10H,0f2H,40H,61H,72H,67H,76H,09H
                DB      11H,10H,0f6H,06H,61H,72H,67H,63H
                DB      18H,21H,0e4H,02H,24H,00H,00H,00H
                DB      14H,00H,02H,00H,3fH,00H,00H,75H
                DB      73H,61H,67H,65H,5fH,6dH,73H,67H
                DB      18H,22H,08H,03H,6fH,00H,00H,00H
                DB      15H,0bH,02H,00H,43H,00H,00H,74H
                DB      72H,61H,64H,65H,6dH,61H,72H,6bH
                DB      0bH,11H,20H
                DD      DGROUP:L307
                DB      44H,6dH,73H,67H,0aH,11H,10H,0f4H
                DB      06H,63H,6fH,75H,6eH,74H,19H,21H
                DB      77H,03H,0e1H,00H,00H,00H,14H,0aH
                DB      02H,00H,3fH,00H,00H,66H,72H,65H
                DB      65H,5fH,6eH,61H,6dH,65H,73H,18H
                DB      21H,58H,04H,51H,00H,00H,00H,14H
                DB      0aH,02H,00H,3fH,00H,00H,66H,72H
                DB      65H,65H,5fH,66H,69H,6cH,65H,18H
                DB      21H,0a9H,04H,6eH,00H,00H,00H,14H
                DB      0aH,02H,00H,3fH,00H,00H,6dH,61H
                DB      69H,6eH,5fH,69H,6eH,69H,74H,06H
                DB      11H,10H,0f4H,06H,69H,18H,21H,17H
                DB      05H,33H,00H,00H,00H,14H,0aH,02H
                DB      00H,3fH,00H,00H,6dH,61H,69H,6eH
                DB      5fH,66H,69H,6eH,69H,19H,21H,4aH
                DB      05H,0c5H,00H,00H,00H,14H,0aH,02H
                DB      00H,3fH,00H,00H,6fH,70H,65H,6eH
                DB      5fH,66H,69H,6cH,65H,73H,1cH,21H
                DB      0fH,06H,73H,03H,00H,00H,14H,08H
                DB      02H,00H,45H,00H,02H,31H,08H,0bH
                DB      49H,67H,65H,74H,5fH,66H,6eH,61H
                DB      6dH,65H,0eH,11H,10H,0f4H,11H,64H
                DB      65H,66H,5fH,64H,72H,69H,76H,65H
                DB      0cH,11H,10H,0e4H,11H,64H,65H,66H
                DB      5fH,64H,69H,72H,0eH,11H,10H,0f0H
                DB      11H,64H,65H,66H,5fH,66H,6eH,61H
                DB      6dH,65H,0cH,11H,10H,0ecH,11H,64H
                DB      65H,66H,5fH,65H,78H,74H,0aH,11H
                DB      10H,0e8H,11H,64H,72H,69H,76H,65H
                DB      08H,11H,10H,0d8H,11H,64H,69H,72H
                DB      0aH,11H,10H,0e0H,11H,66H,6eH,61H
                DB      6dH,65H,08H,11H,10H,0dcH,11H,65H
                DB      78H,74H,0cH,11H,11H,78H,0fdH,46H
                DB      62H,75H,66H,66H,65H,72H,0dH,11H
                DB      11H,70H,0fcH,47H,62H,75H,66H,66H
                DB      65H,72H,32H,0aH,11H,11H,80H,0feH
                DB      48H,6eH,61H,6dH,65H,0bH,11H,10H
                DB      84H,49H,6dH,73H,67H,62H,75H,66H
                DB      09H,11H,10H,0f8H,06H,74H,79H,70H
                DB      65H,0aH,11H,10H,0d4H,11H,74H,6fH
                DB      6bH,65H,6eH,24H,21H,82H,09H,40H
                DB      01H,00H,00H,14H,08H,02H,00H,45H
                DB      00H,02H,31H,08H,0bH,49H,64H,6fH
                DB      5fH,65H,6eH,76H,76H,61H,72H,5fH
                DB      63H,6dH,64H,6cH,69H,6eH,65H,0cH
                DB      11H,10H,0f4H,11H,63H,6dH,64H,6cH
                DB      69H,6eH,65H,0aH,11H,10H,0ecH,11H
                DB      74H,6fH,6bH,65H,6eH,09H,11H,10H
                DB      0f0H,11H,6eH,65H,78H,74H,0aH,11H
                DB      10H,0f8H,06H,6cH,65H,76H,65H,6cH
                DB      0bH,11H,10H,0e8H,11H,65H,6eH,76H
                DB      76H,61H,72H,1eH,21H,0c2H,0aH,0e4H
                DB      00H,00H,00H,15H,09H,02H,00H,4aH
                DB      00H,01H,31H,08H,0bH,61H,64H,64H
                DB      5fH,63H,6fH,6eH,73H,74H,61H,6eH
                DB      74H,08H,11H,10H,0f4H,11H,74H,6dH
                DB      70H,08H,11H,10H,0f0H,11H,6fH,6eH
                DB      65H,0bH,11H,10H,0ecH,11H,73H,74H
                DB      72H,69H,6eH,67H,24H,21H,0a6H,0bH
                DB      8cH,02H,00H,00H,15H,09H,02H,00H
                DB      4aH,00H,01H,31H,08H,0bH,73H,65H
                DB      74H,5fH,70H,72H,6fH,63H,65H,73H
                DB      73H,6fH,72H,5fH,74H,79H,70H,65H
                DB      0aH,11H,10H,0f4H,4bH,74H,6fH,6bH
                DB      65H,6eH,0cH,11H,10H,0f6H,08H,70H
                DB      72H,6fH,74H,65H,63H,74H,08H,11H
                DB      10H,0f0H,11H,74H,6dH,70H,0aH,11H
                DB      10H,0ecH,11H,69H,6eH,70H,75H,74H
                DB      1fH,21H,32H,0eH,7fH,02H,00H,00H
                DB      14H,0aH,02H,00H,4dH,48H,00H,73H
                DB      65H,74H,5fH,62H,75H,69H,6cH,64H
                DB      5fH,74H,61H,72H,67H,65H,74H,08H
                DB      11H,10H,0f0H,11H,74H,6dH,70H,0cH
                DB      11H,10H,0ecH,11H,75H,73H,63H,6fH
                DB      72H,65H,73H,1cH,21H,0b1H,10H,22H
                DB      01H,00H,00H,16H,0aH,02H,00H,4eH
                DB      00H,01H,48H,73H,65H,74H,5fH,6dH
                DB      65H,6dH,5fH,74H,79H,70H,65H,0bH
                DB      11H,10H,0dcH,4fH,62H,75H,66H,66H
                DB      65H,72H,0aH,11H,10H,0f0H,11H,6dH
                DB      6fH,64H,65H,6cH,0dH,11H,10H,0f4H
                DB      08H,6dH,65H,6dH,5fH,74H,79H,70H
                DB      65H,26H,21H,0d3H,11H,0bfH,00H,00H
                DB      00H,14H,08H,02H,00H,50H,00H,02H
                DB      48H,31H,09H,0aH,73H,65H,74H,5fH
                DB      73H,6fH,6dH,65H,5fH,6bH,69H,6eH
                DB      64H,61H,5fH,6eH,61H,6dH,65H,08H
                DB      11H,10H,0f6H,06H,6cH,65H,6eH,08H
                DB      11H,10H,0eeH,40H,74H,6dH,70H,09H
                DB      11H,10H,0f2H,11H,6eH,61H,6dH,65H
                DB      0aH,11H,10H,0f8H,08H,74H,6fH,6bH
                DB      65H,6eH,1eH,21H,92H,12H,9cH,05H
                DB      00H,00H,14H,08H,02H,00H,45H,00H
                DB      02H,31H,08H,0bH,49H,70H,61H,72H
                DB      73H,65H,5fH,74H,6fH,6bH,65H,6eH
                DB      12H,11H,10H,0f8H,06H,6eH,65H,73H
                DB      74H,69H,6eH,67H,5fH,6cH,65H,76H
                DB      65H,6cH,0aH,11H,10H,0f4H,11H,74H
                DB      6fH,6bH,65H,6eH,20H,21H,2eH,18H
                DB      9cH,00H,00H,00H,14H,08H,02H,00H
                DB      51H,00H,02H,48H,31H,09H,0aH,70H
                DB      61H,72H,73H,65H,5fH,63H,6dH,64H
                DB      6cH,69H,6eH,65H,06H,11H,10H,0f8H
                DB      06H,69H,0bH,11H,10H,0a2H,52H,6dH
                DB      73H,67H,62H,75H,66H,09H,11H,10H
                DB      0f2H,40H,61H,72H,67H,76H,09H,11H
                DB      10H,0f6H,06H,61H,72H,67H,63H,22H
                DB      22H,0caH,18H,2dH,00H,00H,00H,16H
                DB      0aH,02H,00H,55H,40H,01H,31H,08H
                DB      0bH,41H,73H,6dH,51H,75H,65H,72H
                DB      79H,45H,78H,74H,65H,72H,6eH,61H
                DB      6cH,09H,11H,10H,0f2H,11H,6eH,61H
                DB      6dH,65H,1eH,22H,0f7H,18H,2dH,00H
                DB      00H,00H,16H,0aH,02H,00H,58H,40H
                DB      01H,31H,08H,0bH,41H,73H,6dH,51H
                DB      75H,65H,72H,79H,54H,79H,70H,65H
                DB      09H,11H,10H,0f2H,11H,6eH,61H,6dH
                DB      65H
$$SYMBOLS       ENDS

$$TYPES         SEGMENT BYTE  'DEBTYP'
                DB      08H,11H,73H,74H,72H,75H,63H,74H
                DB      07H,11H,75H,6eH,69H,6fH,6eH,06H
                DB      11H,65H,6eH,75H,6dH,03H,10H,00H
                DB      03H,10H,10H,03H,10H,01H,03H,10H
                DB      11H,08H,12H,00H,05H,63H,68H,61H
                DB      72H,08H,12H,00H,05H,63H,68H,61H
                DB      72H,0aH,12H,00H,07H,73H,69H,7aH
                DB      65H,5fH,74H,0bH,12H,00H,07H,77H
                DB      63H,68H,61H,72H,5fH,74H,0bH,12H
                DB      00H,07H,77H,63H,68H,61H,72H,5fH
                DB      74H,03H,10H,30H,04H,71H,0dH,00H
                DB      03H,41H,0eH,0cH,12H,00H,0fH,6fH
                DB      6eH,65H,78H,69H,74H,5fH,74H,03H
                DB      41H,08H,04H,20H,00H,11H,0dH,12H
                DB      00H,12H,5fH,5fH,76H,61H,5fH,6cH
                DB      69H,73H,74H,03H,10H,03H,0aH,12H
                DB      00H,14H,66H,70H,6fH,73H,5fH,74H
                DB      0aH,12H,00H,14H,66H,70H,6fH,73H
                DB      5fH,74H,08H,12H,00H,07H,75H,69H
                DB      6eH,74H,0aH,12H,00H,05H,75H,69H
                DB      6eH,74H,5fH,38H,0bH,12H,00H,07H
                DB      75H,69H,6eH,74H,5fH,31H,36H,03H
                DB      10H,13H,0bH,12H,00H,1aH,75H,69H
                DB      6eH,74H,5fH,33H,32H,0eH,12H,00H
                DB      05H,75H,6eH,73H,69H,67H,6eH,65H
                DB      64H,5fH,38H,0fH,12H,00H,07H,75H
                DB      6eH,73H,69H,67H,6eH,65H,64H,5fH
                DB      31H,36H,0fH,12H,00H,1aH,75H,6eH
                DB      73H,69H,67H,6eH,65H,64H,5fH,33H
                DB      32H,09H,12H,00H,04H,69H,6eH,74H
                DB      5fH,38H,0aH,12H,00H,06H,69H,6eH
                DB      74H,5fH,31H,36H,0aH,12H,00H,14H
                DB      69H,6eH,74H,5fH,33H,32H,0cH,12H
                DB      00H,04H,73H,69H,67H,6eH,65H,64H
                DB      5fH,38H,0dH,12H,00H,06H,73H,69H
                DB      67H,6eH,65H,64H,5fH,31H,36H,0dH
                DB      12H,00H,14H,73H,69H,67H,6eH,65H
                DB      64H,5fH,33H,32H,08H,12H,00H,08H
                DB      62H,6fH,6fH,6cH,0dH,12H,00H,06H
                DB      70H,74H,72H,64H,69H,66H,66H,5fH
                DB      74H,0fH,12H,00H,19H,6eH,61H,6dH
                DB      65H,5fH,68H,61H,6eH,64H,6cH,65H
                DB      0aH,12H,00H,05H,75H,69H,6eH,74H
                DB      5fH,38H,0eH,12H,00H,06H,64H,69H
                DB      72H,65H,63H,74H,5fH,69H,64H,78H
                DB      03H,41H,05H,08H,60H,08H,00H,12H
                DB      00H,00H,00H,0dH,61H,11H,05H,5fH
                DB      74H,6dH,70H,66H,63H,68H,61H,72H
                DB      0dH,61H,10H,05H,5fH,75H,6eH,67H
                DB      6fH,74H,74H,65H,6eH,0cH,61H,0eH
                DB      07H,5fH,62H,75H,66H,73H,69H,7aH
                DB      65H,0bH,61H,0cH,06H,5fH,68H,61H
                DB      6eH,64H,6cH,65H,09H,61H,0aH,07H
                DB      5fH,66H,6cH,61H,67H,09H,61H,06H
                DB      2aH,5fH,62H,61H,73H,65H,08H,61H
                DB      04H,06H,5fH,63H,6eH,74H,08H,61H
                DB      00H,2aH,5fH,70H,74H,72H,0bH,12H
                DB      01H,2bH,5fH,5fH,69H,6fH,62H,75H
                DB      66H,08H,12H,00H,2cH,46H,49H,4cH
                DB      45H,03H,41H,2dH,04H,20H,02H,2eH
                DB      04H,20H,02H,11H,08H,60H,02H,00H
                DB      18H,00H,00H,00H,09H,61H,0cH,30H
                DB      66H,6eH,61H,6dH,65H,08H,61H,00H
                DB      2fH,66H,69H,6cH,65H,0dH,12H,00H
                DB      31H,46H,69H,6cH,65H,5fH,49H,6eH
                DB      66H,6fH,04H,20H,00H,08H,08H,60H
                DB      06H,00H,09H,00H,00H,00H,0aH,61H
                DB      08H,33H,62H,75H,66H,66H,65H,72H
                DB      0cH,64H,07H,00H,01H,05H,69H,6eH
                DB      5fH,72H,65H,63H,0cH,61H,06H,18H
                DB      63H,68H,65H,63H,6bH,73H,75H,6dH
                DB      0aH,61H,04H,0aH,69H,6eH,5fH,62H
                DB      75H,66H,0aH,61H,02H,19H,6cH,65H
                DB      6eH,67H,74H,68H,06H,61H,00H,06H
                DB      66H,68H,0dH,12H,00H,34H,4fH,42H
                DB      4aH,5fH,57H,46H,49H,4cH,45H,03H
                DB      41H,35H,08H,60H,02H,00H,05H,00H
                DB      00H,00H,0cH,61H,01H,36H,66H,69H
                DB      6cH,65H,5fH,6fH,75H,74H,08H,61H
                DB      00H,18H,70H,61H,73H,73H,0eH,12H
                DB      00H,37H,70H,6fH,62H,6aH,5fH,73H
                DB      74H,61H,74H,65H,05H,50H,03H,00H
                DB      10H,10H,51H,02H,4eH,4fH,5fH,46H
                DB      50H,5fH,41H,4cH,4cH,4fH,57H,45H
                DB      44H,12H,51H,01H,4eH,4fH,5fH,46H
                DB      50H,5fH,45H,4dH,55H,4cH,41H,54H
                DB      49H,4fH,4eH,12H,51H,00H,44H,4fH
                DB      5fH,46H,50H,5fH,45H,4dH,55H,4cH
                DB      41H,54H,49H,4fH,4eH,07H,12H,03H
                DB      39H,66H,70H,65H,08H,60H,12H,00H
                DB      21H,00H,00H,00H,09H,61H,20H,08H
                DB      64H,65H,62H,75H,67H,0fH,61H,1cH
                DB      11H,6dH,6fH,64H,75H,6cH,65H,5fH
                DB      6eH,61H,6dH,65H,0cH,61H,18H,11H
                DB      74H,65H,78H,74H,5fH,73H,65H,67H
                DB      0cH,61H,14H,11H,64H,61H,74H,61H
                DB      5fH,73H,65H,67H,0eH,61H,10H,11H
                DB      63H,6fH,64H,65H,5fH,63H,6cH,61H
                DB      73H,73H,10H,61H,0cH,11H,62H,75H
                DB      69H,6cH,64H,5fH,74H,61H,72H,67H
                DB      65H,74H,11H,61H,0bH,08H,77H,61H
                DB      72H,6eH,69H,6eH,67H,5fH,65H,72H
                DB      72H,6fH,72H,11H,61H,0aH,08H,77H
                DB      61H,72H,6eH,69H,6eH,67H,5fH,6cH
                DB      65H,76H,65H,6cH,0fH,61H,09H,08H
                DB      65H,72H,72H,6fH,72H,5fH,6cH,69H
                DB      6dH,69H,74H,11H,61H,08H,08H,77H
                DB      61H,72H,6eH,69H,6eH,67H,5fH,63H
                DB      6fH,75H,6eH,74H,0fH,61H,07H,08H
                DB      65H,72H,72H,6fH,72H,5fH,63H,6fH
                DB      75H,6eH,74H,12H,61H,06H,39H,66H
                DB      6cH,6fH,61H,74H,69H,6eH,67H,5fH
                DB      70H,6fH,69H,6eH,74H,15H,61H,05H
                DB      08H,6eH,61H,6dH,69H,6eH,67H,5fH
                DB      63H,6fH,6eH,76H,65H,6eH,74H,69H
                DB      6fH,6eH,0eH,61H,04H,08H,64H,65H
                DB      62H,75H,67H,5fH,66H,6cH,61H,67H
                DB      12H,61H,03H,08H,62H,61H,6eH,6eH
                DB      65H,72H,5fH,70H,72H,69H,6eH,74H
                DB      65H,64H,09H,61H,02H,08H,71H,75H
                DB      69H,65H,74H,0fH,61H,01H,08H,73H
                DB      74H,6fH,70H,5fH,61H,74H,5fH,65H
                DB      6eH,64H,0eH,61H,00H,1fH,73H,69H
                DB      67H,6eH,5fH,76H,61H,6cH,75H,65H
                DB      12H,12H,01H,3bH,67H,6cH,6fH,62H
                DB      61H,6cH,5fH,6fH,70H,74H,69H,6fH
                DB      6eH,73H,12H,12H,00H,3cH,67H,6cH
                DB      6fH,62H,61H,6cH,5fH,6fH,70H,74H
                DB      69H,6fH,6eH,73H,05H,70H,06H,01H
                DB      11H,05H,70H,0dH,01H,0dH,03H,41H
                DB      11H,06H,71H,0dH,02H,06H,40H,06H
                DB      71H,06H,02H,06H,40H,05H,71H,0dH
                DB      01H,0dH,04H,20H,04H,11H,06H,70H
                DB      0dH,02H,11H,06H,05H,21H,06H,01H
                DB      08H,05H,21H,06H,01H,08H,05H,21H
                DB      03H,01H,08H,04H,20H,4fH,08H,05H
                DB      70H,0dH,01H,11H,05H,50H,30H,02H
                DB      11H,0aH,52H,2fH,02H,54H,5fH,4eH
                DB      55H,4cH,4cH,09H,52H,2eH,02H,54H
                DB      5fH,58H,4fH,52H,0bH,52H,2dH,02H
                DB      54H,5fH,58H,4cH,41H,54H,42H,0aH
                DB      52H,2cH,02H,54H,5fH,58H,4cH,41H
                DB      54H,0aH,52H,2bH,02H,54H,5fH,58H
                DB      43H,48H,47H,0aH,52H,2aH,02H,54H
                DB      5fH,58H,41H,44H,44H,0bH,52H,29H
                DB      02H,54H,5fH,57H,52H,4dH,53H,52H
                DB      0aH,52H,28H,02H,54H,5fH,57H,4fH
                DB      52H,44H,0bH,52H,27H,02H,54H,5fH
                DB      57H,49H,44H,54H,48H,0cH,52H,26H
                DB      02H,54H,5fH,57H,42H,49H,4eH,56H
                DB      44H,0eH,52H,25H,02H,54H,5fH,57H
                DB      41H,54H,43H,4fH,4dH,5fH,43H,0aH
                DB      52H,24H,02H,54H,5fH,57H,41H,49H
                DB      54H,0aH,52H,23H,02H,54H,5fH,56H
                DB      45H,52H,57H,0aH,52H,22H,02H,54H
                DB      5fH,56H,45H,52H,52H,0cH,52H,21H
                DB      02H,54H,5fH,56H,41H,52H,41H,52H
                DB      47H,0aH,52H,20H,02H,54H,5fH,55H
                DB      53H,45H,53H,0bH,52H,1fH,02H,54H
                DB      5fH,55H,53H,45H,33H,32H,0bH,52H
                DB      1eH,02H,54H,5fH,55H,53H,45H,31H
                DB      36H,0bH,52H,1dH,02H,54H,5fH,55H
                DB      4eH,49H,4fH,4eH,0dH,52H,1cH,02H
                DB      54H,5fH,54H,59H,50H,45H,44H,45H
                DB      46H,09H,52H,1bH,02H,54H,5fH,54H
                DB      52H,37H,09H,52H,1aH,02H,54H,5fH
                DB      54H,52H,36H,09H,52H,19H,02H,54H
                DB      5fH,54H,52H,35H,09H,52H,18H,02H
                DB      54H,5fH,54H,52H,34H,09H,52H,17H
                DB      02H,54H,5fH,54H,52H,33H,0bH,52H
                DB      16H,02H,54H,5fH,54H,49H,54H,4cH
                DB      45H,0aH,52H,15H,02H,54H,5fH,54H
                DB      49H,4eH,59H,0aH,52H,14H,02H,54H
                DB      5fH,54H,48H,49H,53H,0dH,52H,13H
                DB      02H,54H,5fH,54H,45H,58H,54H,45H
                DB      51H,55H,0aH,52H,12H,02H,54H,5fH
                DB      54H,45H,53H,54H,0bH,52H,11H,02H
                DB      54H,5fH,54H,42H,59H,54H,45H,0dH
                DB      52H,10H,02H,54H,5fH,53H,59H,53H
                DB      43H,41H,4cH,4cH,0bH,52H,0fH,02H
                DB      54H,5fH,53H,57H,4fH,52H,44H,0cH
                DB      52H,0eH,02H,54H,5fH,53H,55H,42H
                DB      54H,54H,4cH,0eH,52H,0dH,02H,54H
                DB      5fH,53H,55H,42H,54H,49H,54H,4cH
                DB      45H,09H,52H,0cH,02H,54H,5fH,53H
                DB      55H,42H,0cH,52H,0bH,02H,54H,5fH
                DB      53H,54H,52H,55H,43H,54H,0bH,52H
                DB      0aH,02H,54H,5fH,53H,54H,52H,55H
                DB      43H,09H,52H,09H,02H,54H,5fH,53H
                DB      54H,52H,0bH,52H,08H,02H,54H,5fH
                DB      53H,54H,4fH,53H,57H,0bH,52H,07H
                DB      02H,54H,5fH,53H,54H,4fH,53H,44H
                DB      0bH,52H,06H,02H,54H,5fH,53H,54H
                DB      4fH,53H,42H,0aH,52H,05H,02H,54H
                DB      5fH,53H,54H,4fH,53H,09H,52H,04H
                DB      02H,54H,5fH,53H,54H,49H,0dH,52H
                DB      03H,02H,54H,5fH,53H,54H,44H,43H
                DB      41H,4cH,4cH,09H,52H,02H,02H,54H
                DB      5fH,53H,54H,44H,09H,52H,01H,02H
                DB      54H,5fH,53H,54H,43H,08H,52H,00H
                DB      02H,54H,5fH,53H,54H,08H,52H,0ffH
                DB      01H,54H,5fH,53H,53H,08H,52H,0feH
                DB      01H,54H,5fH,53H,50H,0aH,52H,0fdH
                DB      01H,54H,5fH,53H,4dH,53H,57H,0bH
                DB      52H,0fcH,01H,54H,5fH,53H,4dH,41H
                DB      4cH,4cH,0aH,52H,0fbH,01H,54H,5fH
                DB      53H,4cH,44H,54H,0cH,52H,0faH,01H
                DB      54H,5fH,53H,49H,5aH,45H,4fH,46H
                DB      0aH,52H,0f9H,01H,54H,5fH,53H,49H
                DB      5aH,45H,0aH,52H,0f8H,01H,54H,5fH
                DB      53H,49H,44H,54H,08H,52H,0f7H,01H
                DB      54H,5fH,53H,49H,0aH,52H,0f6H,01H
                DB      54H,5fH,53H,48H,52H,44H,09H,52H
                DB      0f5H,01H,54H,5fH,53H,48H,52H,0bH
                DB      52H,0f4H,01H,54H,5fH,53H,48H,4fH
                DB      52H,54H,0aH,52H,0f3H,01H,54H,5fH
                DB      53H,48H,4cH,44H,09H,52H,0f2H,01H
                DB      54H,5fH,53H,48H,4cH,0aH,52H,0f1H
                DB      01H,54H,5fH,53H,47H,44H,54H,0aH
                DB      52H,0f0H,01H,54H,5fH,53H,45H,54H
                DB      5aH,0aH,52H,0efH,01H,54H,5fH,53H
                DB      45H,54H,53H,0bH,52H,0eeH,01H,54H
                DB      5fH,53H,45H,54H,50H,4fH,0bH,52H
                DB      0edH,01H,54H,5fH,53H,45H,54H,50H
                DB      45H,0aH,52H,0ecH,01H,54H,5fH,53H
                DB      45H,54H,50H,0aH,52H,0ebH,01H,54H
                DB      5fH,53H,45H,54H,4fH,0bH,52H,0eaH
                DB      01H,54H,5fH,53H,45H,54H,4eH,5aH
                DB      0bH,52H,0e9H,01H,54H,5fH,53H,45H
                DB      54H,4eH,53H,0bH,52H,0e8H,01H,54H
                DB      5fH,53H,45H,54H,4eH,50H,0bH,52H
                DB      0e7H,01H,54H,5fH,53H,45H,54H,4eH
                DB      4fH,0cH,52H,0e6H,01H,54H,5fH,53H
                DB      45H,54H,4eH,4cH,45H,0bH,52H,0e5H
                DB      01H,54H,5fH,53H,45H,54H,4eH,4cH
                DB      0cH,52H,0e4H,01H,54H,5fH,53H,45H
                DB      54H,4eH,47H,45H,0bH,52H,0e3H,01H
                DB      54H,5fH,53H,45H,54H,4eH,47H,0bH
                DB      52H,0e2H,01H,54H,5fH,53H,45H,54H
                DB      4eH,45H,0bH,52H,0e1H,01H,54H,5fH
                DB      53H,45H,54H,4eH,43H,0cH,52H,0e0H
                DB      01H,54H,5fH,53H,45H,54H,4eH,42H
                DB      45H,0bH,52H,0dfH,01H,54H,5fH,53H
                DB      45H,54H,4eH,42H,0cH,52H,0deH,01H
                DB      54H,5fH,53H,45H,54H,4eH,41H,45H
                DB      0bH,52H,0ddH,01H,54H,5fH,53H,45H
                DB      54H,4eH,41H,0bH,52H,0dcH,01H,54H
                DB      5fH,53H,45H,54H,4cH,45H,0aH,52H
                DB      0dbH,01H,54H,5fH,53H,45H,54H,4cH
                DB      0bH,52H,0daH,01H,54H,5fH,53H,45H
                DB      54H,47H,45H,0aH,52H,0d9H,01H,54H
                DB      5fH,53H,45H,54H,47H,0aH,52H,0d8H
                DB      01H,54H,5fH,53H,45H,54H,45H,0aH
                DB      52H,0d7H,01H,54H,5fH,53H,45H,54H
                DB      43H,0bH,52H,0d6H,01H,54H,5fH,53H
                DB      45H,54H,42H,45H,0aH,52H,0d5H,01H
                DB      54H,5fH,53H,45H,54H,42H,0bH,52H
                DB      0d4H,01H,54H,5fH,53H,45H,54H,41H
                DB      45H,0aH,52H,0d3H,01H,54H,5fH,53H
                DB      45H,54H,41H,0dH,52H,0d2H,01H,54H
                DB      5fH,53H,45H,47H,4dH,45H,4eH,54H
                DB      0aH,52H,0d1H,01H,54H,5fH,53H,45H
                DB      47H,32H,0cH,52H,0d0H,01H,54H,5fH
                DB      53H,44H,57H,4fH,52H,44H,0bH,52H
                DB      0cfH,01H,54H,5fH,53H,43H,41H,53H
                DB      57H,0bH,52H,0ceH,01H,54H,5fH,53H
                DB      43H,41H,53H,44H,0bH,52H,0cdH,01H
                DB      54H,5fH,53H,43H,41H,53H,42H,0aH
                DB      52H,0ccH,01H,54H,5fH,53H,43H,41H
                DB      53H,0bH,52H,0cbH,01H,54H,5fH,53H
                DB      42H,59H,54H,45H,09H,52H,0caH,01H
                DB      54H,5fH,53H,42H,42H,09H,52H,0c9H
                DB      01H,54H,5fH,53H,41H,52H,09H,52H
                DB      0c8H,01H,54H,5fH,53H,41H,4cH,0aH
                DB      52H,0c7H,01H,54H,5fH,53H,41H,48H
                DB      46H,09H,52H,0c6H,01H,54H,5fH,52H
                DB      53H,4dH,09H,52H,0c5H,01H,54H,5fH
                DB      52H,4fH,52H,09H,52H,0c4H,01H,54H
                DB      5fH,52H,4fH,4cH,0aH,52H,0c3H,01H
                DB      54H,5fH,52H,45H,54H,4eH,0aH,52H
                DB      0c2H,01H,54H,5fH,52H,45H,54H,46H
                DB      09H,52H,0c1H,01H,54H,5fH,52H,45H
                DB      54H,0aH,52H,0c0H,01H,54H,5fH,52H
                DB      45H,50H,5aH,0bH,52H,0bfH,01H,54H
                DB      5fH,52H,45H,50H,4eH,5aH,0bH,52H
                DB      0beH,01H,54H,5fH,52H,45H,50H,4eH
                DB      45H,0aH,52H,0bdH,01H,54H,5fH,52H
                DB      45H,50H,45H,09H,52H,0bcH,01H,54H
                DB      5fH,52H,45H,50H,0cH,52H,0bbH,01H
                DB      54H,5fH,52H,45H,43H,4fH,52H,44H
                DB      0eH,52H,0baH,01H,54H,5fH,52H,45H
                DB      41H,44H,4fH,4eH,4cH,59H,0bH,52H
                DB      0b9H,01H,54H,5fH,52H,44H,54H,53H
                DB      43H,0bH,52H,0b8H,01H,54H,5fH,52H
                DB      44H,4dH,53H,52H,09H,52H,0b7H,01H
                DB      54H,5fH,52H,43H,52H,09H,52H,0b6H
                DB      01H,54H,5fH,52H,43H,4cH,0bH,52H
                DB      0b5H,01H,54H,5fH,51H,57H,4fH,52H
                DB      44H,0bH,52H,0b4H,01H,54H,5fH,50H
                DB      57H,4fH,52H,44H,0cH,52H,0b3H,01H
                DB      54H,5fH,50H,55H,53H,48H,46H,44H
                DB      0bH,52H,0b2H,01H,54H,5fH,50H,55H
                DB      53H,48H,46H,11H,52H,0b1H,01H,54H
                DB      5fH,50H,55H,53H,48H,43H,4fH,4eH
                DB      54H,45H,58H,54H,0cH,52H,0b0H,01H
                DB      54H,5fH,50H,55H,53H,48H,41H,44H
                DB      0bH,52H,0afH,01H,54H,5fH,50H,55H
                DB      53H,48H,41H,0aH,52H,0aeH,01H,54H
                DB      5fH,50H,55H,53H,48H,0bH,52H,0adH
                DB      01H,54H,5fH,50H,55H,52H,47H,45H
                DB      0cH,52H,0acH,01H,54H,5fH,50H,55H
                DB      42H,4cH,49H,43H,09H,52H,0abH,01H
                DB      54H,5fH,50H,54H,52H,0bH,52H,0aaH
                DB      01H,54H,5fH,50H,52H,4fH,54H,4fH
                DB      0aH,52H,0a9H,01H,54H,5fH,50H,52H
                DB      4fH,43H,0dH,52H,0a8H,01H,54H,5fH
                DB      50H,52H,49H,56H,41H,54H,45H,0bH
                DB      52H,0a7H,01H,54H,5fH,50H,4fH,50H
                DB      46H,44H,0aH,52H,0a6H,01H,54H,5fH
                DB      50H,4fH,50H,46H,10H,52H,0a5H,01H
                DB      54H,5fH,50H,4fH,50H,43H,4fH,4eH
                DB      54H,45H,58H,54H,0bH,52H,0a4H,01H
                DB      54H,5fH,50H,4fH,50H,41H,44H,0aH
                DB      52H,0a3H,01H,54H,5fH,50H,4fH,50H
                DB      41H,09H,52H,0a2H,01H,54H,5fH,50H
                DB      4fH,50H,0cH,52H,0a1H,01H,54H,5fH
                DB      50H,41H,53H,43H,41H,4cH,0aH,52H
                DB      0a0H,01H,54H,5fH,50H,41H,52H,41H
                DB      0aH,52H,9fH,01H,54H,5fH,50H,41H
                DB      47H,45H,0bH,52H,9eH,01H,54H,5fH
                DB      4fH,55H,54H,53H,57H,0bH,52H,9dH
                DB      01H,54H,5fH,4fH,55H,54H,53H,44H
                DB      0bH,52H,9cH,01H,54H,5fH,4fH,55H
                DB      54H,53H,42H,0aH,52H,9bH,01H,54H
                DB      5fH,4fH,55H,54H,53H,09H,52H,9aH
                DB      01H,54H,5fH,4fH,55H,54H,0cH,52H
                DB      99H,01H,54H,5fH,4fH,53H,5fH,4fH
                DB      53H,32H,0cH,52H,98H,01H,54H,5fH
                DB      4fH,53H,5fH,44H,4fH,53H,09H,52H
                DB      97H,01H,54H,5fH,4fH,52H,47H,08H
                DB      52H,96H,01H,54H,5fH,4fH,52H,0cH
                DB      52H,95H,01H,54H,5fH,4fH,50H,54H
                DB      49H,4fH,4eH,0cH,52H,94H,01H,54H
                DB      5fH,4fH,50H,41H,54H,54H,52H,0cH
                DB      52H,93H,01H,54H,5fH,4fH,46H,46H
                DB      53H,45H,54H,0dH,52H,92H,01H,54H
                DB      5fH,4eH,4fH,54H,48H,49H,4eH,47H
                DB      09H,52H,91H,01H,54H,5fH,4eH,4fH
                DB      54H,09H,52H,90H,01H,54H,5fH,4eH
                DB      4fH,50H,0aH,52H,8fH,01H,54H,5fH
                DB      4eH,4fH,38H,37H,09H,52H,8eH,01H
                DB      54H,5fH,4eH,45H,47H,0fH,52H,8dH
                DB      01H,54H,5fH,4eH,45H,41H,52H,53H
                DB      54H,41H,43H,4bH,0aH,52H,8cH,01H
                DB      54H,5fH,4eH,45H,41H,52H,08H,52H
                DB      8bH,01H,54H,5fH,4eH,45H,0aH,52H
                DB      8aH,01H,54H,5fH,4eH,41H,4dH,45H
                DB      09H,52H,89H,01H,54H,5fH,4dH,55H
                DB      4cH,0bH,52H,88H,01H,54H,5fH,4dH
                DB      4fH,56H,5aH,58H,0bH,52H,87H,01H
                DB      54H,5fH,4dH,4fH,56H,53H,58H,0bH
                DB      52H,86H,01H,54H,5fH,4dH,4fH,56H
                DB      53H,57H,0bH,52H,85H,01H,54H,5fH
                DB      4dH,4fH,56H,53H,44H,0bH,52H,84H
                DB      01H,54H,5fH,4dH,4fH,56H,53H,42H
                DB      0aH,52H,83H,01H,54H,5fH,4dH,4fH
                DB      56H,53H,09H,52H,82H,01H,54H,5fH
                DB      4dH,4fH,56H,09H,52H,81H,01H,54H
                DB      5fH,4dH,4fH,44H,0cH,52H,80H,01H
                DB      54H,5fH,4dH,45H,4dH,4fH,52H,59H
                DB      0cH,52H,7fH,01H,54H,5fH,4dH,45H
                DB      44H,49H,55H,4dH,0aH,52H,7eH,01H
                DB      54H,5fH,4dH,41H,53H,4bH,0bH,52H
                DB      7dH,01H,54H,5fH,4dH,41H,43H,52H
                DB      4fH,09H,52H,7cH,01H,54H,5fH,4cH
                DB      54H,52H,08H,52H,7bH,01H,54H,5fH
                DB      4cH,54H,09H,52H,7aH,01H,54H,5fH
                DB      4cH,53H,53H,09H,52H,79H,01H,54H
                DB      5fH,4cH,53H,4cH,0eH,52H,78H,01H
                DB      54H,5fH,4cH,52H,4fH,46H,46H,53H
                DB      45H,54H,0dH,52H,77H,01H,54H,5fH
                DB      4cH,4fH,57H,57H,4fH,52H,44H,09H
                DB      52H,76H,01H,54H,5fH,4cH,4fH,57H
                DB      0bH,52H,75H,01H,54H,5fH,4cH,4fH
                DB      4fH,50H,5aH,0cH,52H,74H,01H,54H
                DB      5fH,4cH,4fH,4fH,50H,4eH,5aH,0cH
                DB      52H,73H,01H,54H,5fH,4cH,4fH,4fH
                DB      50H,4eH,45H,0bH,52H,72H,01H,54H
                DB      5fH,4cH,4fH,4fH,50H,45H,0aH,52H
                DB      71H,01H,54H,5fH,4cH,4fH,4fH,50H
                DB      0bH,52H,70H,01H,54H,5fH,4cH,4fH
                DB      44H,53H,57H,0bH,52H,6fH,01H,54H
                DB      5fH,4cH,4fH,44H,53H,44H,0bH,52H
                DB      6eH,01H,54H,5fH,4cH,4fH,44H,53H
                DB      42H,0aH,52H,6dH,01H,54H,5fH,4cH
                DB      4fH,44H,53H,0aH,52H,6cH,01H,54H
                DB      5fH,4cH,4fH,43H,4bH,0bH,52H,6bH
                DB      01H,54H,5fH,4cH,4fH,43H,41H,4cH
                DB      0aH,52H,6aH,01H,54H,5fH,4cH,4dH
                DB      53H,57H,0aH,52H,69H,01H,54H,5fH
                DB      4cH,4cH,44H,54H,0aH,52H,68H,01H
                DB      54H,5fH,4cH,49H,44H,54H,09H,52H
                DB      67H,01H,54H,5fH,4cH,47H,53H,0aH
                DB      52H,66H,01H,54H,5fH,4cH,47H,44H
                DB      54H,09H,52H,65H,01H,54H,5fH,4cH
                DB      46H,53H,09H,52H,64H,01H,54H,5fH
                DB      4cH,45H,53H,0eH,52H,63H,01H,54H
                DB      5fH,4cH,45H,4eH,47H,54H,48H,4fH
                DB      46H,0cH,52H,62H,01H,54H,5fH,4cH
                DB      45H,4eH,47H,54H,48H,0bH,52H,61H
                DB      01H,54H,5fH,4cH,45H,41H,56H,45H
                DB      09H,52H,60H,01H,54H,5fH,4cH,45H
                DB      41H,08H,52H,5fH,01H,54H,5fH,4cH
                DB      45H,09H,52H,5eH,01H,54H,5fH,4cH
                DB      44H,53H,0bH,52H,5dH,01H,54H,5fH
                DB      4cH,41H,52H,47H,45H,09H,52H,5cH
                DB      01H,54H,5fH,4cH,41H,52H,0aH,52H
                DB      5bH,01H,54H,5fH,4cH,41H,48H,46H
                DB      0bH,52H,5aH,01H,54H,5fH,4cH,41H
                DB      42H,45H,4cH,08H,52H,59H,01H,54H
                DB      5fH,4aH,5aH,08H,52H,58H,01H,54H
                DB      5fH,4aH,53H,09H,52H,57H,01H,54H
                DB      5fH,4aH,50H,4fH,09H,52H,56H,01H
                DB      54H,5fH,4aH,50H,45H,08H,52H,55H
                DB      01H,54H,5fH,4aH,50H,08H,52H,54H
                DB      01H,54H,5fH,4aH,4fH,09H,52H,53H
                DB      01H,54H,5fH,4aH,4eH,5aH,09H,52H
                DB      52H,01H,54H,5fH,4aH,4eH,53H,09H
                DB      52H,51H,01H,54H,5fH,4aH,4eH,50H
                DB      09H,52H,50H,01H,54H,5fH,4aH,4eH
                DB      4fH,0aH,52H,4fH,01H,54H,5fH,4aH
                DB      4eH,4cH,45H,09H,52H,4eH,01H,54H
                DB      5fH,4aH,4eH,4cH,0aH,52H,4dH,01H
                DB      54H,5fH,4aH,4eH,47H,45H,09H,52H
                DB      4cH,01H,54H,5fH,4aH,4eH,47H,09H
                DB      52H,4bH,01H,54H,5fH,4aH,4eH,45H
                DB      09H,52H,4aH,01H,54H,5fH,4aH,4eH
                DB      43H,0aH,52H,49H,01H,54H,5fH,4aH
                DB      4eH,42H,45H,09H,52H,48H,01H,54H
                DB      5fH,4aH,4eH,42H,0aH,52H,47H,01H
                DB      54H,5fH,4aH,4eH,41H,45H,09H,52H
                DB      46H,01H,54H,5fH,4aH,4eH,41H,0aH
                DB      52H,45H,01H,54H,5fH,4aH,4dH,50H
                DB      46H,09H,52H,44H,01H,54H,5fH,4aH
                DB      4dH,50H,09H,52H,43H,01H,54H,5fH
                DB      4aH,4cH,45H,08H,52H,42H,01H,54H
                DB      5fH,4aH,4cH,09H,52H,41H,01H,54H
                DB      5fH,4aH,47H,45H,08H,52H,40H,01H
                DB      54H,5fH,4aH,47H,0bH,52H,3fH,01H
                DB      54H,5fH,4aH,45H,43H,58H,5aH,08H
                DB      52H,3eH,01H,54H,5fH,4aH,45H,0aH
                DB      52H,3dH,01H,54H,5fH,4aH,43H,58H
                DB      5aH,08H,52H,3cH,01H,54H,5fH,4aH
                DB      43H,09H,52H,3bH,01H,54H,5fH,4aH
                DB      42H,45H,08H,52H,3aH,01H,54H,5fH
                DB      4aH,42H,09H,52H,39H,01H,54H,5fH
                DB      4aH,41H,45H,08H,52H,38H,01H,54H
                DB      5fH,4aH,41H,09H,52H,37H,01H,54H
                DB      5fH,49H,52H,50H,0bH,52H,36H,01H
                DB      54H,5fH,49H,52H,45H,54H,44H,0aH
                DB      52H,35H,01H,54H,5fH,49H,52H,45H
                DB      54H,0cH,52H,34H,01H,54H,5fH,49H
                DB      4eH,56H,4fH,4bH,45H,0cH,52H,33H
                DB      01H,54H,5fH,49H,4eH,56H,4cH,50H
                DB      47H,0aH,52H,32H,01H,54H,5fH,49H
                DB      4eH,56H,44H,0aH,52H,31H,01H,54H
                DB      5fH,49H,4eH,54H,4fH,09H,52H,30H
                DB      01H,54H,5fH,49H,4eH,54H,0aH,52H
                DB      2fH,01H,54H,5fH,49H,4eH,53H,57H
                DB      0aH,52H,2eH,01H,54H,5fH,49H,4eH
                DB      53H,44H,0aH,52H,2dH,01H,54H,5fH
                DB      49H,4eH,53H,42H,0aH,52H,2cH,01H
                DB      54H,5fH,49H,4eH,53H,32H,10H,52H
                DB      2bH,01H,54H,5fH,49H,4eH,43H,4cH
                DB      55H,44H,45H,4cH,49H,42H,0dH,52H
                DB      2aH,01H,54H,5fH,49H,4eH,43H,4cH
                DB      55H,44H,45H,09H,52H,29H,01H,54H
                DB      5fH,49H,4eH,43H,08H,52H,28H,01H
                DB      54H,5fH,49H,4eH,0aH,52H,27H,01H
                DB      54H,5fH,49H,4dH,55H,4cH,0cH,52H
                DB      26H,01H,54H,5fH,49H,47H,4eH,4fH
                DB      52H,45H,0cH,52H,25H,01H,54H,5fH
                DB      49H,46H,4eH,44H,45H,46H,0aH,52H
                DB      24H,01H,54H,5fH,49H,46H,4eH,42H
                DB      0cH,52H,23H,01H,54H,5fH,49H,46H
                DB      49H,44H,4eH,49H,0bH,52H,22H,01H
                DB      54H,5fH,49H,46H,49H,44H,4eH,09H
                DB      52H,21H,01H,54H,5fH,49H,46H,45H
                DB      0cH,52H,20H,01H,54H,5fH,49H,46H
                DB      44H,49H,46H,49H,0bH,52H,1fH,01H
                DB      54H,5fH,49H,46H,44H,49H,46H,0bH
                DB      52H,1eH,01H,54H,5fH,49H,46H,44H
                DB      45H,46H,09H,52H,1dH,01H,54H,5fH
                DB      49H,46H,42H,08H,52H,1cH,01H,54H
                DB      5fH,49H,46H,0aH,52H,1bH,01H,54H
                DB      5fH,49H,44H,49H,56H,0aH,52H,1aH
                DB      01H,54H,5fH,48H,55H,47H,45H,09H
                DB      52H,19H,01H,54H,5fH,48H,4cH,54H
                DB      0eH,52H,18H,01H,54H,5fH,48H,49H
                DB      47H,48H,57H,4fH,52H,44H,0aH,52H
                DB      17H,01H,54H,5fH,48H,49H,47H,48H
                DB      08H,52H,16H,01H,54H,5fH,47H,54H
                DB      08H,52H,15H,01H,54H,5fH,47H,53H
                DB      0bH,52H,14H,01H,54H,5fH,47H,52H
                DB      4fH,55H,50H,0cH,52H,13H,01H,54H
                DB      5fH,47H,4cH,4fH,42H,41H,4cH,08H
                DB      52H,12H,01H,54H,5fH,47H,45H,0dH
                DB      52H,11H,01H,54H,5fH,46H,59H,4cH
                DB      32H,58H,50H,31H,0bH,52H,10H,01H
                DB      54H,5fH,46H,59H,4cH,32H,58H,0dH
                DB      52H,0fH,01H,54H,5fH,46H,58H,54H
                DB      52H,41H,43H,54H,0aH,52H,0eH,01H
                DB      54H,5fH,46H,58H,43H,48H,0aH,52H
                DB      0dH,01H,54H,5fH,46H,58H,41H,4dH
                DB      0bH,52H,0cH,01H,54H,5fH,46H,57H
                DB      4fH,52H,44H,0bH,52H,0bH,01H,54H
                DB      5fH,46H,57H,41H,49H,54H,0dH,52H
                DB      0aH,01H,54H,5fH,46H,55H,43H,4fH
                DB      4dH,50H,50H,0cH,52H,09H,01H,54H
                DB      5fH,46H,55H,43H,4fH,4dH,50H,0bH
                DB      52H,08H,01H,54H,5fH,46H,55H,43H
                DB      4fH,4dH,0aH,52H,07H,01H,54H,5fH
                DB      46H,54H,53H,54H,0cH,52H,06H,01H
                DB      54H,5fH,46H,53H,55H,42H,52H,50H
                DB      0bH,52H,05H,01H,54H,5fH,46H,53H
                DB      55H,42H,52H,0bH,52H,04H,01H,54H
                DB      5fH,46H,53H,55H,42H,50H,0aH,52H
                DB      03H,01H,54H,5fH,46H,53H,55H,42H
                DB      0bH,52H,02H,01H,54H,5fH,46H,53H
                DB      54H,53H,57H,0aH,52H,01H,01H,54H
                DB      5fH,46H,53H,54H,50H,0dH,52H,00H
                DB      01H,54H,5fH,46H,53H,54H,45H,4eH
                DB      56H,57H,0dH,52H,0ffH,00H,54H,5fH
                DB      46H,53H,54H,45H,4eH,56H,44H,0cH
                DB      52H,0feH,00H,54H,5fH,46H,53H,54H
                DB      45H,4eH,56H,0bH,52H,0fdH,00H,54H
                DB      5fH,46H,53H,54H,43H,57H,09H,52H
                DB      0fcH,00H,54H,5fH,46H,53H,54H,0bH
                DB      52H,0fbH,00H,54H,5fH,46H,53H,51H
                DB      52H,54H,0dH,52H,0faH,00H,54H,5fH
                DB      46H,53H,49H,4eH,43H,4fH,53H,0aH
                DB      52H,0f9H,00H,54H,5fH,46H,53H,49H
                DB      4eH,0cH,52H,0f8H,00H,54H,5fH,46H
                DB      53H,45H,54H,50H,4dH,0cH,52H,0f7H
                DB      00H,54H,5fH,46H,53H,43H,41H,4cH
                DB      45H,0cH,52H,0f6H,00H,54H,5fH,46H
                DB      53H,41H,56H,45H,57H,0cH,52H,0f5H
                DB      00H,54H,5fH,46H,53H,41H,56H,45H
                DB      44H,0bH,52H,0f4H,00H,54H,5fH,46H
                DB      53H,41H,56H,45H,08H,52H,0f3H,00H
                DB      54H,5fH,46H,53H,0dH,52H,0f2H,00H
                DB      54H,5fH,46H,52H,53H,54H,4fH,52H
                DB      57H,0dH,52H,0f1H,00H,54H,5fH,46H
                DB      52H,53H,54H,4fH,52H,44H,0cH,52H
                DB      0f0H,00H,54H,5fH,46H,52H,53H,54H
                DB      4fH,52H,0dH,52H,0efH,00H,54H,5fH
                DB      46H,52H,4eH,44H,49H,4eH,54H,0bH
                DB      52H,0eeH,00H,54H,5fH,46H,50H,54H
                DB      41H,4eH,0cH,52H,0edH,00H,54H,5fH
                DB      46H,50H,52H,45H,4dH,31H,0bH,52H
                DB      0ecH,00H,54H,5fH,46H,50H,52H,45H
                DB      4dH,0cH,52H,0ebH,00H,54H,5fH,46H
                DB      50H,41H,54H,41H,4eH,0dH,52H,0eaH
                DB      00H,54H,5fH,46H,4fH,52H,54H,52H
                DB      41H,4eH,0aH,52H,0e9H,00H,54H,5fH
                DB      46H,4fH,52H,43H,09H,52H,0e8H,00H
                DB      54H,5fH,46H,4fH,52H,0cH,52H,0e7H
                DB      00H,54H,5fH,46H,4eH,53H,54H,53H
                DB      57H,0eH,52H,0e6H,00H,54H,5fH,46H
                DB      4eH,53H,54H,45H,4eH,56H,57H,0eH
                DB      52H,0e5H,00H,54H,5fH,46H,4eH,53H
                DB      54H,45H,4eH,56H,44H,0dH,52H,0e4H
                DB      00H,54H,5fH,46H,4eH,53H,54H,45H
                DB      4eH,56H,0cH,52H,0e3H,00H,54H,5fH
                DB      46H,4eH,53H,54H,43H,57H,0dH,52H
                DB      0e2H,00H,54H,5fH,46H,4eH,53H,41H
                DB      56H,45H,57H,0dH,52H,0e1H,00H,54H
                DB      5fH,46H,4eH,53H,41H,56H,45H,44H
                DB      0cH,52H,0e0H,00H,54H,5fH,46H,4eH
                DB      53H,41H,56H,45H,0aH,52H,0dfH,00H
                DB      54H,5fH,46H,4eH,4fH,50H,0cH,52H
                DB      0deH,00H,54H,5fH,46H,4eH,49H,4eH
                DB      49H,54H,0bH,52H,0ddH,00H,54H,5fH
                DB      46H,4eH,45H,4eH,49H,0cH,52H,0dcH
                DB      00H,54H,5fH,46H,4eH,44H,49H,53H
                DB      49H,0cH,52H,0dbH,00H,54H,5fH,46H
                DB      4eH,43H,4cH,45H,58H,0bH,52H,0daH
                DB      00H,54H,5fH,46H,4dH,55H,4cH,50H
                DB      0aH,52H,0d9H,00H,54H,5fH,46H,4dH
                DB      55H,4cH,0aH,52H,0d8H,00H,54H,5fH
                DB      46H,4cH,44H,5aH,0bH,52H,0d7H,00H
                DB      54H,5fH,46H,4cH,44H,50H,49H,0cH
                DB      52H,0d6H,00H,54H,5fH,46H,4cH,44H
                DB      4cH,4eH,32H,0cH,52H,0d5H,00H,54H
                DB      5fH,46H,4cH,44H,4cH,47H,32H,0cH
                DB      52H,0d4H,00H,54H,5fH,46H,4cH,44H
                DB      4cH,32H,54H,0cH,52H,0d3H,00H,54H
                DB      5fH,46H,4cH,44H,4cH,32H,45H,0dH
                DB      52H,0d2H,00H,54H,5fH,46H,4cH,44H
                DB      45H,4eH,56H,57H,0dH,52H,0d1H,00H
                DB      54H,5fH,46H,4cH,44H,45H,4eH,56H
                DB      44H,0cH,52H,0d0H,00H,54H,5fH,46H
                DB      4cH,44H,45H,4eH,56H,0bH,52H,0cfH
                DB      00H,54H,5fH,46H,4cH,44H,43H,57H
                DB      0aH,52H,0ceH,00H,54H,5fH,46H,4cH
                DB      44H,31H,09H,52H,0cdH,00H,54H,5fH
                DB      46H,4cH,44H,0aH,52H,0ccH,00H,54H
                DB      5fH,46H,4cH,41H,54H,0cH,52H,0cbH
                DB      00H,54H,5fH,46H,49H,53H,55H,42H
                DB      52H,0bH,52H,0caH,00H,54H,5fH,46H
                DB      49H,53H,55H,42H,0bH,52H,0c9H,00H
                DB      54H,5fH,46H,49H,53H,54H,50H,0aH
                DB      52H,0c8H,00H,54H,5fH,46H,49H,53H
                DB      54H,0bH,52H,0c7H,00H,54H,5fH,46H
                DB      49H,4eH,49H,54H,0dH,52H,0c6H,00H
                DB      54H,5fH,46H,49H,4eH,43H,53H,54H
                DB      50H,0bH,52H,0c5H,00H,54H,5fH,46H
                DB      49H,4dH,55H,4cH,0aH,52H,0c4H,00H
                DB      54H,5fH,46H,49H,4cH,44H,0cH,52H
                DB      0c3H,00H,54H,5fH,46H,49H,44H,49H
                DB      56H,52H,0bH,52H,0c2H,00H,54H,5fH
                DB      46H,49H,44H,49H,56H,0cH,52H,0c1H
                DB      00H,54H,5fH,46H,49H,43H,4fH,4dH
                DB      50H,0bH,52H,0c0H,00H,54H,5fH,46H
                DB      49H,43H,4fH,4dH,0bH,52H,0bfH,00H
                DB      54H,5fH,46H,49H,41H,44H,44H,0bH
                DB      52H,0beH,00H,54H,5fH,46H,46H,52H
                DB      45H,45H,0aH,52H,0bdH,00H,54H,5fH
                DB      46H,45H,4eH,49H,0cH,52H,0bcH,00H
                DB      54H,5fH,46H,44H,49H,56H,52H,50H
                DB      0bH,52H,0bbH,00H,54H,5fH,46H,44H
                DB      49H,56H,52H,0bH,52H,0baH,00H,54H
                DB      5fH,46H,44H,49H,56H,50H,0aH,52H
                DB      0b9H,00H,54H,5fH,46H,44H,49H,56H
                DB      0bH,52H,0b8H,00H,54H,5fH,46H,44H
                DB      49H,53H,49H,0dH,52H,0b7H,00H,54H
                DB      5fH,46H,44H,45H,43H,53H,54H,50H
                DB      0aH,52H,0b6H,00H,54H,5fH,46H,43H
                DB      4fH,53H,0cH,52H,0b5H,00H,54H,5fH
                DB      46H,43H,4fH,4dH,50H,50H,0bH,52H
                DB      0b4H,00H,54H,5fH,46H,43H,4fH,4dH
                DB      50H,0aH,52H,0b3H,00H,54H,5fH,46H
                DB      43H,4fH,4dH,0bH,52H,0b2H,00H,54H
                DB      5fH,46H,43H,4cH,45H,58H,0aH,52H
                DB      0b1H,00H,54H,5fH,46H,43H,48H,53H
                DB      0bH,52H,0b0H,00H,54H,5fH,46H,42H
                DB      53H,54H,50H,0aH,52H,0afH,00H,54H
                DB      5fH,46H,42H,4cH,44H,0eH,52H,0aeH
                DB      00H,54H,5fH,46H,41H,52H,53H,54H
                DB      41H,43H,4bH,09H,52H,0adH,00H,54H
                DB      5fH,46H,41H,52H,0bH,52H,0acH,00H
                DB      54H,5fH,46H,41H,44H,44H,50H,0aH
                DB      52H,0abH,00H,54H,5fH,46H,41H,44H
                DB      44H,0aH,52H,0aaH,00H,54H,5fH,46H
                DB      41H,42H,53H,0bH,52H,0a9H,00H,54H
                DB      5fH,46H,32H,58H,4dH,31H,0bH,52H
                DB      0a8H,00H,54H,5fH,45H,58H,54H,52H
                DB      4eH,0fH,52H,0a7H,00H,54H,5fH,45H
                DB      58H,54H,45H,52H,4eH,44H,45H,46H
                DB      0cH,52H,0a6H,00H,54H,5fH,45H,58H
                DB      54H,45H,52H,4eH,0cH,52H,0a5H,00H
                DB      54H,5fH,45H,58H,50H,4fH,52H,54H
                DB      0aH,52H,0a4H,00H,54H,5fH,45H,56H
                DB      45H,4eH,09H,52H,0a3H,00H,54H,5fH
                DB      45H,53H,50H,09H,52H,0a2H,00H,54H
                DB      5fH,45H,53H,49H,08H,52H,0a1H,00H
                DB      54H,5fH,45H,53H,0bH,52H,0a0H,00H
                DB      54H,5fH,45H,52H,52H,4fH,52H,0aH
                DB      52H,9fH,00H,54H,5fH,45H,51H,55H
                DB      32H,09H,52H,9eH,00H,54H,5fH,45H
                DB      51H,55H,08H,52H,9dH,00H,54H,5fH
                DB      45H,51H,0bH,52H,9cH,00H,54H,5fH
                DB      45H,4eH,54H,45H,52H,0aH,52H,9bH
                DB      00H,54H,5fH,45H,4eH,44H,53H,0aH
                DB      52H,9aH,00H,54H,5fH,45H,4eH,44H
                DB      50H,0bH,52H,99H,00H,54H,5fH,45H
                DB      4eH,44H,49H,46H,09H,52H,98H,00H
                DB      54H,5fH,45H,4eH,44H,0cH,52H,97H
                DB      00H,54H,5fH,45H,4cH,53H,45H,49H
                DB      46H,0aH,52H,96H,00H,54H,5fH,45H
                DB      4cH,53H,45H,09H,52H,95H,00H,54H
                DB      5fH,45H,44H,58H,09H,52H,94H,00H
                DB      54H,5fH,45H,44H,49H,09H,52H,93H
                DB      00H,54H,5fH,45H,43H,58H,0aH,52H
                DB      92H,00H,54H,5fH,45H,43H,48H,4fH
                DB      09H,52H,91H,00H,54H,5fH,45H,42H
                DB      58H,09H,52H,90H,00H,54H,5fH,45H
                DB      42H,50H,09H,52H,8fH,00H,54H,5fH
                DB      45H,41H,58H,08H,52H,8eH,00H,54H
                DB      5fH,44H,58H,0bH,52H,8dH,00H,54H
                DB      5fH,44H,57H,4fH,52H,44H,08H,52H
                DB      8cH,00H,54H,5fH,44H,57H,09H,52H
                DB      8bH,00H,54H,5fH,44H,55H,50H,08H
                DB      52H,8aH,00H,54H,5fH,44H,54H,08H
                DB      52H,89H,00H,54H,5fH,44H,53H,09H
                DB      52H,88H,00H,54H,5fH,44H,52H,37H
                DB      09H,52H,87H,00H,54H,5fH,44H,52H
                DB      36H,09H,52H,86H,00H,54H,5fH,44H
                DB      52H,33H,09H,52H,85H,00H,54H,5fH
                DB      44H,52H,32H,09H,52H,84H,00H,54H
                DB      5fH,44H,52H,31H,09H,52H,83H,00H
                DB      54H,5fH,44H,52H,30H,08H,52H,82H
                DB      00H,54H,5fH,44H,51H,08H,52H,81H
                DB      00H,54H,5fH,44H,50H,08H,52H,80H
                DB      00H,54H,5fH,44H,4cH,08H,51H,7fH
                DB      54H,5fH,44H,49H,56H,07H,51H,7eH
                DB      54H,5fH,44H,49H,07H,51H,7dH,54H
                DB      5fH,44H,48H,07H,51H,7cH,54H,5fH
                DB      44H,46H,08H,51H,7bH,54H,5fH,44H
                DB      45H,43H,07H,51H,7aH,54H,5fH,44H
                DB      44H,07H,51H,79H,54H,5fH,44H,42H
                DB      08H,51H,78H,54H,5fH,44H,41H,53H
                DB      08H,51H,77H,54H,5fH,44H,41H,41H
                DB      07H,51H,76H,54H,5fH,43H,58H,09H
                DB      51H,75H,54H,5fH,43H,57H,44H,45H
                DB      08H,51H,74H,54H,5fH,43H,57H,44H
                DB      07H,51H,73H,54H,5fH,43H,53H,08H
                DB      51H,72H,54H,5fH,43H,52H,34H,08H
                DB      51H,71H,54H,5fH,43H,52H,33H,08H
                DB      51H,70H,54H,5fH,43H,52H,32H,08H
                DB      51H,6fH,54H,5fH,43H,52H,30H,0aH
                DB      51H,6eH,54H,5fH,43H,50H,55H,49H
                DB      44H,0cH,51H,6dH,54H,5fH,43H,4fH
                DB      4dH,50H,41H,43H,54H,0bH,51H,6cH
                DB      54H,5fH,43H,4fH,4dH,4dH,4fH,4eH
                DB      0cH,51H,6bH,54H,5fH,43H,4fH,4dH
                DB      4dH,45H,4eH,54H,09H,51H,6aH,54H
                DB      5fH,43H,4fH,4dH,4dH,0eH,51H,69H
                DB      54H,5fH,43H,4dH,50H,58H,43H,48H
                DB      47H,38H,42H,0cH,51H,68H,54H,5fH
                DB      43H,4dH,50H,58H,43H,48H,47H,0aH
                DB      51H,67H,54H,5fH,43H,4dH,50H,53H
                DB      57H,0aH,51H,66H,54H,5fH,43H,4dH
                DB      50H,53H,44H,0aH,51H,65H,54H,5fH
                DB      43H,4dH,50H,53H,42H,09H,51H,64H
                DB      54H,5fH,43H,4dH,50H,53H,08H,51H
                DB      63H,54H,5fH,43H,4dH,50H,08H,51H
                DB      62H,54H,5fH,43H,4dH,43H,09H,51H
                DB      61H,54H,5fH,43H,4cH,54H,53H,08H
                DB      51H,60H,54H,5fH,43H,4cH,49H,08H
                DB      51H,5fH,54H,5fH,43H,4cH,44H,08H
                DB      51H,5eH,54H,5fH,43H,4cH,43H,07H
                DB      51H,5dH,54H,5fH,43H,4cH,07H,51H
                DB      5cH,54H,5fH,43H,48H,08H,51H,5bH
                DB      54H,5fH,43H,44H,51H,08H,51H,5aH
                DB      54H,5fH,43H,42H,57H,0bH,51H,59H
                DB      54H,5fH,43H,41H,54H,53H,54H,52H
                DB      0cH,51H,58H,54H,5fH,43H,41H,53H
                DB      45H,4dH,41H,50H,0aH,51H,57H,54H
                DB      5fH,43H,41H,4cH,4cH,46H,09H,51H
                DB      56H,54H,5fH,43H,41H,4cH,4cH,06H
                DB      51H,55H,54H,5fH,43H,09H,51H,54H
                DB      54H,5fH,42H,59H,54H,45H,07H,51H
                DB      53H,54H,5fH,42H,58H,08H,51H,52H
                DB      54H,5fH,42H,54H,53H,08H,51H,51H
                DB      54H,5fH,42H,54H,52H,08H,51H,50H
                DB      54H,5fH,42H,54H,43H,07H,51H,4fH
                DB      54H,5fH,42H,54H,0aH,51H,4eH,54H
                DB      5fH,42H,53H,57H,41H,50H,08H,51H
                DB      4dH,54H,5fH,42H,53H,52H,08H,51H
                DB      4cH,54H,5fH,42H,53H,46H,07H,51H
                DB      4bH,54H,5fH,42H,50H,0aH,51H,4aH
                DB      54H,5fH,42H,4fH,55H,4eH,44H,07H
                DB      51H,49H,54H,5fH,42H,4cH,07H,51H
                DB      48H,54H,5fH,42H,48H,0aH,51H,47H
                DB      54H,5fH,42H,41H,53H,49H,43H,07H
                DB      51H,46H,54H,5fH,41H,58H,07H,51H
                DB      45H,54H,5fH,41H,54H,0bH,51H,44H
                DB      54H,5fH,41H,53H,53H,55H,4dH,45H
                DB      09H,51H,43H,54H,5fH,41H,52H,50H
                DB      4cH,08H,51H,42H,54H,5fH,41H,4eH
                DB      44H,0aH,51H,41H,54H,5fH,41H,4cH
                DB      49H,47H,4eH,0aH,51H,40H,54H,5fH
                DB      41H,4cH,49H,41H,53H,07H,51H,3fH
                DB      54H,5fH,41H,4cH,07H,51H,3eH,54H
                DB      5fH,41H,48H,09H,51H,3dH,54H,5fH
                DB      41H,44H,44H,52H,08H,51H,3cH,54H
                DB      5fH,41H,44H,44H,08H,51H,3bH,54H
                DB      5fH,41H,44H,43H,09H,51H,3aH,54H
                DB      5fH,41H,42H,53H,32H,08H,51H,39H
                DB      54H,5fH,41H,41H,53H,08H,51H,38H
                DB      54H,5fH,41H,41H,4dH,08H,51H,37H
                DB      54H,5fH,41H,41H,44H,08H,51H,36H
                DB      54H,5fH,41H,41H,41H,0aH,51H,35H
                DB      54H,5fH,57H,48H,49H,4cH,45H,0aH
                DB      51H,34H,54H,5fH,55H,4eH,54H,49H
                DB      4cH,0bH,51H,33H,54H,5fH,54H,46H
                DB      43H,4fH,4eH,44H,0cH,51H,32H,54H
                DB      5fH,53H,54H,41H,52H,54H,55H,50H
                DB      0aH,51H,31H,54H,5fH,53H,54H,41H
                DB      43H,4bH,08H,51H,30H,54H,5fH,53H
                DB      45H,51H,0bH,51H,2fH,54H,5fH,52H
                DB      45H,50H,45H,41H,54H,0aH,51H,2eH
                DB      54H,5fH,52H,41H,44H,49H,58H,0bH
                DB      51H,2dH,54H,5fH,4eH,4fH,4cH,49H
                DB      53H,54H,0bH,51H,2cH,54H,5fH,4eH
                DB      4fH,43H,52H,45H,46H,0aH,51H,2bH
                DB      54H,5fH,4dH,4fH,44H,45H,4cH,11H
                DB      51H,2aH,54H,5fH,4cH,49H,53H,54H
                DB      4dH,41H,43H,52H,4fH,41H,4cH,4cH
                DB      0eH,51H,29H,54H,5fH,4cH,49H,53H
                DB      54H,4dH,41H,43H,52H,4fH,0bH,51H
                DB      28H,54H,5fH,4cH,49H,53H,54H,49H
                DB      46H,0cH,51H,27H,54H,5fH,4cH,49H
                DB      53H,54H,41H,4cH,4cH,09H,51H,26H
                DB      54H,5fH,4cH,49H,53H,54H,0fH,51H
                DB      25H,54H,5fH,46H,41H,52H,44H,41H
                DB      54H,41H,5fH,55H,4eH,0cH,51H,24H
                DB      54H,5fH,46H,41H,52H,44H,41H,54H
                DB      41H,09H,51H,23H,54H,5fH,45H,58H
                DB      49H,54H,0aH,51H,22H,54H,5fH,45H
                DB      52H,52H,4eH,5aH,0cH,51H,21H,54H
                DB      5fH,45H,52H,52H,4eH,44H,45H,46H
                DB      0aH,51H,20H,54H,5fH,45H,52H,52H
                DB      4eH,42H,0cH,51H,1fH,54H,5fH,45H
                DB      52H,52H,49H,44H,4eH,49H,0bH,51H
                DB      1eH,54H,5fH,45H,52H,52H,49H,44H
                DB      4eH,09H,51H,1dH,54H,5fH,45H,52H
                DB      52H,45H,0cH,51H,1cH,54H,5fH,45H
                DB      52H,52H,44H,49H,46H,49H,0bH,51H
                DB      1bH,54H,5fH,45H,52H,52H,44H,49H
                DB      46H,0bH,51H,1aH,54H,5fH,45H,52H
                DB      52H,44H,45H,46H,09H,51H,19H,54H
                DB      5fH,45H,52H,52H,42H,08H,51H,18H
                DB      54H,5fH,45H,52H,52H,09H,51H,17H
                DB      54H,5fH,45H,4eH,44H,57H,0bH,51H
                DB      16H,54H,5fH,44H,4fH,53H,53H,45H
                DB      47H,0cH,51H,15H,54H,5fH,44H,41H
                DB      54H,41H,5fH,55H,4eH,09H,51H,14H
                DB      54H,5fH,44H,41H,54H,41H,09H,51H
                DB      13H,54H,5fH,43H,52H,45H,46H,0dH
                DB      51H,12H,54H,5fH,43H,4fH,4eH,54H
                DB      49H,4eH,55H,45H,0aH,51H,11H,54H
                DB      5fH,43H,4fH,4eH,53H,54H,09H,51H
                DB      10H,54H,5fH,43H,4fH,44H,45H,0aH
                DB      51H,0fH,54H,5fH,42H,52H,45H,41H
                DB      4bH,0aH,51H,0eH,54H,5fH,41H,4cH
                DB      50H,48H,41H,09H,51H,0dH,54H,5fH
                DB      38H,30H,38H,37H,09H,51H,0cH,54H
                DB      5fH,38H,30H,38H,36H,09H,51H,0bH
                DB      54H,5fH,35H,38H,36H,50H,08H,51H
                DB      0aH,54H,5fH,35H,38H,36H,09H,51H
                DB      09H,54H,5fH,34H,38H,36H,50H,08H
                DB      51H,08H,54H,5fH,34H,38H,36H,08H
                DB      51H,07H,54H,5fH,33H,38H,37H,09H
                DB      51H,06H,54H,5fH,33H,38H,36H,50H
                DB      08H,51H,05H,54H,5fH,33H,38H,36H
                DB      08H,51H,04H,54H,5fH,32H,38H,37H
                DB      09H,51H,03H,54H,5fH,32H,38H,36H
                DB      50H,09H,51H,02H,54H,5fH,32H,38H
                DB      36H,43H,08H,51H,01H,54H,5fH,32H
                DB      38H,36H,08H,51H,00H,54H,5fH,31H
                DB      38H,36H,0dH,12H,03H,4bH,61H,73H
                DB      6dH,5fH,74H,6fH,6bH,65H,6eH,05H
                DB      70H,06H,01H,0dH,05H,70H,0dH,01H
                DB      08H,04H,20H,13H,08H,06H,70H,0dH
                DB      02H,08H,11H,06H,70H,0dH,02H,06H
                DB      40H,04H,20H,4fH,08H,05H,50H,0fH
                DB      00H,10H,0dH,51H,0eH,53H,59H,4dH
                DB      5fH,53H,54H,52H,55H,43H,54H,13H
                DB      51H,0dH,53H,59H,4dH,5fH,53H,54H
                DB      52H,55H,43H,54H,5fH,46H,49H,45H
                DB      4cH,44H,12H,51H,0cH,53H,59H,4dH
                DB      5fH,43H,4cH,41H,53H,53H,5fH,4cH
                DB      4eH,41H,4dH,45H,0cH,51H,0bH,53H
                DB      59H,4dH,5fH,4cH,4eH,41H,4dH,45H
                DB      0aH,51H,0aH,53H,59H,4dH,5fH,45H
                DB      58H,54H,0aH,51H,09H,53H,59H,4dH
                DB      5fH,4cH,49H,42H,0cH,51H,08H,53H
                DB      59H,4dH,5fH,43H,4fH,4eH,53H,54H
                DB      0cH,51H,07H,53H,59H,4dH,5fH,4dH
                DB      41H,43H,52H,4fH,0bH,51H,06H,53H
                DB      59H,4dH,5fH,50H,52H,4fH,43H,0aH
                DB      51H,05H,53H,59H,4dH,5fH,47H,52H
                DB      50H,0aH,51H,04H,53H,59H,4dH,5fH
                DB      53H,45H,47H,0cH,51H,03H,53H,59H
                DB      4dH,5fH,53H,54H,41H,43H,4bH,0fH
                DB      51H,02H,53H,59H,4dH,5fH,45H,58H
                DB      54H,45H,52H,4eH,41H,4cH,0fH,51H
                DB      01H,53H,59H,4dH,5fH,49H,4eH,54H
                DB      45H,52H,4eH,41H,4cH,10H,51H,00H
                DB      53H,59H,4dH,5fH,55H,4eH,44H,45H
                DB      46H,49H,4eH,45H,44H,0dH,12H,03H
                DB      53H,73H,79H,6dH,5fH,73H,74H,61H
                DB      74H,65H,05H,71H,53H,01H,11H,05H
                DB      50H,0bH,00H,10H,0bH,51H,0aH,53H
                DB      59H,4dH,5fH,46H,41H,52H,34H,0bH
                DB      51H,09H,53H,59H,4dH,5fH,46H,41H
                DB      52H,32H,0cH,51H,08H,53H,59H,4dH
                DB      5fH,4eH,45H,41H,52H,34H,0cH,51H
                DB      07H,53H,59H,4dH,5fH,4eH,45H,41H
                DB      52H,32H,0eH,51H,06H,53H,59H,4dH
                DB      5fH,46H,4cH,4fH,41H,54H,31H,30H
                DB      0dH,51H,05H,53H,59H,4dH,5fH,46H
                DB      4cH,4fH,41H,54H,38H,0dH,51H,04H
                DB      53H,59H,4dH,5fH,46H,4cH,4fH,41H
                DB      54H,34H,0bH,51H,03H,53H,59H,4dH
                DB      5fH,49H,4eH,54H,36H,0bH,51H,02H
                DB      53H,59H,4dH,5fH,49H,4eH,54H,34H
                DB      0bH,51H,01H,53H,59H,4dH,5fH,49H
                DB      4eH,54H,32H,0bH,51H,00H,53H,59H
                DB      4dH,5fH,49H,4eH,54H,31H,0cH,12H
                DB      03H,56H,73H,79H,6dH,5fH,74H,79H
                DB      70H,65H,05H,71H,56H,01H,11H
$$TYPES         ENDS

                END
