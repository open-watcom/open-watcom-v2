.386p
                NAME    main
                EXTRN   __argc :BYTE
                EXTRN   _cstart_ :BYTE
                EXTRN   MsgPrintf1_ :BYTE
                EXTRN   InputQueueLine_ :BYTE
                EXTRN   AsmError_ :BYTE
                EXTRN   strcat_ :BYTE
                EXTRN   strchr_ :BYTE
                EXTRN   strncpy_ :BYTE
                EXTRN   stackavail_ :BYTE
                EXTRN   strcspn_ :BYTE
                EXTRN   AddStringToIncludePath_ :BYTE
                EXTRN   PushLineQueue_ :BYTE
                EXTRN   StoreConstant_ :BYTE
                EXTRN   AsmScan_ :BYTE
                EXTRN   _AsmBuffer :BYTE
                EXTRN   getenv_ :BYTE
                EXTRN   strcpy_ :BYTE
                EXTRN   AsmAlloc_ :BYTE
                EXTRN   strlen_ :BYTE
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
                EXTRN   WriteObjModule_ :BYTE
                EXTRN   AsmInit_ :BYTE
                EXTRN   exit_ :BYTE
                EXTRN   MsgInit_ :BYTE
                EXTRN   tolower_ :BYTE
                EXTRN   __IsTable :BYTE
                EXTRN   __CHK :BYTE
DGROUP          GROUP   CONST,CONST2,_DATA,_BSS
_TEXT           SEGMENT BYTE PUBLIC USE32 'CODE'
                ASSUME  CS:_TEXT ,DS:DGROUP,SS:DGROUP
                PUBLIC  main_
                PUBLIC  trademark_
                PUBLIC  ProcessCmdlineOptions_
                PUBLIC  AsmQueryExternal_
                PUBLIC  AsmQueryType_
isvalidident_:  push    ebp
                mov     ebp,esp
                push    0000002cH
                call    near ptr __CHK
                push    ebx
                push    ecx
                push    edx
                push    esi
                push    edi
                sub     esp,00000010H
                mov     dword ptr [ebp]-24H,eax
                mov     eax,dword ptr [ebp]-24H
                mov     al,byte ptr [eax]
                inc     al
                and     eax,000000ffH
                test    byte ptr __IsTable[eax],20H
                je      short L1
                mov     dword ptr [ebp]-20H,0ffffffffH
                jmp     near ptr L19
L1:             mov     eax,dword ptr [ebp]-24H
                mov     dword ptr [ebp]-1cH,eax
L2:             mov     eax,dword ptr [ebp]-1cH
                cmp     byte ptr [eax],00H
                jne     short L4
                jmp     near ptr L18
L3:             mov     eax,dword ptr [ebp]-1cH
                inc     dword ptr [ebp]-1cH
                jmp     short L2
L4:             mov     eax,dword ptr [ebp]-1cH
                mov     al,byte ptr [eax]
                and     eax,000000ffH
                call    near ptr tolower_
                mov     byte ptr [ebp]-18H,al
                cmp     byte ptr [ebp]-18H,5fH
                je      short L5
                cmp     byte ptr [ebp]-18H,2eH
                jne     short L6
L5:             jmp     short L7
L6:             cmp     byte ptr [ebp]-18H,24H
                jne     short L8
L7:             jmp     short L9
L8:             cmp     byte ptr [ebp]-18H,40H
                jne     short L10
L9:             jmp     short L11
L10:            cmp     byte ptr [ebp]-18H,3fH
                jne     short L12
L11:            jmp     short L13
L12:            mov     al,byte ptr [ebp]-18H
                inc     al
                and     eax,000000ffH
                test    byte ptr __IsTable[eax],20H
                je      short L14
L13:            jmp     short L15
L14:            mov     al,byte ptr [ebp]-18H
                inc     al
                and     eax,000000ffH
                test    byte ptr __IsTable[eax],80H
                je      short L16
L15:            jmp     short L17
L16:            mov     dword ptr [ebp]-20H,0ffffffffH
                jmp     short L19
L17:            jmp     short L3
L18:            mov     dword ptr [ebp]-20H,00000001H
L19:            mov     eax,dword ptr [ebp]-20H
                lea     esp,[ebp]-14H
                pop     edi
                pop     esi
                pop     edx
                pop     ecx
                pop     ebx
                pop     ebp
                ret
main_:          push    ebp
                mov     ebp,esp
                push    00000024H
                call    near ptr __CHK
                push    ebx
                push    ecx
                push    esi
                push    edi
                sub     esp,0000000cH
                mov     dword ptr [ebp]-1cH,eax
                mov     dword ptr [ebp]-18H,edx
                call    near ptr main_init_
                mov     eax,dword ptr [ebp]-18H
                mov     eax,dword ptr [eax]
                call    near ptr MsgInit_
                test    eax,eax
                je      short L20
                mov     eax,00000001H
                jmp     near ptr exit_
L20:            mov     eax,offset DGROUP:L153
                call    near ptr put_cmdline_const_in_list_
                mov     edx,dword ptr [ebp]-18H
                mov     eax,dword ptr [ebp]-1cH
                call    near ptr parse_cmdline_
                cmp     byte ptr _Quiet,00H
                jne     short L21
                cmp     byte ptr _BannerPrinted,00H
                je      short L22
L21:            jmp     short L23
L22:            mov     byte ptr _BannerPrinted,01H
                call    near ptr trademark_
L23:            mov     ebx,0ffffffffH
                mov     edx,0ffffffffH
                mov     eax,0ffffffffH
                call    near ptr AsmInit_
                call    near ptr open_files_
                call    near ptr WriteObjModule_
                call    near ptr MsgFini_
                call    near ptr main_fini_
                xor     eax,eax
                mov     al,byte ptr _ErrorFlag
                mov     dword ptr [ebp]-14H,eax
                mov     eax,dword ptr [ebp]-14H
                lea     esp,[ebp]-10H
                pop     edi
                pop     esi
                pop     ecx
                pop     ebx
                pop     ebp
                ret
usage_msg_:     push    ebp
                mov     ebp,esp
                push    0000001cH
                call    near ptr __CHK
                push    ebx
                push    ecx
                push    edx
                push    esi
                push    edi
                sub     esp,00000000H
                mov     edx,000007f0H
                mov     eax,000007d0H
                call    near ptr PrintfUsage_
                mov     eax,00000001H
                jmp     near ptr exit_
trademark_:     push    ebp
                mov     ebp,esp
                push    00000028H
                call    near ptr __CHK
                push    ebx
                push    ecx
                push    edx
                push    esi
                push    edi
                sub     esp,00000004H
                mov     dword ptr [ebp]-18H,00000000H
                cmp     byte ptr _Quiet,00H
                jne     short L25
L24:            mov     eax,dword ptr [ebp]-18H
                shl     eax,02H
                cmp     dword ptr L187[eax],00000000H
                je      short L25
                mov     eax,dword ptr [ebp]-18H
                inc     dword ptr [ebp]-18H
                shl     eax,02H
                push    dword ptr L187[eax]
                mov     eax,offset DGROUP:L158
                push    eax
                call    near ptr printf_
                add     esp,00000008H
                jmp     short L24
L25:            lea     esp,[ebp]-14H
                pop     edi
                pop     esi
                pop     edx
                pop     ecx
                pop     ebx
                pop     ebp
                ret
free_file_:     push    ebp
                mov     ebp,esp
                push    0000001cH
                call    near ptr __CHK
                push    ebx
                push    ecx
                push    edx
                push    esi
                push    edi
                sub     esp,00000000H
                mov     eax,dword ptr _AsmFiles+0cH
                call    near ptr AsmFree_
                mov     eax,dword ptr _AsmFiles+10H
                call    near ptr AsmFree_
                mov     eax,dword ptr _AsmFiles+14H
                call    near ptr AsmFree_
                lea     esp,[ebp]-14H
                pop     edi
                pop     esi
                pop     edx
                pop     ecx
                pop     ebx
                pop     ebp
                ret
main_init_:     push    ebp
                mov     ebp,esp
                push    00000020H
                call    near ptr __CHK
                push    ebx
                push    ecx
                push    edx
                push    esi
                push    edi
                sub     esp,00000004H
                call    near ptr MemInit_
                mov     dword ptr [ebp]-18H,00000000H
L26:            cmp     dword ptr [ebp]-18H,00000002H
                jle     short L28
                jmp     short L29
L27:            mov     eax,dword ptr [ebp]-18H
                inc     dword ptr [ebp]-18H
                jmp     short L26
L28:            mov     eax,dword ptr [ebp]-18H
                shl     eax,02H
                mov     dword ptr _AsmFiles[eax],00000000H
                mov     eax,dword ptr [ebp]-18H
                shl     eax,02H
                mov     dword ptr _AsmFiles[eax]+0cH,00000000H
                jmp     short L27
L29:            call    near ptr ObjRecInit_
                call    near ptr GenMSOmfInit_
                lea     esp,[ebp]-14H
                pop     edi
                pop     esi
                pop     edx
                pop     ecx
                pop     ebx
                pop     ebp
                ret
main_fini_:     push    ebp
                mov     ebp,esp
                push    0000001cH
                call    near ptr __CHK
                push    ebx
                push    ecx
                push    edx
                push    esi
                push    edi
                sub     esp,00000000H
                call    near ptr free_file_
                call    near ptr GenMSOmfFini_
                call    near ptr AsmShutDown_
                call    near ptr MemFini_
                lea     esp,[ebp]-14H
                pop     edi
                pop     esi
                pop     edx
                pop     ecx
                pop     ebx
                pop     ebp
                ret
open_files_:    push    ebp
                mov     ebp,esp
                push    00000024H
                call    near ptr __CHK
                push    ebx
                push    ecx
                push    edx
                push    esi
                push    edi
                sub     esp,00000000H
                mov     edx,offset DGROUP:L159
                mov     eax,dword ptr _AsmFiles+0cH
                call    near ptr fopen_
                mov     dword ptr _AsmFiles,eax
                cmp     dword ptr _AsmFiles,00000000H
                jne     short L30
                push    dword ptr _AsmFiles+0cH
                push    00000001H
                call    near ptr Fatal_
                add     esp,00000008H
L30:            mov     eax,dword ptr _AsmFiles+14H
                call    near ptr ObjWriteOpen_
                mov     dword ptr _pobjState+1H,eax
                cmp     dword ptr _pobjState+1H,00000000H
                jne     short L31
                push    dword ptr _AsmFiles+14H
                push    00000001H
                call    near ptr Fatal_
                add     esp,00000008H
L31:            mov     byte ptr _pobjState,02H
                call    near ptr DelErrFile_
                lea     esp,[ebp]-14H
                pop     edi
                pop     esi
                pop     edx
                pop     ecx
                pop     ebx
                pop     ebp
                ret
get_fname_:     push    ebp
                mov     ebp,esp
                push    000001b0H
                call    near ptr __CHK
                push    ebx
                push    ecx
                push    esi
                push    edi
                sub     esp,00000190H
                mov     dword ptr [ebp]-24H,eax
                mov     dword ptr [ebp]-28H,edx
                cmp     dword ptr [ebp]-28H,00000000H
                jne     near ptr L37
                cmp     dword ptr [ebp]-24H,00000000H
                jne     short L32
                lea     edx,[ebp]-78H
                mov     eax,0000043fH
                call    near ptr MsgGet_
                lea     eax,[ebp]-78H
                push    eax
                push    00000001H
                call    near ptr Fatal_
                add     esp,00000008H
L32:            cmp     dword ptr _AsmFiles+0cH,00000000H
                je      short L33
                push    00000005H
                call    near ptr Fatal_
                add     esp,00000004H
L33:            lea     eax,[ebp]-20H
                push    eax
                lea     eax,[ebp]-1cH
                push    eax
                lea     ecx,[ebp]-18H
                lea     ebx,[ebp]-14H
                lea     edx,[ebp]-19cH
                mov     eax,dword ptr [ebp]-24H
                call    near ptr _splitpath2_
                mov     eax,dword ptr [ebp]-20H
                cmp     byte ptr [eax],00H
                jne     short L34
                mov     dword ptr [ebp]-20H,offset DGROUP:L160
L34:            mov     eax,dword ptr [ebp]-20H
                push    eax
                mov     ecx,dword ptr [ebp]-1cH
                mov     ebx,dword ptr [ebp]-18H
                mov     edx,dword ptr [ebp]-14H
                lea     eax,[ebp]-108H
                call    near ptr _makepath_
                lea     eax,[ebp]-108H
                call    near ptr strlen_
                inc     eax
                call    near ptr AsmAlloc_
                mov     dword ptr _AsmFiles+0cH,eax
                lea     edx,[ebp]-108H
                mov     eax,dword ptr _AsmFiles+0cH
                call    near ptr strcpy_
                cmp     dword ptr _AsmFiles+14H,00000000H
                jne     short L35
                mov     dword ptr [ebp]-20H,offset DGROUP:L161
                mov     eax,dword ptr [ebp]-20H
                push    eax
                mov     ecx,dword ptr [ebp]-1cH
                xor     ebx,ebx
                xor     edx,edx
                lea     eax,[ebp]-108H
                call    near ptr _makepath_
                lea     eax,[ebp]-108H
                call    near ptr strlen_
                inc     eax
                call    near ptr AsmAlloc_
                mov     dword ptr _AsmFiles+14H,eax
                lea     edx,[ebp]-108H
                mov     eax,dword ptr _AsmFiles+14H
                call    near ptr strcpy_
L35:            cmp     dword ptr _AsmFiles+10H,00000000H
                jne     short L36
                mov     dword ptr [ebp]-20H,offset DGROUP:L162
                mov     eax,dword ptr [ebp]-20H
                push    eax
                mov     ecx,dword ptr [ebp]-1cH
                xor     ebx,ebx
                xor     edx,edx
                lea     eax,[ebp]-108H
                call    near ptr _makepath_
                lea     eax,[ebp]-108H
                call    near ptr strlen_
                inc     eax
                call    near ptr AsmAlloc_
                mov     dword ptr _AsmFiles+10H,eax
                lea     edx,[ebp]-108H
                mov     eax,dword ptr _AsmFiles+10H
                call    near ptr strcpy_
L36:            jmp     near ptr L44
L37:            lea     eax,[ebp]-20H
                push    eax
                lea     eax,[ebp]-1cH
                push    eax
                lea     ecx,[ebp]-18H
                lea     ebx,[ebp]-14H
                lea     edx,[ebp]-19cH
                mov     eax,dword ptr [ebp]-24H
                call    near ptr _splitpath2_
                mov     eax,dword ptr [ebp]-20H
                cmp     byte ptr [eax],00H
                jne     short L40
                cmp     dword ptr [ebp]-28H,00000001H
                jne     short L38
                mov     dword ptr [ebp]-1a0H,offset DGROUP:L162
                jmp     short L39
L38:            mov     dword ptr [ebp]-1a0H,offset DGROUP:L161
L39:            mov     eax,dword ptr [ebp]-1a0H
                mov     dword ptr [ebp]-20H,eax
                jmp     short L41
L40:            mov     edx,dword ptr [ebp]-20H
                lea     eax,[ebp]-78H
                call    near ptr strcpy_
                lea     eax,[ebp]-78H
                mov     dword ptr [ebp]-20H,eax
L41:            mov     eax,dword ptr [ebp]-1cH
                cmp     byte ptr [eax],00H
                je      short L42
                mov     eax,dword ptr [ebp]-1cH
                cmp     byte ptr [eax],2aH
                jne     short L43
L42:            push    00000000H
                lea     eax,[ebp]-1cH
                push    eax
                xor     ecx,ecx
                xor     ebx,ebx
                lea     edx,[ebp]-19cH
                mov     eax,dword ptr _AsmFiles+0cH
                call    near ptr _splitpath2_
                mov     dword ptr [ebp]-14H,offset DGROUP:L163
                mov     dword ptr [ebp]-18H,offset DGROUP:L163
L43:            mov     eax,dword ptr [ebp]-20H
                push    eax
                mov     ecx,dword ptr [ebp]-1cH
                mov     ebx,dword ptr [ebp]-18H
                mov     edx,dword ptr [ebp]-14H
                lea     eax,[ebp]-108H
                call    near ptr _makepath_
                lea     eax,[ebp]-108H
                call    near ptr strlen_
                inc     eax
                call    near ptr AsmAlloc_
                mov     edx,dword ptr [ebp]-28H
                shl     edx,02H
                mov     dword ptr _AsmFiles[edx]+0cH,eax
                lea     edx,[ebp]-108H
                mov     eax,dword ptr [ebp]-28H
                shl     eax,02H
                mov     eax,dword ptr _AsmFiles[eax]+0cH
                call    near ptr strcpy_
L44:            lea     esp,[ebp]-10H
                pop     edi
                pop     esi
                pop     ecx
                pop     ebx
                pop     ebp
                ret
do_envvar_cmdline_:
                push    ebp
                mov     ebp,esp
                push    0000002cH
                call    near ptr __CHK
                push    ebx
                push    ecx
                push    esi
                push    edi
                sub     esp,00000014H
                mov     dword ptr [ebp]-24H,eax
                mov     dword ptr [ebp]-20H,edx
                cmp     dword ptr [ebp]-20H,0000000aH
                jge     near ptr L56
                mov     eax,dword ptr [ebp]-24H
                call    near ptr getenv_
                mov     dword ptr [ebp]-14H,eax
                cmp     dword ptr [ebp]-14H,00000000H
                je      near ptr L56
                mov     eax,dword ptr [ebp]-14H
                mov     dword ptr [ebp]-18H,eax
                mov     eax,dword ptr [ebp]-14H
                mov     dword ptr [ebp]-1cH,eax
L45:            mov     eax,dword ptr [ebp]-18H
                cmp     byte ptr [eax],00H
                je      near ptr L56
L46:            mov     eax,dword ptr [ebp]-1cH
                cmp     byte ptr [eax],20H
                je      short L47
                mov     eax,dword ptr [ebp]-1cH
                mov     al,byte ptr [eax]
                and     eax,000000ffH
                cmp     eax,00002f74H
                jne     short L48
L47:            jmp     short L49
L48:            mov     eax,dword ptr [ebp]-1cH
                cmp     byte ptr [eax],00H
                jne     short L50
L49:            jmp     short L51
L50:            mov     eax,dword ptr [ebp]-1cH
                inc     dword ptr [ebp]-1cH
                jmp     short L46
L51:            mov     eax,dword ptr [ebp]-1cH
                cmp     byte ptr [eax],00H
                jne     short L52
                mov     edx,dword ptr [ebp]-20H
                inc     edx
                mov     eax,dword ptr [ebp]-18H
                call    near ptr parse_token_
                jmp     short L56
L52:            mov     eax,dword ptr [ebp]-1cH
                mov     byte ptr [eax],00H
                mov     eax,dword ptr [ebp]-1cH
                inc     dword ptr [ebp]-1cH
                mov     edx,dword ptr [ebp]-20H
                inc     edx
                mov     eax,dword ptr [ebp]-18H
                call    near ptr parse_token_
L53:            mov     eax,dword ptr [ebp]-1cH
                cmp     byte ptr [eax],20H
                je      short L54
                mov     eax,dword ptr [ebp]-1cH
                mov     al,byte ptr [eax]
                and     eax,000000ffH
                cmp     eax,00002f74H
                jne     short L55
L54:            mov     eax,dword ptr [ebp]-1cH
                inc     dword ptr [ebp]-1cH
                jmp     short L53
L55:            mov     eax,dword ptr [ebp]-1cH
                cmp     byte ptr [eax],00H
                je      short L56
                mov     eax,dword ptr [ebp]-1cH
                mov     dword ptr [ebp]-18H,eax
                jmp     near ptr L45
L56:            lea     esp,[ebp]-10H
                pop     edi
                pop     esi
                pop     ecx
                pop     ebx
                pop     ebp
                ret
AddConstant_:   push    ebp
                mov     ebp,esp
                push    000000f4H
                call    near ptr __CHK
                push    ebx
                push    ecx
                push    edx
                push    esi
                push    edi
                sub     esp,000000d8H
                mov     dword ptr [ebp]-1cH,eax
                mov     byte ptr [ebp]-18H,00H
L57:            cmp     byte ptr [ebp]-18H,05H
                jb      short L59
                jmp     short L60
L58:            mov     al,byte ptr [ebp]-18H
                inc     byte ptr [ebp]-18H
                jmp     short L57
L59:            xor     eax,eax
                mov     al,byte ptr [ebp]-18H
                lea     eax,[eax+eax*8]
                lea     edx,[ebp]-4cH
                add     edx,eax
                xor     eax,eax
                mov     al,byte ptr [ebp]-18H
                shl     eax,02H
                mov     dword ptr _AsmBuffer[eax],edx
                jmp     short L58
L60:            lea     edx,[ebp]-0ecH
                mov     eax,dword ptr [ebp]-1cH
                call    near ptr AsmScan_
                mov     edx,00000001H
                xor     eax,eax
                call    near ptr StoreConstant_
                lea     esp,[ebp]-14H
                pop     edi
                pop     esi
                pop     edx
                pop     ecx
                pop     ebx
                pop     ebp
                ret
ProcessCmdlineOptions_:
                push    ebp
                mov     ebp,esp
                push    00000028H
                call    near ptr __CHK
                push    ebx
                push    ecx
                push    edx
                push    esi
                push    edi
                sub     esp,0000000cH
                call    near ptr PushLineQueue_
                cmp     byte ptr _ProcessorType,00H
                je      short L61
                call    near ptr set_processor_type_
L61:            cmp     byte ptr _MemType,00H
                je      short L62
                call    near ptr set_mem_type_
L62:            mov     eax,dword ptr _CmdlineConstList
                mov     dword ptr [ebp]-18H,eax
L63:            cmp     dword ptr [ebp]-18H,00000000H
                jne     short L65
                jmp     short L66
L64:            mov     eax,dword ptr [ebp]-1cH
                mov     dword ptr [ebp]-18H,eax
                jmp     short L63
L65:            mov     eax,dword ptr [ebp]-18H
                mov     eax,dword ptr [eax]
                mov     dword ptr [ebp]-1cH,eax
                mov     eax,dword ptr [ebp]-18H
                mov     eax,dword ptr [eax]+4H
                call    near ptr AddConstant_
                mov     eax,dword ptr [ebp]-18H
                mov     eax,dword ptr [eax]+4H
                call    near ptr AsmFree_
                mov     eax,dword ptr [ebp]-18H
                call    near ptr AsmFree_
                jmp     short L64
L66:            mov     eax,offset DGROUP:L164
                call    near ptr getenv_
                mov     dword ptr [ebp]-20H,eax
                cmp     dword ptr [ebp]-20H,00000000H
                je      short L67
                mov     eax,dword ptr [ebp]-20H
                call    near ptr AddStringToIncludePath_
L67:            lea     esp,[ebp]-14H
                pop     edi
                pop     esi
                pop     edx
                pop     ecx
                pop     ebx
                pop     ebp
                ret
put_cmdline_const_in_list_:
                push    ebp
                mov     ebp,esp
                push    00000038H
                call    near ptr __CHK
                push    ebx
                push    ecx
                push    edx
                push    esi
                push    edi
                sub     esp,0000001cH
                mov     dword ptr [ebp]-28H,eax
                mov     edx,offset DGROUP:L165
                mov     eax,dword ptr [ebp]-28H
                call    near ptr strcspn_
                mov     ebx,eax
                mov     edx,offset DGROUP:L166
                mov     eax,dword ptr [ebp]-28H
                call    near ptr strcspn_
                cmp     ebx,eax
                jae     short L68
                mov     edx,offset DGROUP:L165
                mov     eax,dword ptr [ebp]-28H
                call    near ptr strcspn_
                mov     dword ptr [ebp]-30H,eax
                jmp     short L69
L68:            mov     edx,offset DGROUP:L166
                mov     eax,dword ptr [ebp]-28H
                call    near ptr strcspn_
                mov     dword ptr [ebp]-30H,eax
L69:            mov     eax,dword ptr [ebp]-30H
                mov     dword ptr [ebp]-1cH,eax
                mov     eax,dword ptr [ebp]-1cH
                add     eax,00000004H
                and     al,0fcH
                mov     edx,eax
                call    near ptr stackavail_
                cmp     edx,eax
                jae     short L70
                mov     eax,dword ptr [ebp]-1cH
                add     eax,00000004H
                and     al,0fcH
                sub     esp,eax
                mov     eax,esp
                mov     dword ptr [ebp]-2cH,eax
                jmp     short L71
L70:            mov     dword ptr [ebp]-2cH,00000000H
L71:            mov     eax,dword ptr [ebp]-2cH
                mov     dword ptr [ebp]-20H,eax
                mov     ebx,dword ptr [ebp]-1cH
                mov     edx,dword ptr [ebp]-28H
                mov     eax,dword ptr [ebp]-20H
                call    near ptr strncpy_
                mov     eax,dword ptr [ebp]-20H
                add     eax,dword ptr [ebp]-1cH
                mov     byte ptr [eax],00H
                mov     eax,dword ptr [ebp]-20H
                call    near ptr isvalidident_
                cmp     eax,0ffffffffH
                jne     short L72
                mov     dword ptr [ebp]-24H,0ffffffffH
                jmp     near ptr L79
L72:            mov     dword ptr [ebp]-18H,offset DGROUP:_CmdlineConstList
L73:            mov     eax,dword ptr [ebp]-18H
                cmp     dword ptr [eax],00000000H
                jne     short L75
                jmp     short L76
L74:            mov     eax,dword ptr [ebp]-18H
                mov     eax,dword ptr [eax]
                mov     dword ptr [ebp]-18H,eax
                jmp     short L73
L75:            jmp     short L74
L76:            mov     eax,00000008H
                call    near ptr AsmAlloc_
                mov     edx,eax
                mov     eax,dword ptr [ebp]-18H
                mov     dword ptr [eax],edx
                mov     eax,dword ptr [ebp]-18H
                mov     eax,dword ptr [eax]
                mov     dword ptr [eax],00000000H
                mov     eax,dword ptr [ebp]-28H
                call    near ptr strlen_
                add     eax,00000003H
                call    near ptr AsmAlloc_
                mov     edx,eax
                mov     eax,dword ptr [ebp]-18H
                mov     eax,dword ptr [eax]
                mov     dword ptr [eax]+4H,edx
                mov     edx,dword ptr [ebp]-28H
                mov     eax,dword ptr [ebp]-18H
                mov     eax,dword ptr [eax]
                mov     eax,dword ptr [eax]+4H
                call    near ptr strcpy_
                mov     edx,0000003dH
                mov     eax,dword ptr [ebp]-28H
                call    near ptr strchr_
                test    eax,eax
                jne     short L78
                mov     edx,00000023H
                mov     eax,dword ptr [ebp]-18H
                mov     eax,dword ptr [eax]
                mov     eax,dword ptr [eax]+4H
                call    near ptr strchr_
                mov     dword ptr [ebp]-20H,eax
                cmp     dword ptr [ebp]-20H,00000000H
                jne     short L77
                mov     edx,offset DGROUP:L167
                mov     eax,dword ptr [ebp]-18H
                mov     eax,dword ptr [eax]
                mov     eax,dword ptr [eax]+4H
                call    near ptr strcat_
                jmp     short L78
L77:            mov     eax,dword ptr [ebp]-20H
                mov     byte ptr [eax],3dH
L78:            mov     dword ptr [ebp]-24H,00000001H
L79:            mov     eax,dword ptr [ebp]-24H
                lea     esp,[ebp]-14H
                pop     edi
                pop     esi
                pop     edx
                pop     ecx
                pop     ebx
                pop     ebp
                ret
set_processor_type_:
                push    ebp
                mov     ebp,esp
                push    0000002cH
                call    near ptr __CHK
                push    ebx
                push    ecx
                push    edx
                push    esi
                push    edi
                sub     esp,00000010H
                call    near ptr stackavail_
                cmp     eax,000000a0H
                jbe     short L80
                mov     eax,000000a0H
                sub     esp,eax
                mov     eax,esp
                mov     dword ptr [ebp]-20H,eax
                jmp     short L81
L80:            mov     dword ptr [ebp]-20H,00000000H
L81:            mov     eax,dword ptr [ebp]-20H
                mov     dword ptr [ebp]-18H,eax
                cmp     dword ptr [ebp]-18H,00000000H
                jne     short L83
                mov     eax,00000441H
                call    near ptr AsmError_
                mov     eax,0ffffffffH
                jmp     near ptr exit_
                nop
L82             DD      L84
                DD      L85
                DD      L86
                DD      L87
                DD      L88
                DD      L89
                DD      L91
                DD      L90
L83:            mov     al,byte ptr _ProcessorType
                sub     al,30H
                mov     byte ptr [ebp]-24H,al
                cmp     byte ptr [ebp]-24H,07H
                ja      short L91
                xor     eax,eax
                mov     al,byte ptr [ebp]-24H
                shl     eax,02H
                jmp     dword ptr cs:L82[eax]
L84:            mov     dword ptr [ebp]-1cH,offset DGROUP:L168
                jmp     short L91
L85:            mov     dword ptr [ebp]-1cH,offset DGROUP:L169
                jmp     short L91
L86:            mov     dword ptr [ebp]-1cH,offset DGROUP:L170
                jmp     short L91
L87:            mov     dword ptr [ebp]-1cH,offset DGROUP:L171
                jmp     short L91
L88:            mov     dword ptr [ebp]-1cH,offset DGROUP:L172
                jmp     short L91
L89:            mov     dword ptr [ebp]-1cH,offset DGROUP:L173
                jmp     short L91
L90:            jmp     short L92
L91:            mov     eax,dword ptr [ebp]-18H
                mov     byte ptr [eax],00H
                mov     edx,offset DGROUP:L174
                mov     eax,dword ptr [ebp]-18H
                call    near ptr strcat_
                mov     edx,dword ptr [ebp]-1cH
                mov     eax,dword ptr [ebp]-18H
                call    near ptr strcat_
                mov     eax,dword ptr [ebp]-18H
                call    near ptr InputQueueLine_
                mov     eax,dword ptr [ebp]-18H
                mov     byte ptr [eax],00H
                mov     edx,offset DGROUP:L175
                mov     eax,dword ptr [ebp]-18H
                call    near ptr strcat_
                mov     edx,dword ptr [ebp]-1cH
                mov     eax,dword ptr [ebp]-18H
                call    near ptr strcat_
                mov     edx,offset DGROUP:L176
                mov     eax,dword ptr [ebp]-18H
                call    near ptr strcat_
                mov     eax,dword ptr [ebp]-18H
                call    near ptr InputQueueLine_
L92:            lea     esp,[ebp]-14H
                pop     edi
                pop     esi
                pop     edx
                pop     ecx
                pop     ebx
                pop     ebp
                ret
set_mem_type_:  push    ebp
                mov     ebp,esp
                push    0000002cH
                call    near ptr __CHK
                push    ebx
                push    ecx
                push    edx
                push    esi
                push    edi
                sub     esp,00000010H
                call    near ptr stackavail_
                cmp     eax,000000a0H
                jbe     short L93
                mov     eax,000000a0H
                sub     esp,eax
                mov     eax,esp
                mov     dword ptr [ebp]-20H,eax
                jmp     short L94
L93:            mov     dword ptr [ebp]-20H,00000000H
L94:            mov     eax,dword ptr [ebp]-20H
                mov     dword ptr [ebp]-18H,eax
                cmp     dword ptr [ebp]-18H,00000000H
                jne     short L95
                mov     eax,00000441H
                call    near ptr AsmError_
                mov     eax,0ffffffffH
                jmp     near ptr exit_
L95:            mov     edi,dword ptr [ebp]-18H
                mov     esi,offset DGROUP:L177
                movsd
                movsd
                mov     al,byte ptr _MemType
                mov     byte ptr [ebp]-24H,al
                cmp     byte ptr [ebp]-24H,6cH
                jb      short L97
                cmp     byte ptr [ebp]-24H,6cH
                jbe     near ptr L101
                cmp     byte ptr [ebp]-24H,73H
                jb      short L96
                cmp     byte ptr [ebp]-24H,73H
                jbe     near ptr L103
                cmp     byte ptr [ebp]-24H,74H
                je      near ptr L104
                jmp     near ptr L105
L96:            cmp     byte ptr [ebp]-24H,6dH
                je      short L102
                jmp     near ptr L105
L97:            cmp     byte ptr [ebp]-24H,66H
                jb      short L98
                cmp     byte ptr [ebp]-24H,66H
                jbe     short L99
                cmp     byte ptr [ebp]-24H,68H
                je      short L100
                jmp     short L105
L98:            cmp     byte ptr [ebp]-24H,63H
                jne     short L105
                mov     dword ptr [ebp]-1cH,offset DGROUP:L178
                jmp     short L106
L99:            mov     dword ptr [ebp]-1cH,offset DGROUP:L179
                jmp     short L106
L100:           mov     dword ptr [ebp]-1cH,offset DGROUP:L180
                jmp     short L106
L101:           mov     dword ptr [ebp]-1cH,offset DGROUP:L181
                jmp     short L106
L102:           mov     dword ptr [ebp]-1cH,offset DGROUP:L182
                jmp     short L106
L103:           mov     dword ptr [ebp]-1cH,offset DGROUP:L183
                jmp     short L106
L104:           mov     dword ptr [ebp]-1cH,offset DGROUP:L184
                jmp     short L106
L105:           mov     edi,dword ptr [ebp]-18H
                mov     esi,offset DGROUP:L185
                movsw
                movsb
                mov     edx,offset DGROUP:_MemType
                mov     eax,dword ptr [ebp]-18H
                call    near ptr strcat_
                mov     edx,dword ptr [ebp]-18H
                mov     eax,0000044eH
                call    near ptr MsgPrintf1_
                mov     eax,00000001H
                jmp     near ptr exit_
L106:           mov     edx,dword ptr [ebp]-1cH
                mov     eax,dword ptr [ebp]-18H
                call    near ptr strcat_
                mov     eax,dword ptr [ebp]-18H
                call    near ptr InputQueueLine_
                mov     eax,dword ptr [ebp]-18H
                mov     byte ptr [eax],00H
                mov     edx,offset DGROUP:L175
                mov     eax,dword ptr [ebp]-18H
                call    near ptr strcat_
                mov     edx,dword ptr [ebp]-1cH
                mov     eax,dword ptr [ebp]-18H
                call    near ptr strcat_
                mov     edx,offset DGROUP:L176
                mov     eax,dword ptr [ebp]-18H
                call    near ptr strcat_
                mov     eax,dword ptr [ebp]-18H
                call    near ptr InputQueueLine_
                lea     esp,[ebp]-14H
                pop     edi
                pop     esi
                pop     edx
                pop     ecx
                pop     ebx
                pop     ebp
                ret
parse_token_:   push    ebp
                mov     ebp,esp
                push    00000038H
                call    near ptr __CHK
                push    ebx
                push    ecx
                push    esi
                push    edi
                sub     esp,0000001cH
                mov     dword ptr [ebp]-18H,eax
                mov     dword ptr [ebp]-14H,edx
                mov     eax,dword ptr [ebp]-18H
                mov     al,byte ptr [eax]
                mov     byte ptr [ebp]-1cH,al
                cmp     byte ptr [ebp]-1cH,2fH
                jb      short L108
                cmp     byte ptr [ebp]-1cH,2fH
                jbe     short L112
                cmp     byte ptr [ebp]-1cH,3fH
                jb      short L107
                cmp     byte ptr [ebp]-1cH,3fH
                jbe     short L110
                cmp     byte ptr [ebp]-1cH,40H
                je      near ptr L144
                jmp     near ptr L145
L107:           cmp     byte ptr [ebp]-1cH,3dH
                je      short L111
                jmp     near ptr L145
L108:           cmp     byte ptr [ebp]-1cH,23H
                jb      short L109
                cmp     byte ptr [ebp]-1cH,23H
                jbe     short L111
                cmp     byte ptr [ebp]-1cH,2dH
                je      short L112
                jmp     near ptr L145
L109:           jmp     near ptr L145
L110:           call    near ptr usage_msg_
                jmp     near ptr L146
L111:           mov     eax,0000043dH
                call    near ptr AsmError_
L112:           mov     eax,dword ptr [ebp]-18H
                inc     dword ptr [ebp]-18H
                mov     eax,dword ptr [ebp]-18H
                mov     al,byte ptr [eax]
                and     eax,000000ffH
                call    near ptr tolower_
                mov     dword ptr [ebp]-20H,eax
                cmp     dword ptr [ebp]-20H,00000066H
                jb      near ptr L116
                cmp     dword ptr [ebp]-20H,00000066H
                jbe     near ptr L135
                cmp     dword ptr [ebp]-20H,0000006dH
                jb      short L114
                cmp     dword ptr [ebp]-20H,0000006dH
                jbe     near ptr L124
                cmp     dword ptr [ebp]-20H,00000073H
                jb      short L113
                cmp     dword ptr [ebp]-20H,00000073H
                jbe     near ptr L128
                cmp     dword ptr [ebp]-20H,0000007aH
                je      near ptr L141
                jmp     near ptr L142
L113:           cmp     dword ptr [ebp]-20H,00000071H
                je      near ptr L140
                jmp     near ptr L142
L114:           cmp     dword ptr [ebp]-20H,00000069H
                jb      short L115
                cmp     dword ptr [ebp]-20H,00000069H
                jbe     near ptr L126
                cmp     dword ptr [ebp]-20H,0000006aH
                je      near ptr L128
                jmp     near ptr L142
L115:           cmp     dword ptr [ebp]-20H,00000068H
                je      near ptr L125
                jmp     near ptr L142
L116:           cmp     dword ptr [ebp]-20H,00000037H
                jb      short L118
                cmp     dword ptr [ebp]-20H,00000037H
                jbe     near ptr L123
                cmp     dword ptr [ebp]-20H,00000064H
                jb      short L117
                cmp     dword ptr [ebp]-20H,00000064H
                jbe     near ptr L129
                jmp     near ptr L127
L117:           cmp     dword ptr [ebp]-20H,0000003fH
                je      near ptr L125
                jmp     near ptr L142
L118:           cmp     dword ptr [ebp]-20H,00000033H
                jb      short L119
                cmp     dword ptr [ebp]-20H,00000034H
                jbe     short L120
                cmp     dword ptr [ebp]-20H,00000035H
                je      short L123
                jmp     near ptr L142
L119:           cmp     dword ptr [ebp]-20H,00000030H
                jae     short L123
                jmp     near ptr L142
L120:           mov     eax,dword ptr [ebp]-18H
                cmp     byte ptr [eax]+1H,72H
                jne     short L121
                mov     byte ptr _NamingConvention,01H
                jmp     near ptr L143
L121:           mov     eax,dword ptr [ebp]-18H
                cmp     byte ptr [eax]+1H,73H
                jne     short L122
                mov     byte ptr _NamingConvention,02H
                jmp     near ptr L143
L122:           mov     eax,dword ptr [ebp]-18H
                cmp     byte ptr [eax]+1H,00H
                je      short L123
                mov     edx,dword ptr [ebp]-18H
                mov     eax,0000044eH
                call    near ptr MsgPrintf1_
                mov     eax,00000001H
                jmp     near ptr exit_
L123:           mov     eax,dword ptr [ebp]-18H
                mov     al,byte ptr [eax]
                mov     byte ptr _ProcessorType,al
                jmp     near ptr L143
L124:           mov     eax,dword ptr [ebp]-18H
                mov     al,byte ptr [eax]+1H
                mov     byte ptr _MemType,al
                jmp     near ptr L143
L125:           call    near ptr usage_msg_
                jmp     near ptr L143
L126:           mov     eax,dword ptr [ebp]-18H
                add     eax,00000002H
                call    near ptr AddStringToIncludePath_
                jmp     near ptr L143
L127:           mov     byte ptr _StopAtEnd,01H
                jmp     near ptr L143
L128:           mov     byte ptr _SignValue,01H
                jmp     near ptr L143
L129:           mov     eax,dword ptr [ebp]-18H
                mov     al,byte ptr [eax]+1H
                mov     byte ptr [ebp]-24H,al
                cmp     byte ptr [ebp]-24H,31H
                jb      short L130
                cmp     byte ptr [ebp]-24H,32H
                jbe     short L131
                cmp     byte ptr [ebp]-24H,36H
                je      short L132
                jmp     short L133
L130:           jmp     short L133
L131:           mov     byte ptr _DebugFlag,01H
                jmp     near ptr L146
L132:           mov     byte ptr _Debug,01H
                mov     eax,offset DGROUP:L186
                push    eax
                call    near ptr printf_
                add     esp,00000004H
                jmp     near ptr L146
L133:           mov     eax,dword ptr [ebp]-18H
                inc     eax
                call    near ptr put_cmdline_const_in_list_
                cmp     eax,0ffffffffH
                jne     short L134
                mov     edx,dword ptr [ebp]-18H
                mov     eax,0000044eH
                call    near ptr MsgPrintf1_
                mov     eax,00000001H
                jmp     near ptr exit_
L134:           jmp     near ptr L143
L135:           mov     eax,dword ptr [ebp]-18H
                mov     al,byte ptr [eax]+1H
                mov     byte ptr [ebp]-28H,al
                cmp     byte ptr [ebp]-28H,65H
                jb      short L136
                cmp     byte ptr [ebp]-28H,65H
                jbe     short L138
                cmp     byte ptr [ebp]-28H,6fH
                je      short L137
                jmp     short L139
L136:           jmp     short L139
L137:           mov     edx,00000002H
                mov     eax,dword ptr [ebp]-18H
                add     eax,00000003H
                call    near ptr get_fname_
                jmp     near ptr L146
L138:           mov     edx,00000001H
                mov     eax,dword ptr [ebp]-18H
                add     eax,00000003H
                call    near ptr get_fname_
                jmp     short L146
L139:           mov     edx,dword ptr [ebp]-18H
                mov     eax,0000044eH
                call    near ptr MsgPrintf1_
                mov     eax,00000001H
                jmp     near ptr exit_
L140:           mov     byte ptr _Quiet,01H
                jmp     short L143
L141:           mov     eax,dword ptr [ebp]-18H
                mov     al,byte ptr [eax]+1H
                mov     byte ptr [ebp]-2cH,al
                cmp     byte ptr [ebp]-2cH,71H
                jne     short L142
                mov     byte ptr _Quiet,01H
                jmp     short L146
L142:           mov     edx,dword ptr [ebp]-18H
                mov     eax,0000044eH
                call    near ptr MsgPrintf1_
                mov     eax,00000001H
                jmp     near ptr exit_
L143:           jmp     short L146
L144:           mov     edx,dword ptr [ebp]-14H
                mov     eax,dword ptr [ebp]-18H
                inc     eax
                call    near ptr do_envvar_cmdline_
                jmp     short L146
L145:           xor     edx,edx
                mov     eax,dword ptr [ebp]-18H
                call    near ptr get_fname_
L146:           lea     esp,[ebp]-10H
                pop     edi
                pop     esi
                pop     ecx
                pop     ebx
                pop     ebp
                ret
parse_cmdline_: push    ebp
                mov     ebp,esp
                push    0000007cH
                call    near ptr __CHK
                push    ebx
                push    ecx
                push    esi
                push    edi
                sub     esp,0000005cH
                mov     dword ptr [ebp]-18H,eax
                mov     dword ptr [ebp]-1cH,edx
                cmp     dword ptr [ebp]-18H,00000001H
                jne     short L147
                call    near ptr usage_msg_
L147:           mov     dword ptr [ebp]-14H,00000001H
L148:           mov     eax,dword ptr [ebp]-14H
                cmp     eax,dword ptr [ebp]-18H
                jl      short L150
                jmp     short L151
L149:           mov     eax,dword ptr [ebp]-14H
                inc     dword ptr [ebp]-14H
                jmp     short L148
L150:           xor     edx,edx
                mov     eax,dword ptr [ebp]-14H
                shl     eax,02H
                add     eax,dword ptr [ebp]-1cH
                mov     eax,dword ptr [eax]
                call    near ptr parse_token_
                jmp     short L149
L151:           cmp     dword ptr _AsmFiles+0cH,00000000H
                jne     short L152
                lea     edx,[ebp]-6cH
                mov     eax,00000440H
                call    near ptr MsgGet_
                lea     eax,[ebp]-6cH
                push    eax
                push    00000001H
                call    near ptr Fatal_
                add     esp,00000008H
L152:           lea     esp,[ebp]-10H
                pop     edi
                pop     esi
                pop     ecx
                pop     ebx
                pop     ebp
                ret
AsmQueryExternal_:
                push    ebp
                mov     ebp,esp
                push    00000024H
                call    near ptr __CHK
                push    ebx
                push    ecx
                push    edx
                push    esi
                push    edi
                sub     esp,00000008H
                mov     dword ptr [ebp]-1cH,eax
                mov     byte ptr [ebp]-18H,00H
                mov     al,byte ptr [ebp]-18H
                lea     esp,[ebp]-14H
                pop     edi
                pop     esi
                pop     edx
                pop     ecx
                pop     ebx
                pop     ebp
                ret
AsmQueryType_:  push    ebp
                mov     ebp,esp
                push    00000024H
                call    near ptr __CHK
                push    ebx
                push    ecx
                push    edx
                push    esi
                push    edi
                sub     esp,00000008H
                mov     dword ptr [ebp]-1cH,eax
                mov     byte ptr [ebp]-18H,00H
                mov     al,byte ptr [ebp]-18H
                lea     esp,[ebp]-14H
                pop     edi
                pop     esi
                pop     edx
                pop     ecx
                pop     ebx
                pop     ebp
                ret
_TEXT           ENDS

CONST           SEGMENT DWORD PUBLIC USE32 'DATA'
L153            DB      5fH,5fH,57H,41H,53H,4dH,5fH,5fH
                DB      00H
L154            DB      57H,41H,54H,43H,4fH,4dH,20H,41H
                DB      73H,73H,65H,6dH,62H,6cH,65H,72H
                DB      20H,56H,65H,72H,73H,69H,6fH,6eH
                DB      20H,30H,2eH,39H,00H
L155            DB      43H,6fH,70H,79H,72H,69H,67H,68H
                DB      74H,20H,62H,79H,20H,57H,41H,54H
                DB      43H,4fH,4dH,20H,49H,6eH,74H,65H
                DB      72H,6eH,61H,74H,69H,6fH,6eH,61H
                DB      6cH,20H,43H,6fH,72H,70H,2eH,20H
                DB      31H,39H,39H,32H,2cH,20H,31H,39H
                DB      39H,33H,2eH,20H,41H,6cH,6cH,20H
                DB      72H,69H,67H,68H,74H,73H,20H,72H
                DB      65H,73H,65H,72H,76H,65H,64H,2eH
                DB      00H
L156            DB      57H,41H,54H,43H,4fH,4dH,20H,69H
                DB      73H,20H,61H,20H,74H,72H,61H,64H
                DB      65H,6dH,61H,72H,6bH,20H,6fH,66H
                DB      20H,57H,41H,54H,43H,4fH,4dH,20H
                DB      49H,6eH,74H,65H,72H,6eH,61H,74H
                DB      69H,6fH,6eH,61H,6cH,20H,43H,6fH
                DB      72H,70H,2eH,00H
L157            DB      20H,00H
L158            DB      25H,73H,0aH,00H
L159            DB      72H,00H
L160            DB      61H,73H,6dH,00H
L161            DB      6fH,62H,6aH,00H
L162            DB      65H,72H,72H,00H
L163            DB      00H
L164            DB      49H,4eH,43H,4cH,55H,44H,45H,00H
L165            DB      3dH,00H
L166            DB      23H,00H
L167            DB      3dH,31H,00H
L168            DB      38H,30H,38H,36H,00H
L169            DB      31H,38H,36H,00H
L170            DB      32H,38H,36H,00H
L171            DB      33H,38H,36H,00H
L172            DB      34H,38H,36H,00H
L173            DB      35H,38H,36H,00H
L174            DB      2eH,00H
L175            DB      5fH,5fH,00H
L176            DB      5fH,5fH,20H,65H,71H,75H,20H,31H
                DB      00H
L177            DB      2eH,4dH,4fH,44H,45H,4cH,20H,00H
L178            DB      43H,4fH,4dH,50H,41H,43H,54H,00H
L179            DB      46H,4cH,41H,54H,00H
L180            DB      48H,55H,47H,45H,00H
L181            DB      4cH,41H,52H,47H,45H,00H
L182            DB      4dH,45H,44H,49H,55H,4dH,00H
L183            DB      53H,4dH,41H,4cH,4cH,00H
L184            DB      54H,49H,4eH,59H,00H
L185            DB      2fH,6dH,00H
L186            DB      64H,65H,62H,75H,67H,67H,69H,6eH
                DB      67H,20H,6fH,75H,74H,70H,75H,74H
                DB      20H,6fH,6eH,20H,0aH,00H
CONST           ENDS

CONST2          SEGMENT DWORD PUBLIC USE32 'DATA'
CONST2          ENDS

_DATA           SEGMENT DWORD PUBLIC USE32 'DATA'
                PUBLIC  _Debug
                PUBLIC  _SignValue
                PUBLIC  _StopAtEnd
                PUBLIC  _BannerPrinted
                PUBLIC  _ErrorFlag
                PUBLIC  _WarningFlag
                PUBLIC  _DebugFlag
                PUBLIC  _NamingConvention
                PUBLIC  _CmdlineConstList
_Debug          DB      00H
_SignValue      DB      00H
_StopAtEnd      DB      00H
_Quiet          DB      00H
_BannerPrinted  DB      00H
_ProcessorType  DB      00H
_MemType        DB      00H
_ErrorFlag      DB      00H
_WarningFlag    DB      00H
_DebugFlag      DB      00H
_NamingConvention LABEL BYTE
                DB      00H
_CmdlineConstList LABEL BYTE
                DB      00H,00H,00H,00H
L187            DD      DGROUP:L154
                DD      DGROUP:L155
                DD      DGROUP:L156
                DD      DGROUP:L157
                DB      00H,00H,00H,00H
_DATA           ENDS

_BSS            SEGMENT DWORD PUBLIC USE32 'BSS'
                PUBLIC  _AsmFiles
                PUBLIC  _pobjState
                ORG     00000000H
_AsmFiles       LABEL   BYTE
                ORG     00000018H
_pobjState      LABEL   BYTE
                ORG     0000001dH
_BSS            ENDS

$$SYMBOLS       SEGMENT BYTE USE16 'DEBSYM'
                DB      11H,12H
                DF      DGROUP:_AsmFiles
                DB      34H,41H,73H,6dH,46H,69H,6cH,65H
                DB      73H,19H,12H
                DF      DGROUP:_CmdlineConstList
                DB      39H,43H,6dH,64H,6cH,69H,6eH,65H
                DB      43H,6fH,6eH,73H,74H,4cH,69H,73H
                DB      74H,12H,12H
                DF      DGROUP:_pobjState
                DB      3fH,70H,6fH,62H,6aH,53H,74H,61H
                DB      74H,65H,0eH,12H
                DF      DGROUP:_Quiet
                DB      0aH,51H,75H,69H,65H,74H,19H,12H
                DF      DGROUP:_NamingConvention
                DB      0aH,4eH,61H,6dH,69H,6eH,67H,43H
                DB      6fH,6eH,76H,65H,6eH,74H,69H,6fH
                DB      6eH,10H,12H
                DF      DGROUP:_MemType
                DB      0aH,4dH,65H,6dH,54H,79H,70H,65H
                DB      16H,12H
                DF      DGROUP:_ProcessorType
                DB      0aH,50H,72H,6fH,63H,65H,73H,73H
                DB      6fH,72H,54H,79H,70H,65H,12H,12H
                DF      DGROUP:_SignValue
                DB      22H,53H,69H,67H,6eH,56H,61H,6cH
                DB      75H,65H,0eH,12H
                DF      DGROUP:_Debug
                DB      0aH,44H,65H,62H,75H,67H,12H,12H
                DF      DGROUP:_StopAtEnd
                DB      0aH,53H,74H,6fH,70H,41H,74H,45H
                DB      6eH,64H,16H,12H
                DF      DGROUP:_BannerPrinted
                DB      0aH,42H,61H,6eH,6eH,65H,72H,50H
                DB      72H,69H,6eH,74H,65H,64H,12H,12H
                DF      DGROUP:_DebugFlag
                DB      0aH,44H,65H,62H,75H,67H,46H,6cH
                DB      61H,67H,14H,12H
                DF      DGROUP:_WarningFlag
                DB      0aH,57H,61H,72H,6eH,69H,6eH,67H
                DB      46H,6cH,61H,67H,12H,12H
                DF      DGROUP:_ErrorFlag
                DB      0aH,45H,72H,72H,6fH,72H,46H,6cH
                DB      61H,67H,08H,32H
                DF      isvalidident_
                DB      24H,24H,00H,00H,00H,00H,0d8H,00H
                DB      00H,00H,00H,00H,18H,0aH,04H,00H
                DB      00H,00H,40H,30H,1cH,01H,30H,1cH
                DB      69H,73H,76H,61H,6cH,69H,64H,69H
                DB      64H,65H,6eH,74H,06H,11H,10H,0e4H
                DB      13H,73H,0dH,11H,10H,0e8H,0aH,6cH
                DB      77H,72H,5fH,63H,68H,61H,72H,07H
                DB      11H,10H,0dcH,13H,69H,64H,1eH,24H
                DB      0d8H,00H,00H,00H,0adH,00H,00H,00H
                DB      00H,00H,17H,09H,04H,00H,00H,00H
                DB      42H,30H,1cH,02H,30H,1cH,30H,1fH
                DB      6dH,61H,69H,6eH,09H,11H,10H,0e8H
                DB      41H,61H,72H,67H,76H,09H,11H,10H
                DB      0e4H,08H,61H,72H,67H,63H,1eH,24H
                DB      85H,01H,00H,00H,31H,00H,00H,00H
                DB      00H,00H,18H,00H,04H,00H,00H,00H
                DB      43H,00H,00H,75H,73H,61H,67H,65H
                DB      5fH,6dH,73H,67H,1eH,24H,0b6H,01H
                DB      00H,00H,60H,00H,00H,00H,00H,00H
                DB      18H,0aH,04H,00H,00H,00H,43H,00H
                DB      00H,74H,72H,61H,64H,65H,6dH,61H
                DB      72H,6bH,0dH,11H,21H
                DF      DGROUP:L187
                DB      44H,6dH,73H,67H,0aH,11H,10H,0e8H
                DB      08H,63H,6fH,75H,6eH,74H,1eH,24H
                DB      16H,02H,00H,00H,40H,00H,00H,00H
                DB      00H,00H,18H,0aH,04H,00H,00H,00H
                DB      43H,00H,00H,66H,72H,65H,65H,5fH
                DB      66H,69H,6cH,65H,1eH,24H,56H,02H
                DB      00H,00H,6aH,00H,00H,00H,00H,00H
                DB      18H,0aH,04H,00H,00H,00H,43H,00H
                DB      00H,6dH,61H,69H,6eH,5fH,69H,6eH
                DB      69H,74H,06H,11H,10H,0e8H,08H,69H
                DB      1eH,24H,0c0H,02H,00H,00H,36H,00H
                DB      00H,00H,00H,00H,18H,0aH,04H,00H
                DB      00H,00H,43H,00H,00H,6dH,61H,69H
                DB      6eH,5fH,66H,69H,6eH,69H,1fH,24H
                DB      0f6H,02H,00H,00H,83H,00H,00H,00H
                DB      00H,00H,18H,0aH,04H,00H,00H,00H
                DB      43H,00H,00H,6fH,70H,65H,6eH,5fH
                DB      66H,69H,6cH,65H,73H,22H,24H,79H
                DB      03H,00H,00H,4aH,02H,00H,00H,00H
                DB      00H,17H,09H,04H,00H,00H,00H,45H
                DB      00H,02H,30H,1cH,30H,1fH,67H,65H
                DB      74H,5fH,66H,6eH,61H,6dH,65H,0aH
                DB      11H,10H,0ecH,13H,64H,72H,69H,76H
                DB      65H,08H,11H,10H,0e8H,13H,64H,69H
                DB      72H,0aH,11H,10H,0e4H,13H,66H,6eH
                DB      61H,6dH,65H,08H,11H,10H,0e0H,13H
                DB      65H,78H,74H,0cH,11H,11H,64H,0feH
                DB      46H,62H,75H,66H,66H,65H,72H,0aH
                DB      11H,11H,0f8H,0feH,47H,6eH,61H,6dH
                DB      65H,0bH,11H,10H,88H,48H,6dH,73H
                DB      67H,62H,75H,66H,09H,11H,10H,0d8H
                DB      08H,74H,79H,70H,65H,0aH,11H,10H
                DB      0dcH,13H,74H,6fH,6bH,65H,6eH,2aH
                DB      24H,0c3H,05H,00H,00H,0ecH,00H,00H
                DB      00H,00H,00H,17H,09H,04H,00H,00H
                DB      00H,45H,00H,02H,30H,1cH,30H,1fH
                DB      64H,6fH,5fH,65H,6eH,76H,76H,61H
                DB      72H,5fH,63H,6dH,64H,6cH,69H,6eH
                DB      65H,0cH,11H,10H,0ecH,13H,63H,6dH
                DB      64H,6cH,69H,6eH,65H,0aH,11H,10H
                DB      0e8H,13H,74H,6fH,6bH,65H,6eH,09H
                DB      11H,10H,0e4H,13H,6eH,65H,78H,74H
                DB      0aH,11H,10H,0e0H,08H,6cH,65H,76H
                DB      65H,6cH,0bH,11H,10H,0dcH,13H,65H
                DB      6eH,76H,76H,61H,72H,22H,24H,0afH
                DB      06H,00H,00H,70H,00H,00H,00H,00H
                DB      00H,18H,0aH,04H,00H,00H,00H,49H
                DB      00H,01H,30H,1cH,41H,64H,64H,43H
                DB      6fH,6eH,73H,74H,61H,6eH,74H,0bH
                DB      11H,10H,0b4H,4eH,74H,6fH,6bH,65H
                DB      6eH,73H,0aH,11H,10H,0e8H,1bH,63H
                DB      6fH,75H,6eH,74H,0cH,11H,11H,14H
                DB      0ffH,4fH,62H,75H,66H,66H,65H,72H
                DB      0bH,11H,10H,0e4H,13H,73H,74H,72H
                DB      69H,6eH,67H,2aH,24H,1fH,07H,00H
                DB      00H,9eH,00H,00H,00H,00H,00H,18H
                DB      0aH,04H,00H,00H,00H,43H,00H,00H
                DB      50H,72H,6fH,63H,65H,73H,73H,43H
                DB      6dH,64H,6cH,69H,6eH,65H,4fH,70H
                DB      74H,69H,6fH,6eH,73H,09H,11H,10H
                DB      0e8H,39H,63H,75H,72H,72H,09H,11H
                DB      10H,0e4H,39H,6eH,65H,78H,74H,08H
                DB      11H,10H,0e0H,13H,65H,6eH,76H,31H
                DB      24H,0bdH,07H,00H,00H,81H,01H,00H
                DB      00H,00H,00H,18H,0aH,04H,00H,00H
                DB      00H,40H,30H,1cH,01H,30H,1cH,70H
                DB      75H,74H,5fH,63H,6dH,64H,6cH,69H
                DB      6eH,65H,5fH,63H,6fH,6eH,73H,74H
                DB      5fH,69H,6eH,5fH,6cH,69H,73H,74H
                DB      08H,11H,10H,0e8H,50H,70H,74H,72H
                DB      0aH,11H,10H,0e4H,1aH,63H,6fH,75H
                DB      6eH,74H,08H,11H,10H,0e0H,13H,74H
                DB      6dH,70H,0bH,11H,10H,0d8H,13H,73H
                DB      74H,72H,69H,6eH,67H,27H,24H,3eH
                DB      09H,00H,00H,34H,01H,00H,00H,00H
                DB      00H,18H,0aH,04H,00H,00H,00H,43H
                DB      00H,00H,73H,65H,74H,5fH,70H,72H
                DB      6fH,63H,65H,73H,73H,6fH,72H,5fH
                DB      74H,79H,70H,65H,0bH,11H,10H,0e8H
                DB      13H,62H,75H,66H,66H,65H,72H,0eH
                DB      11H,10H,0e4H,13H,70H,72H,6fH,63H
                DB      65H,73H,73H,6fH,72H,21H,24H,72H
                DB      0aH,00H,00H,7dH,01H,00H,00H,00H
                DB      00H,18H,0aH,04H,00H,00H,00H,43H
                DB      00H,00H,73H,65H,74H,5fH,6dH,65H
                DB      6dH,5fH,74H,79H,70H,65H,0bH,11H
                DB      10H,0e8H,13H,62H,75H,66H,66H,65H
                DB      72H,0aH,11H,10H,0e4H,13H,6dH,6fH
                DB      64H,65H,6cH,24H,24H,0efH,0bH,00H
                DB      00H,37H,03H,00H,00H,00H,00H,17H
                DB      09H,04H,00H,00H,00H,45H,00H,02H
                DB      30H,1cH,30H,1fH,70H,61H,72H,73H
                DB      65H,5fH,74H,6fH,6bH,65H,6eH,12H
                DB      11H,10H,0ecH,08H,6eH,65H,73H,74H
                DB      69H,6eH,67H,5fH,6cH,65H,76H,65H
                DB      6cH,0aH,11H,10H,0e8H,13H,74H,6fH
                DB      6bH,65H,6eH,26H,24H,26H,0fH,00H
                DB      00H,82H,00H,00H,00H,00H,00H,17H
                DB      09H,04H,00H,00H,00H,51H,00H,02H
                DB      30H,1cH,30H,1fH,70H,61H,72H,73H
                DB      65H,5fH,63H,6dH,64H,6cH,69H,6eH
                DB      65H,06H,11H,10H,0ecH,08H,69H,0bH
                DB      11H,10H,94H,52H,6dH,73H,67H,62H
                DB      75H,66H,09H,11H,10H,0e4H,41H,61H
                DB      72H,67H,76H,09H,11H,10H,0e8H,08H
                DB      61H,72H,67H,63H,27H,24H,0a8H,0fH
                DB      00H,00H,2cH,00H,00H,00H,00H,00H
                DB      18H,0aH,04H,00H,00H,00H,55H,40H
                DB      01H,30H,1cH,41H,73H,6dH,51H,75H
                DB      65H,72H,79H,45H,78H,74H,65H,72H
                DB      6eH,61H,6cH,09H,11H,10H,0e4H,13H
                DB      6eH,61H,6dH,65H,23H,24H,0d4H,0fH
                DB      00H,00H,2cH,00H,00H,00H,00H,00H
                DB      18H,0aH,04H,00H,00H,00H,58H,40H
                DB      01H,30H,1cH,41H,73H,6dH,51H,75H
                DB      65H,72H,79H,54H,79H,70H,65H,09H
                DB      11H,10H,0e4H,13H,6eH,61H,6dH,65H
$$SYMBOLS       ENDS

$$TYPES         SEGMENT BYTE USE16 'DEBTYP'
                DB      08H,11H,73H,74H,72H,75H,63H,74H
                DB      07H,11H,75H,6eH,69H,6fH,6eH,06H
                DB      11H,65H,6eH,75H,6dH,03H,10H,00H
                DB      03H,10H,10H,03H,10H,01H,03H,10H
                DB      11H,03H,10H,03H,03H,10H,13H,08H
                DB      12H,00H,05H,63H,68H,61H,72H,08H
                DB      12H,00H,05H,63H,68H,61H,72H,0aH
                DB      12H,00H,09H,73H,69H,7aH,65H,5fH
                DB      74H,0bH,12H,00H,07H,77H,63H,68H
                DB      61H,72H,5fH,74H,0bH,12H,00H,07H
                DB      77H,63H,68H,61H,72H,5fH,74H,03H
                DB      10H,30H,04H,72H,0fH,00H,03H,46H
                DB      10H,0cH,12H,00H,11H,6fH,6eH,65H
                DB      78H,69H,74H,5fH,74H,03H,46H,0aH
                DB      04H,20H,00H,13H,0dH,12H,00H,14H
                DB      5fH,5fH,76H,61H,5fH,6cH,69H,73H
                DB      74H,03H,10H,03H,0aH,12H,00H,16H
                DB      66H,70H,6fH,73H,5fH,74H,0aH,12H
                DB      00H,16H,66H,70H,6fH,73H,5fH,74H
                DB      08H,12H,00H,0aH,62H,6fH,6fH,6cH
                DB      08H,12H,00H,09H,75H,69H,6eH,74H
                DB      0aH,12H,00H,05H,75H,69H,6eH,74H
                DB      5fH,38H,0bH,12H,00H,07H,75H,69H
                DB      6eH,74H,5fH,31H,36H,03H,10H,13H
                DB      0bH,12H,00H,1dH,75H,69H,6eH,74H
                DB      5fH,33H,32H,0eH,12H,00H,05H,75H
                DB      6eH,73H,69H,67H,6eH,65H,64H,5fH
                DB      38H,0fH,12H,00H,07H,75H,6eH,73H
                DB      69H,67H,6eH,65H,64H,5fH,31H,36H
                DB      0fH,12H,00H,1dH,75H,6eH,73H,69H
                DB      67H,6eH,65H,64H,5fH,33H,32H,09H
                DB      12H,00H,04H,69H,6eH,74H,5fH,38H
                DB      0aH,12H,00H,06H,69H,6eH,74H,5fH
                DB      31H,36H,0aH,12H,00H,16H,69H,6eH
                DB      74H,5fH,33H,32H,0cH,12H,00H,04H
                DB      73H,69H,67H,6eH,65H,64H,5fH,38H
                DB      0dH,12H,00H,06H,73H,69H,67H,6eH
                DB      65H,64H,5fH,31H,36H,0dH,12H,00H
                DB      16H,73H,69H,67H,6eH,65H,64H,5fH
                DB      33H,32H,0dH,12H,00H,08H,70H,74H
                DB      72H,64H,69H,66H,66H,5fH,74H,0fH
                DB      12H,00H,1cH,6eH,61H,6dH,65H,5fH
                DB      68H,61H,6eH,64H,6cH,65H,0aH,12H
                DB      00H,05H,75H,69H,6eH,74H,5fH,38H
                DB      0eH,12H,00H,08H,64H,69H,72H,65H
                DB      63H,74H,5fH,69H,64H,78H,03H,46H
                DB      05H,08H,60H,08H,00H,1aH,00H,00H
                DB      00H,0dH,61H,19H,05H,5fH,74H,6dH
                DB      70H,66H,63H,68H,61H,72H,0dH,61H
                DB      18H,05H,5fH,75H,6eH,67H,6fH,74H
                DB      74H,65H,6eH,0cH,61H,14H,09H,5fH
                DB      62H,75H,66H,73H,69H,7aH,65H,0bH
                DB      61H,10H,08H,5fH,68H,61H,6eH,64H
                DB      6cH,65H,09H,61H,0cH,09H,5fH,66H
                DB      6cH,61H,67H,09H,61H,08H,2cH,5fH
                DB      62H,61H,73H,65H,08H,61H,04H,08H
                DB      5fH,63H,6eH,74H,08H,61H,00H,2cH
                DB      5fH,70H,74H,72H,0bH,12H,01H,2dH
                DB      5fH,5fH,69H,6fH,62H,75H,66H,08H
                DB      12H,00H,2eH,46H,49H,4cH,45H,03H
                DB      46H,2fH,04H,20H,02H,30H,04H,20H
                DB      02H,13H,08H,60H,02H,00H,18H,00H
                DB      00H,00H,09H,61H,0cH,32H,66H,6eH
                DB      61H,6dH,65H,08H,61H,00H,31H,66H
                DB      69H,6cH,65H,0dH,12H,00H,33H,46H
                DB      69H,6cH,65H,5fH,49H,6eH,66H,6fH
                DB      09H,12H,01H,80H,37H,6eH,6fH,64H
                DB      65H,03H,46H,35H,08H,60H,02H,00H
                DB      08H,00H,00H,00H,0aH,61H,04H,13H
                DB      73H,74H,72H,69H,6eH,67H,08H,61H
                DB      00H,36H,6eH,65H,78H,74H,08H,12H
                DB      00H,35H,6eH,6fH,64H,65H,03H,46H
                DB      38H,04H,20H,00H,0aH,08H,60H,06H
                DB      00H,0dH,00H,00H,00H,0aH,61H,0cH
                DB      3aH,62H,75H,66H,66H,65H,72H,0cH
                DB      64H,0bH,00H,01H,05H,69H,6eH,5fH
                DB      72H,65H,63H,0cH,61H,0aH,1bH,63H
                DB      68H,65H,63H,6bH,73H,75H,6dH,0aH
                DB      61H,06H,0cH,69H,6eH,5fH,62H,75H
                DB      66H,0aH,61H,04H,1cH,6cH,65H,6eH
                DB      67H,74H,68H,06H,61H,00H,08H,66H
                DB      68H,0dH,12H,00H,3bH,4fH,42H,4aH
                DB      5fH,57H,46H,49H,4cH,45H,03H,46H
                DB      3cH,08H,60H,02H,00H,05H,00H,00H
                DB      00H,0cH,61H,01H,3dH,66H,69H,6cH
                DB      65H,5fH,6fH,75H,74H,08H,61H,00H
                DB      1bH,70H,61H,73H,73H,0eH,12H,00H
                DB      3eH,70H,6fH,62H,6aH,5fH,73H,74H
                DB      61H,74H,65H,05H,72H,08H,01H,13H
                DB      03H,46H,13H,06H,72H,08H,02H,08H
                DB      41H,05H,72H,0fH,01H,0fH,04H,20H
                DB      04H,13H,06H,72H,0fH,02H,13H,08H
                DB      05H,21H,92H,00H,0aH,05H,21H,8fH
                DB      00H,0aH,04H,20H,4fH,0aH,05H,72H
                DB      0fH,01H,13H,05H,50H,21H,00H,10H
                DB      09H,51H,27H,54H,5fH,50H,41H,54H
                DB      48H,0cH,51H,26H,54H,5fH,42H,49H
                DB      4eH,5fH,4eH,55H,4dH,0cH,51H,25H
                DB      54H,5fH,50H,45H,52H,43H,45H,4eH
                DB      54H,12H,51H,3fH,54H,5fH,51H,55H
                DB      45H,53H,54H,49H,4fH,4eH,5fH,4dH
                DB      41H,52H,4bH,08H,51H,2eH,54H,5fH
                DB      44H,4fH,54H,0aH,51H,2dH,54H,5fH
                DB      4dH,49H,4eH,55H,53H,09H,51H,2bH
                DB      54H,5fH,50H,4cH,55H,53H,0aH,51H
                DB      2aH,54H,5fH,54H,49H,4dH,45H,53H
                DB      0fH,51H,3bH,54H,5fH,53H,45H,4dH
                DB      49H,5fH,43H,4fH,4cH,4fH,4eH,0aH
                DB      51H,3aH,54H,5fH,43H,4fH,4cH,4fH
                DB      4eH,0aH,51H,2cH,54H,5fH,43H,4fH
                DB      4dH,4dH,41H,12H,51H,5dH,54H,5fH
                DB      43H,4cH,5fH,53H,51H,5fH,42H,52H
                DB      41H,43H,4bH,45H,54H,0fH,51H,29H
                DB      54H,5fH,43H,4cH,5fH,42H,52H,41H
                DB      43H,4bH,45H,54H,12H,51H,5bH,54H
                DB      5fH,4fH,50H,5fH,53H,51H,5fH,42H
                DB      52H,41H,43H,4bH,45H,54H,0fH,51H
                DB      28H,54H,5fH,4fH,50H,5fH,42H,52H
                DB      41H,43H,4bH,45H,54H,15H,51H,11H
                DB      54H,5fH,49H,44H,5fH,49H,4eH,5fH
                DB      42H,41H,43H,4bH,51H,55H,4fH,54H
                DB      45H,53H,0dH,51H,10H,54H,5fH,4eH
                DB      45H,47H,41H,54H,49H,56H,45H,0dH
                DB      51H,0fH,54H,5fH,50H,4fH,53H,49H
                DB      54H,49H,56H,45H,09H,51H,0eH,54H
                DB      5fH,4eH,4fH,4fH,50H,0aH,51H,0dH
                DB      54H,5fH,46H,4cH,4fH,41H,54H,08H
                DB      51H,0cH,54H,5fH,4eH,55H,4dH,0cH
                DB      51H,0bH,54H,5fH,48H,45H,58H,5fH
                DB      4eH,55H,4dH,0eH,51H,0aH,54H,5fH
                DB      48H,45H,58H,5fH,4eH,55H,4dH,5fH
                DB      30H,0cH,51H,09H,54H,5fH,4fH,43H
                DB      54H,5fH,4eH,55H,4dH,0cH,51H,08H
                DB      54H,5fH,44H,45H,43H,5fH,4eH,55H
                DB      4dH,10H,51H,07H,54H,5fH,44H,49H
                DB      52H,45H,43H,54H,5fH,45H,58H,50H
                DB      52H,0eH,51H,06H,54H,5fH,44H,49H
                DB      52H,45H,43H,54H,49H,56H,45H,0bH
                DB      51H,05H,54H,5fH,53H,54H,52H,49H
                DB      4eH,47H,08H,51H,04H,54H,5fH,52H
                DB      45H,47H,07H,51H,03H,54H,5fH,49H
                DB      44H,0bH,51H,02H,54H,5fH,52H,45H
                DB      53H,5fH,49H,44H,08H,51H,01H,54H
                DB      5fH,49H,4eH,53H,0aH,51H,00H,54H
                DB      5fH,46H,49H,4eH,41H,4cH,09H,12H
                DB      03H,4aH,73H,74H,61H,74H,65H,08H
                DB      60H,03H,00H,09H,00H,00H,00H,0eH
                DB      61H,05H,13H,73H,74H,72H,69H,6eH
                DB      67H,5fH,70H,74H,72H,09H,61H,04H
                DB      4aH,74H,6fH,6bH,65H,6eH,09H,61H
                DB      00H,16H,76H,61H,6cH,75H,65H,0bH
                DB      12H,01H,4cH,61H,73H,6dH,5fH,74H
                DB      6fH,6bH,04H,20H,04H,4dH,05H,21H
                DB      9fH,00H,0aH,03H,46H,39H,06H,72H
                DB      0fH,02H,08H,41H,04H,20H,4fH,0aH
                DB      05H,50H,0dH,00H,10H,12H,51H,0cH
                DB      53H,59H,4dH,5fH,43H,4cH,41H,53H
                DB      53H,5fH,4cH,4eH,41H,4dH,45H,0cH
                DB      51H,0bH,53H,59H,4dH,5fH,4cH,4eH
                DB      41H,4dH,45H,0aH,51H,0aH,53H,59H
                DB      4dH,5fH,45H,58H,54H,0aH,51H,09H
                DB      53H,59H,4dH,5fH,4cH,49H,42H,0cH
                DB      51H,08H,53H,59H,4dH,5fH,43H,4fH
                DB      4eH,53H,54H,0cH,51H,07H,53H,59H
                DB      4dH,5fH,4dH,41H,43H,52H,4fH,0bH
                DB      51H,06H,53H,59H,4dH,5fH,50H,52H
                DB      4fH,43H,0aH,51H,05H,53H,59H,4dH
                DB      5fH,47H,52H,50H,0aH,51H,04H,53H
                DB      59H,4dH,5fH,53H,45H,47H,0cH,51H
                DB      03H,53H,59H,4dH,5fH,53H,54H,41H
                DB      43H,4bH,0fH,51H,02H,53H,59H,4dH
                DB      5fH,45H,58H,54H,45H,52H,4eH,41H
                DB      4cH,0fH,51H,01H,53H,59H,4dH,5fH
                DB      49H,4eH,54H,45H,52H,4eH,41H,4cH
                DB      10H,51H,00H,53H,59H,4dH,5fH,55H
                DB      4eH,44H,45H,46H,49H,4eH,45H,44H
                DB      0dH,12H,03H,53H,73H,79H,6dH,5fH
                DB      73H,74H,61H,74H,65H,05H,72H,53H
                DB      01H,13H,05H,50H,0bH,00H,10H,0bH
                DB      51H,0aH,53H,59H,4dH,5fH,46H,41H
                DB      52H,34H,0bH,51H,09H,53H,59H,4dH
                DB      5fH,46H,41H,52H,32H,0cH,51H,08H
                DB      53H,59H,4dH,5fH,4eH,45H,41H,52H
                DB      34H,0cH,51H,07H,53H,59H,4dH,5fH
                DB      4eH,45H,41H,52H,32H,0eH,51H,06H
                DB      53H,59H,4dH,5fH,46H,4cH,4fH,41H
                DB      54H,31H,30H,0dH,51H,05H,53H,59H
                DB      4dH,5fH,46H,4cH,4fH,41H,54H,38H
                DB      0dH,51H,04H,53H,59H,4dH,5fH,46H
                DB      4cH,4fH,41H,54H,34H,0bH,51H,03H
                DB      53H,59H,4dH,5fH,49H,4eH,54H,36H
                DB      0bH,51H,02H,53H,59H,4dH,5fH,49H
                DB      4eH,54H,34H,0bH,51H,01H,53H,59H
                DB      4dH,5fH,49H,4eH,54H,32H,0bH,51H
                DB      00H,53H,59H,4dH,5fH,49H,4eH,54H
                DB      31H,0cH,12H,03H,56H,73H,79H,6dH
                DB      5fH,74H,79H,70H,65H,05H,72H,56H
                DB      01H,13H
$$TYPES         ENDS

END
