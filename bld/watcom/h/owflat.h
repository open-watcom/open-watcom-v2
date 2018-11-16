 
/* 32-bit compiler */

// check carry flag, and set eax=0 if set and eax=1 if clear
#define CarryToBool 0x73 4 0x33 0xC0 0xEB 5 0xB8 1 0 0 0

// check carry flag, and set ebx=0 if set and ebx=bx if clear
#define ValidateHandle 0x73 2 0x33 0xDB 0xF 0xB7 0xDB

// check carry flag, and set eax=0 if set
#define ValidateEax 0x73 2 0x33 0xC0

// check carry flag, and set ecx=0 if set
#define ValidateEcx 0x73 2 0x33 0xC9

// check carry flag, and set edx=0 if set
#define ValidateEdx 0x73 2 0x33 0xD2

// check carry flag, and set esi=0 if set
#define ValidateEsi 0x73 2 0x33 0xF6

// check carry flag, and set edi=0 if set
#define ValidateEdi 0x73 2 0x33 0xFF

// check disc id, set to -1 on carry, extend to eax
#define ValidateDisc 0x73 2 0xB0 0xFF 0xF 0xBE 0xC0

#pragma aux RdosTestGate = \
    CallGate_test_gate \
    parm [edi] \
    value [eax];

#pragma aux RdosLoad32 = \
    CallGate_load_device32  \

#pragma aux RdosSwapShort = \
    "xchg al,ah"    \
    parm [ax]   \
    value [ax];

#pragma aux RdosSwapLong = \
    "xchg al,ah"    \
    "rol eax,16"    \
    "xchg al,ah"    \
    parm [eax]   \
    value [eax];

#pragma aux RdosGetCharSize = \
    "mov al,1" \
    "mov ah,[edi]" \
    "test ah,80h" \
    "jz size_ok" \
    "inc al" \
    "test ah,20h" \
    "jz size_ok" \
    "inc al" \
    "test ah,10h" \
    "jz size_ok" \
    "inc al" \
    "size_ok: " \
    "movzx eax,al" \
    parm [edi]   \
    value [eax];

#pragma aux RdosGetLongRandom = \
    CallGate_get_random  \
    value [eax];

#pragma aux RdosGetRandom = \
    CallGate_get_random  \
    "mul edx" \
    parm [edx] \
    value [edx] \
    modify [eax];

#pragma aux RdosGetMac = \
    CallGate_get_mac_address  \
    CarryToBool \
    parm [edi] \
    value [eax];

#pragma aux RdosCreateUuid = \
    CallGate_create_uuid  \
    parm [edi];

#pragma aux RdosGetAcpiStatus = \
    CallGate_get_acpi_status  \
    "jnc AcpiDone" \
    "mov eax,-1" \
    "AcpiDone: " \
    value [eax];

#pragma aux RdosGetAcpiObject = \
    CallGate_get_acpi_object  \
    CarryToBool \
    parm [eax] [edi] \
    value [eax];

#pragma aux RdosGetAcpiMethod = \
    CallGate_get_acpi_method  \
    CarryToBool \
    parm [eax] [edx] [edi] \
    value [eax];

#pragma aux RdosGetAcpiDevice = \
    CallGate_get_acpi_device  \
    CarryToBool \
    parm [eax] [edi] \
    value [eax];

#pragma aux RdosGetAcpiDeviceIrq = \
    "push ecx" \
    "push edx" \
    CallGate_get_acpi_device_irq  \
    "jc IrqFail" \
    "movzx ecx,ah" \
    "mov [ebx],ecx" \
    "movzx ecx,dl" \
    "mov [esi],ecx" \
    "movzx ecx,dh" \
    "mov [edi],ecx" \
    "movzx eax,al" \
    "jmp IrqDone" \
    "IrqFail:" \
    "mov eax,-1" \
    "IrqDone:" \
    "pop edx" \
    "pop ecx" \
    parm [eax] [edx] [ebx] [esi] [edi] \
    value [eax];

#pragma aux RdosGetAcpiDeviceIo = \
    "push ecx" \
    "push edi" \
    "push esi" \
    CallGate_get_acpi_device_io  \
    "jc IoFail" \
    "movzx eax,si" \
    "pop esi" \
    "mov [esi],eax" \
    "movzx eax,di" \
    "pop edi" \
    "mov [edi],eax" \
    "movzx eax,cx" \
    "jmp IoDone" \
    "IoFail:" \
    "pop esi" \
    "pop edi" \
    "xor eax,eax" \
    "IoDone:" \
    "pop ecx" \
    parm [eax] [edx] [esi] [edi] \
    value [eax];

#pragma aux RdosGetAcpiDeviceMem = \
    "push ecx" \
    "push edi" \
    "push esi" \
    CallGate_get_acpi_device_mem  \
    "jc MemFail" \
    "mov eax,esi" \
    "pop esi" \
    "mov [esi],eax" \
    "mov eax,edi" \
    "pop edi" \
    "mov [edi],eax" \
    "mov eax,ecx" \
    "jmp MemDone" \
    "MemFail:" \
    "pop esi" \
    "pop edi" \
    "xor eax,eax" \
    "MemDone:" \
    "pop ecx" \
    parm [eax] [edx] [esi] [edi] \
    value [eax];
    
#pragma aux RdosGetPciDeviceName = \
    CallGate_get_pci_device_name  \
    CarryToBool \
    parm [eax] [edi] \
    value [eax];

#pragma aux RdosGetPciDeviceInfo = \
    "push ecx" \
    "push ebx" \
    "push esi" \
    CallGate_get_pci_device_info  \
    "jc Fail" \
    "movzx eax,bh" \
    "pop esi" \
    "mov [esi],eax" \
    "movzx eax,bl" \
    "pop ebx" \
    "mov [ebx],eax" \
    "movzx eax,ch" \
    "pop ecx" \
    "mov [ecx],eax" \
    "mov eax,1" \
    "jmp Done" \
    "Fail:" \
    "pop esi" \
    "pop ebx" \
    "pop ecx" \
    "xor eax,eax" \
    "Done:" \
    parm [eax] [esi] [ebx] [ecx] \
    value [eax];

#pragma aux RdosGetPciDeviceVendor = \
    "push edx" \
    CallGate_get_pci_device_vendor  \
    "jc Fail" \
    "movzx eax,ax" \
    "mov [esi],eax" \
    "movzx edx,dx" \
    "mov [edi],edx" \
    "mov eax,1" \
    "jmp Done" \
    "Fail:" \
    "xor eax,eax" \
    "Done:" \
    "pop edx" \    
    parm [eax] [esi] [edi] \
    value [eax];

#pragma aux RdosGetPciDeviceClass = \
    CallGate_get_pci_device_class  \
    "jc Fail" \
    "push edx" \
    "movzx edx,ah" \
    "mov [esi],edx" \
    "movzx edx,al" \
    "mov [edi],edx" \
    "mov eax,1" \
    "pop edx" \
    "jmp Done" \
    "Fail:" \
    "xor eax,eax" \
    "Done:" \
    parm [eax] [esi] [edi] \
    value [eax];

#pragma aux RdosGetPciDeviceIrq = \
    CallGate_get_pci_device_irq  \
    "jc Fail" \
    "movzx eax,al" \
    "jmp Done" \
    "Fail:" \
    "mov eax,-1" \
    "Done:" \
    parm [eax] \
    value [eax];
    
#pragma aux RdosGetCpuTemperature = \
    CallGate_get_cpu_temperature  \
    "jnc TempDone" \
    "xor eax,eax" \
    "TempDone: " \
    value [eax];

#pragma aux RdosGetHidReportItem = \
    CallGate_get_hid_report_item  \
    CarryToBool \
    parm [eax] [edx] [edi] \
    value [eax];

#pragma aux RdosGetHidReportInputData = \
    CallGate_get_hid_report_input_data  \
    CarryToBool \
    parm [eax] [ebx] [edx] [edi] \
    value [eax];

#pragma aux RdosGetHidReportOutputData = \
    CallGate_get_hid_report_output_data  \
    CarryToBool \
    parm [eax] [ebx] [edx] [edi] \
    value [eax];

#pragma aux RdosGetHidReportFeatureData = \
    CallGate_get_hid_report_feature_data  \
    CarryToBool \
    parm [eax] [ebx] [edx] [edi] \
    value [eax];

#pragma aux RdosGetAudioDeviceCount = \
    "xor ecx,ecx" \
    CallGate_get_audio_device_count  \
    value [ecx];

#pragma aux RdosGetAudioCodecCount = \
    "xor ecx,ecx" \
    CallGate_get_audio_codec_count  \
    parm [eax] \
    value [ecx];

#pragma aux RdosGetAudioCodecVersion = \
    CallGate_get_audio_codec_version  \
    "jnc save" \
    "xor eax,eax" \
    "xor edx,edx" \
    "save: " \
    "movzx eax,ax" \
    "movzx edx,dx" \
    "mov [esi],edx" \
    "mov [edi],eax" \
    parm [eax] [edx] [esi] [edi]   \
    value [eax] \
    modify [eax edx];

#pragma aux RdosGetAudioWidgetInfo = \
    CallGate_get_audio_widget_info  \
    "movzx eax,al" \    
    "jnc done" \
    "xor eax,eax" \
    "done: " \
    parm [eax] [edx] [ebx] [edi]    \
    value [eax];

#pragma aux RdosGetAudioWidgetConnectionList = \
    "xor ecx,ecx" \
    CallGate_get_audio_widget_connection_list  \
    parm [eax] [edx] [ebx] [edi]    \
    value [ecx];

#pragma aux RdosGetSelectedAudioConnection = \
    CallGate_get_selected_audio_connection  \
    "jnc done" \
    "xor eax,eax" \
    "done: " \
    parm [eax] [edx] [ebx]    \
    value [eax];

#pragma aux RdosGetAudioInputAmpCap = \
    CallGate_get_audio_input_amp_cap  \
    "jnc save" \
    "xor eax,eax" \
    "xor edx,edx" \
    "save: " \
    "mov [esi],eax" \
    "mov [edi],edx" \
    parm [eax] [edx] [ebx] [esi] [edi]   \
    value [eax] \
    modify [eax edx];

#pragma aux RdosGetAudioOutputAmpCap = \
    CallGate_get_audio_output_amp_cap  \
    "jnc save" \
    "xor eax,eax" \
    "xor edx,edx" \
    "save: " \
    "mov [esi],eax" \
    "mov [edi],edx" \
    parm [eax] [edx] [ebx] [esi] [edi]   \
    value [eax] \
    modify [eax edx];

#pragma aux RdosHasAudioInputMute = \
    CallGate_has_audio_input_mute  \
    CarryToBool \
    parm [eax] [edx] [ebx] \
    value [eax] \

#pragma aux RdosHasAudioOutputMute = \
    CallGate_has_audio_output_mute  \
    CarryToBool \
    parm [eax] [edx] [ebx] \
    value [eax] \

#pragma aux RdosReadAudioInputAmp = \
    CallGate_read_audio_input_amp  \
    parm [eax] [edx] [ebx] [ecx] [esi]   \
    value [eax];

#pragma aux RdosReadAudioOutputAmp = \
    CallGate_read_audio_output_amp  \
    parm [eax] [edx] [ebx] [ecx] \
    value [eax];

#pragma aux RdosIsAudioInputAmpMuted = \
    CallGate_is_audio_input_amp_muted  \
    CarryToBool \
    parm [eax] [edx] [ebx] [ecx] [esi]   \
    value [eax];

#pragma aux RdosIsAudioOutputAmpMuted = \
    CallGate_is_audio_output_amp_muted  \
    CarryToBool \
    parm [eax] [edx] [ebx] [ecx] \
    value [eax];

#pragma aux RdosGetFixedAudioOutput = \
    "push ecx" \
    CallGate_get_fixed_audio_output  \
    "jnc save" \
    "xor eax,eax" \
    "xor ecx,ecx" \
    "xor edx,edx" \
    "stc" \
    "save: " \
    "mov [esi],eax" \
    "mov [edi],edx" \
    "mov eax,ecx" \
    "pop ecx" \
    "mov [ecx],eax" \
    CarryToBool \
    parm [esi] [edi] [ecx] \
    value [eax];

#pragma aux RdosGetJackAudioOutput = \
    "push ecx" \
    CallGate_get_jack_audio_output  \
    "jnc save" \
    "xor eax,eax" \
    "xor ecx,ecx" \
    "xor edx,edx" \
    "stc" \
    "save: " \
    "mov [esi],eax" \
    "mov [edi],edx" \
    "mov eax,ecx" \
    "pop ecx" \
    "mov [ecx],eax" \
    CarryToBool \
    parm [ebx] [esi] [edi] [ecx] \
    value [eax];

#pragma aux RdosGetJackAudioInput = \
    "push ecx" \
    CallGate_get_jack_audio_input  \
    "jnc save" \
    "xor eax,eax" \
    "xor ecx,ecx" \
    "xor edx,edx" \
    "stc" \
    "save: " \
    "mov [esi],eax" \
    "mov [edi],edx" \
    "mov eax,ecx" \
    "pop ecx" \
    "mov [ecx],eax" \
    CarryToBool \
    parm [ebx] [esi] [edi] [ecx] \
    value [eax];

#pragma aux RdosSetTextMode = \
    "mov ax,3"  \
    CallGate_set_video_mode  \
    modify [ax];

#pragma aux RdosQueryVideoMode = \
    "push edx" \
    CallGate_query_video_mode \
    "movzx ecx,cx" \
    "mov [esi],ecx" \
    "movzx edx,dx" \
    "mov [edi],edx" \
    "movzx eax,ax" \
    "pop edx" \
    "mov [edx],eax" \
    CarryToBool \
    parm [eax] [edx] [esi] [edi] \
    value [eax] \
    modify [eax ecx edx esi edi];

#pragma aux RdosSetVideoMode = \
    "push ebp" \
    "push esi" \
    "push edi" \
    "push eax" \
    "mov ax,[eax]" \
    "push ecx" \
    "mov cx,[ecx]" \
    "push edx" \
    "mov dx,[edx]" \
    CallGate_get_video_mode \
    ValidateEax \
    CallGate_set_video_mode  \
    "pop ebp" \
    "movzx edx,dx" \
    "mov [ebp],edx" \
    "pop ebp" \
    "movzx ecx,cx" \
    "mov [ebp],ecx" \
    "pop ebp" \
    "movzx eax,ax" \
    "mov [ebp],eax" \
    "pop ebp" \
    "mov [ebp],edi" \
    "pop ebp" \
    "movzx esi,si" \
    "mov [ebp],esi" \
    "pop ebp" \
    ValidateHandle \
    parm [eax] [ecx] [edx] [esi] [edi] \
    value [ebx] \
    modify [eax ecx edx esi edi];

#pragma aux RdosSetClipRect = \
    CallGate_set_clip_rect  \
    parm [ebx] [ecx] [edx] [esi] [edi];

#pragma aux RdosClearClipRect = \
    CallGate_clear_clip_rect  \
    parm [ebx];

#pragma aux RdosSetDrawColor = \
    CallGate_set_drawcolor  \
    parm [ebx] [eax];

#pragma aux RdosSetLGOP = \
    CallGate_set_lgop  \
    parm [ebx] [eax];

#pragma aux RdosSetHollowStyle = \
    CallGate_set_hollow_style  \
    parm [ebx];

#pragma aux RdosSetFilledStyle = \
    CallGate_set_filled_style  \
    parm [ebx];

#pragma aux RdosAnsiToUtf8 = \
    CallGate_ansi_to_utf8  \
    parm [esi] [edi] [ecx]  \
    value [eax];

#pragma aux RdosUtf8ToAnsi = \
    CallGate_utf8_to_ansi  \
    parm [esi] [edi] [ecx]  \
    value [eax];

#pragma aux RdosAnsiToUtf16 = \
    CallGate_ansi_to_utf16  \
    parm [esi] [edi] [ecx]  \
    value [eax];

#pragma aux RdosUtf16ToAnsi = \
    CallGate_utf16_to_ansi  \
    parm [esi] [edi] [ecx]  \
    value [eax];

#pragma aux RdosOpenFont = \
    CallGate_open_font  \
    ValidateHandle \
    parm [edx] [eax]  \
    value [ebx];

#pragma aux RdosCloseFont = \
    CallGate_close_font  \
    parm [ebx];

#pragma aux RdosGetStringMetrics = \
    CallGate_get_string_metrics  \
    "movzx ecx,cx" \
    "movzx edx,dx" \
    "mov [eax],ecx" \
    "mov [esi],edx" \
    parm [ebx] [edi] [eax] [esi] \
    modify [ecx edx];

#pragma aux RdosSetFont = \
    CallGate_set_font  \
    parm [ebx] [eax];

#pragma aux RdosGetPixel = \
    CallGate_get_pixel  \
    parm [ebx] [ecx] [edx]  \
    value [eax];

#pragma aux RdosSetPixel = \
    CallGate_set_pixel  \
    parm [ebx] [ecx] [edx];

// Blit here
// DrawMask here

#pragma aux RdosDrawLine = \
    CallGate_draw_line  \
    parm [ebx] [ecx] [edx] [esi] [edi];

#pragma aux RdosDrawString = \
    CallGate_draw_string  \
    parm [ebx] [ecx] [edx] [edi];

#pragma aux RdosDrawRect = \
    CallGate_draw_rect  \
    parm [ebx] [ecx] [edx] [esi] [edi];

#pragma aux RdosDrawEllipse = \
    CallGate_draw_ellipse  \
    parm [ebx] [ecx] [edx] [esi] [edi];

#pragma aux RdosCreateBitmap = \
    CallGate_create_bitmap  \
    ValidateHandle \
    parm [eax] [ecx] [edx] \
    value [ebx];

#pragma aux RdosCreateAlphaBitmap = \
    CallGate_create_alpha_bitmap  \
    ValidateHandle \
    parm [ecx] [edx] \
    value [ebx];

#pragma aux RdosExtractValidBitmapMask = \
    CallGate_extract_valid_bitmap_mask  \
    "mov bx,ax" \
    ValidateHandle  \
    parm [ebx]  \
    value [ebx] \
    modify [ax];

#pragma aux RdosExtractInvalidBitmapMask = \
    CallGate_extract_invalid_bitmap_mask  \
    "mov bx,ax" \
    ValidateHandle  \
    parm [ebx]  \
    value [ebx] \
    modify [ax];

#pragma aux RdosExtractAlphaBitmap = \
    CallGate_extract_alpha_bitmap  \
    "mov bx,ax" \
    ValidateHandle  \
    parm [ebx]  \
    value [ebx] \
    modify [ax];

#pragma aux RdosDuplicateBitmapHandle = \
    CallGate_dup_bitmap_handle  \
    ValidateHandle  \
    parm [ebx]  \
    value [ebx];

#pragma aux RdosCloseBitmap = \
    CallGate_close_bitmap  \
    parm [ebx];

#pragma aux RdosCreateStringBitmap = \
    CallGate_create_string_bitmap  \
    ValidateHandle  \
    parm [ebx] [edi]    \
    value [ebx];


// GetBitmapInfo here

#pragma aux RdosCreateSprite = \
    CallGate_create_sprite  \
    ValidateHandle  \
    parm [ebx] [ecx] [edx] [eax]    \
    value [ebx];

#pragma aux RdosCloseSprite = \
    CallGate_close_sprite  \
    parm [ebx];

#pragma aux RdosShowSprite = \
    CallGate_show_sprite  \
    parm [ebx];

#pragma aux RdosHideSprite = \
    CallGate_hide_sprite  \
    parm [ebx];

#pragma aux RdosMoveSprite = \
    CallGate_move_sprite  \
    parm [ebx] [ecx] [edx];

#pragma aux RdosSetForeColor = \
    CallGate_set_forecolor  \
    parm [eax];

#pragma aux RdosSetBackColor = \
    CallGate_set_backcolor  \
    parm [eax];

#pragma aux RdosAllocateMem = \
    CallGate_allocate_app_mem  \
    ValidateEdx \
    parm [eax]  \
    value [edx];

#pragma aux RdosFreeMem = \
    CallGate_free_app_mem  \
    parm [edx];

#pragma aux RdosAllocateDebugMem = \
    CallGate_allocate_debug_app_mem  \
    ValidateEdx \
    parm [eax]  \
    value [edx];

#pragma aux RdosFreeDebugMem = \
    CallGate_free_debug_app_mem  \
    parm [edx];

#pragma aux RdosGetThreadLinear = \
    CallGate_get_thread_linear  \
    ValidateEdx \
    parm [ebx] [edx] [esi] \
    value [edx];

#pragma aux RdosReadThreadMem = \
    CallGate_read_thread_mem  \
    parm [ebx] [edx] [esi] [edi] [ecx] \
    value [eax];

#pragma aux RdosWriteThreadMem = \
    CallGate_write_thread_mem  \
    parm [ebx] [edx] [esi] [edi] [ecx] \
    value [eax];

#pragma aux RdosGetDebugThread = \
    CallGate_get_debug_thread  \
    ValidateEax \
    "movzx eax,ax"  \
    value [eax];

#pragma aux RdosGetThreadTss = \
    CallGate_get_thread_tss  \
    parm [ebx] [edi];

#pragma aux RdosSetThreadTss = \
    CallGate_set_thread_tss  \
    parm [ebx] [edi];

#pragma aux RdosRemoteDebug = \
    CallGate_remote_debug  \
    parm [edx];

#pragma aux RdosRemoteGui = \
    CallGate_remote_gui  \
    parm [edx];

#pragma aux RdosDebugTrace = \
    CallGate_debug_trace;

#pragma aux RdosDebugPace = \
    CallGate_debug_pace;

#pragma aux RdosDebugGo = \
    CallGate_debug_go;

#pragma aux RdosDebugRun = \
    CallGate_debug_run;

#pragma aux RdosDebugNext = \
    CallGate_debug_next;

#pragma aux RdosSetCodeBreak = \
    CallGate_set_code_break  \
    CarryToBool \
    parm [ebx] [eax] [si] [edi] \
    value [eax];

#pragma aux RdosSetReadDataBreak = \
    CallGate_set_read_data_break  \
    CarryToBool \
    parm [ebx] [eax] [si] [edi] [ecx] \
    value [eax];

#pragma aux RdosSetWriteDataBreak = \
    CallGate_set_write_data_break  \
    CarryToBool \
    parm [ebx] [eax] [si] [edi] [ecx] \
    value [eax];

#pragma aux RdosClearBreak = \
    CallGate_clear_break  \
    parm [ebx] [eax];

#pragma aux RdosHasPhysical64 = \
    CallGate_has_physical64  \
    CarryToBool \
    value [eax];

#pragma aux RdosUsesPae = \
    CallGate_uses_pae  \
    CarryToBool \
    value [eax];

#pragma aux RdosGetFreePhysical = \
    CallGate_get_free_physical  \
    value [edx eax];

#pragma aux RdosGetPhysicalEntryType = \
    CallGate_get_physical_entry_type  \
    ValidateEax \
    parm [ebx] \
    value [eax];

#pragma aux RdosGetPhysicalEntryBase = \
    CallGate_get_physical_entry_base  \
    parm [ebx] \
    value [ebx eax];

#pragma aux RdosGetPhysicalEntrySize = \
    CallGate_get_physical_entry_size  \
    parm [ebx] \
    value [ebx eax];

#pragma aux RdosGetFreeGdt = \
    CallGate_get_free_gdt  \
    "movzx eax,ax"  \
    value [eax];

#pragma aux RdosGetFreeLdt = \
    CallGate_get_free_ldt  \
    "movzx eax,ax"  \
    value [eax];

#pragma aux RdosGetFreeSmallKernelLinear = \
    CallGate_available_small_linear  \
    value [eax];

#pragma aux RdosGetFreeBigKernelLinear = \
    CallGate_available_big_linear  \
    value [eax];

#pragma aux RdosGetFreeSmallLocalLinear = \
    CallGate_available_small_local_linear  \
    value [eax];

#pragma aux RdosGetFreeBigLocalLinear = \
    CallGate_available_big_local_linear  \
    value [eax];

#pragma aux RdosGetMaxComPort = \
    "clc" \
    CallGate_get_max_com_port  \
    "jc fail" \
    "movzx eax,ax"  \
    "jmp done" \
    "fail:" \
    "xor eax,eax" \
    "done:" \        
    value [eax];

#pragma aux RdosIsComAvailable = \
    CallGate_is_com_available  \
    CarryToBool  \
    parm [al] \
    value [eax];

#pragma aux RdosGetStdComPar = \
    CallGate_get_std_com_par  \
    "jc fail" \
    "movzx eax,al" \
    "mov [ebx],eax" \
    "movzx edx,dx" \
    "mov [esi],edx" \
    "mov [edi],ecx" \
    "mov eax,1" \
    "jmp done" \
    "fail:" \
    "xor eax,eax" \
    "done:" \
    parm [al] [ebx] [esi] [edi] \
    value [eax] \
    modify [ecx edx];

#pragma aux RdosGetUsbComPar = \
    CallGate_get_usb_com_par  \
    "jc fail" \
    "movzx eax,ax" \
    "mov [esi],eax" \
    "mov eax,1" \
    "jmp done" \
    "fail:" \
    "xor eax,eax" \
    "done:" \
    parm [al] [esi] \
    value [eax];

#pragma aux RdosOpenCom = \
    CallGate_open_com  \
    ValidateHandle  \
    parm [al] [ecx] [bh] [ah] [bl] [esi] [edi]  \
    value [ebx];

#pragma aux RdosCloseCom = \
    CallGate_close_com  \
    parm [ebx];

#pragma aux RdosFlushCom = \
    CallGate_flush_com  \
    parm [ebx];

#pragma aux RdosResetCom = \
    CallGate_reset_com  \
    parm [ebx];

#pragma aux RdosReadCom = \
    CallGate_read_com  \
    parm [ebx]  \
    value [al];

#pragma aux RdosWriteCom = \
    CallGate_write_com  \
    "movsx eax,al"  \
    parm [ebx] [al] \
    value [eax];

#pragma aux RdosEnableCts = \
    CallGate_enable_cts  \
    parm [ebx];

#pragma aux RdosDisableCts = \
    CallGate_disable_cts  \
    parm [ebx];

#pragma aux RdosEnableAutoRts = \
    CallGate_enable_auto_rts  \
    parm [ebx];

#pragma aux RdosDisableAutoRts = \
    CallGate_disable_auto_rts  \
    parm [ebx];

#pragma aux RdosGetCts = \
    CallGate_get_cts  \
    "jc CtsOff" \
    "mov eax,1" \
    "jmp CtsDone" \
    "CtsOff:" \
    "xor eax,eax" \
    "CtsDone:" \
    parm [ebx] \
    value [eax];

#pragma aux RdosGetDsr = \
    CallGate_get_dsr  \
    "jc DsrOff" \
    "mov eax,1" \
    "jmp DsrDone" \
    "DsrOff:" \
    "xor eax,eax" \
    "DsrDone:" \
    parm [ebx] \
    value [eax];

#pragma aux RdosSetDtr = \
    CallGate_set_dtr  \
    parm [ebx];

#pragma aux RdosResetDtr = \
    CallGate_reset_dtr  \
    parm [ebx];

#pragma aux RdosSetRts = \
    CallGate_set_rts  \
    parm [ebx];

#pragma aux RdosResetRts = \
    CallGate_reset_rts  \
    parm [ebx];

#pragma aux RdosGetReceiveBufferSpace = \
    CallGate_get_com_receive_space  \
    ValidateEax \
    parm [ebx]  \
    value [eax];

#pragma aux RdosGetComRecCount = \
    CallGate_get_com_rec_count  \
    ValidateEax \
    parm [ebx]  \
    value [eax];

#pragma aux RdosGetSendBufferSpace = \
    CallGate_get_com_send_space  \
    ValidateEax \
    parm [ebx]  \
    value [eax];

#pragma aux RdosWaitForSendCompletedCom = \
    CallGate_wait_for_send_completed_com  \
    parm [ebx];

#pragma aux RdosGetMaxPrinters = \
    CallGate_get_max_printer  \
    "jc fail" \
    "movzx eax,al"  \
    "jmp done" \
    "fail:" \
    "xor eax,eax" \
    "done:" \        
    value [eax];

#pragma aux RdosOpenPrinter = \
    CallGate_open_printer  \
    ValidateHandle  \
    parm [al]  \
    value [ebx];

#pragma aux RdosClosePrinter = \
    CallGate_close_printer  \
    parm [ebx];

#pragma aux RdosGetPrinterName = \
    CallGate_get_printer_name  \
    CarryToBool \
    parm [ebx] [edi] \
    value [eax];

#pragma aux RdosIsPrinterJammed = \
    CallGate_is_printer_jammed  \
    "cmc" \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosIsPrinterPaperLow = \
    CallGate_is_printer_paper_low  \
    "cmc" \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosIsPrinterPaperEnd = \
    CallGate_is_printer_paper_end  \
    "cmc" \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosIsPrinterCutterJammed = \
    CallGate_is_printer_cutter_jammed  \
    "cmc" \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosIsPrinterOk = \
    CallGate_is_printer_ok  \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosIsPrinterHeadLifted = \
    CallGate_is_printer_head_lifted  \
    "cmc" \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosHasPrinterPaperInPresenter = \
    CallGate_has_printer_paper_in_presenter  \
    "cmc" \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosHasPrinterTemperatureError = \
    CallGate_has_printer_temp_error  \
    "cmc" \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosHasPrinterFeedError = \
    CallGate_has_printer_feed_error  \
    "cmc" \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosPrintTest = \
    CallGate_print_test  \
    parm [ebx];

#pragma aux RdosCreatePrinterBitmap = \
    CallGate_create_printer_bitmap  \
    "mov ebx,eax" \
    ValidateHandle \
    parm [ebx] [edx] \
    value [ebx];

#pragma aux RdosPrintBitmap = \
    CallGate_print_bitmap  \
    parm [ebx] [eax];

#pragma aux RdosPresentPrinterMedia = \
    CallGate_present_printer_media  \
    parm [ebx] [eax];

#pragma aux RdosEjectPrinterMedia = \
    CallGate_eject_printer_media  \
    parm [ebx];

#pragma aux RdosWaitForPrint = \
    CallGate_wait_for_print  \
    parm [ebx];

#pragma aux RdosResetPrinter = \
    CallGate_reset_printer  \
    parm [ebx];

#pragma aux RdosGetLonModules = \
    CallGate_get_lon_modules  \
    "jc fail" \
    "movzx eax,al"  \
    "jmp done" \
    "fail:" \
    "xor eax,eax" \
    "done:" \        
    value [eax];

#pragma aux RdosOpenLonModule = \
    CallGate_open_lon_module  \
    ValidateHandle  \
    parm [al] [esi] [edi] \
    value [ebx];

#pragma aux RdosResetLonModule = \
    CallGate_reset_lon_module  \
    parm [al];

#pragma aux RdosCloseLonModule = \
    CallGate_close_lon_module  \
    parm [ebx];

#pragma aux RdosAddWaitForLonModule = \
    CallGate_add_wait_for_lon_module  \
    parm [ebx] [eax] [ecx];

#pragma aux RdosSendLonModuleMsg = \
    CallGate_send_lon_module_msg  \
    parm [ebx] [edi] [ecx];

#pragma aux RdosHasLonModuleMsg = \
    CallGate_has_lon_module_msg  \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosReceiveLonModuleMsg = \
    CallGate_receive_lon_module_msg  \
    parm [ebx] [edi] \
    value [ecx];
    
#pragma aux RdosGetMaxCardDev = \
    CallGate_get_max_carddev  \
    "jc fail" \
    "movzx eax,al"  \
    "jmp done" \
    "fail:" \
    "xor eax,eax" \
    "done:" \        
    value [eax];

#pragma aux RdosOpenCardDev = \
    CallGate_open_carddev  \
    ValidateHandle  \
    parm [al]  \
    value [ebx];

#pragma aux RdosCloseCardDev = \
    CallGate_close_carddev  \
    parm [ebx];

#pragma aux RdosGetCardDevName = \
    CallGate_get_carddev_name  \
    CarryToBool \
    parm [ebx] [edi] \
    value [eax];

#pragma aux RdosIsCardDevOk = \
    CallGate_is_carddev_ok  \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosIsCardDevBusy = \
    CallGate_is_carddev_busy  \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosIsCardDevInserted = \
    CallGate_is_carddev_inserted  \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosHadCardDevInserted = \
    CallGate_had_carddev_inserted  \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosClearCardDevInserted = \
    CallGate_clear_carddev_inserted  \
    parm [ebx];

#pragma aux RdosWaitForCard = \
    CallGate_wait_for_card  \
    CarryToBool \
    parm [ebx] [edi] \
    value [eax];

#pragma aux RdosGetCanModuleInfo = \
    CallGate_get_can_module_info  \
    CarryToBool \
    "movzx ecx,cx" \
    "mov [esi],ecx" \
    "movzx edx,dx" \
    "mov [edi],edx" \
    parm [eax] [esi] [edi] \
    value [eax] \
    modify [ecx edx];

#pragma aux RdosCheckCanSerialPort = \
    CallGate_check_can_serial_port  \
    "mov [esi],eax" \
    "mov [edi],edx" \
    CarryToBool \
    parm [eax] [esi] [edi] \
    value [eax] \
    modify [edx];

#pragma aux RdosGetCanModuleVersion = \
    CallGate_get_can_module_version  \
    "movzx ecx,ah" \
    "mov [esi],ecx" \
    "movzx ecx,al" \
    "mov [edi],ecx" \
    "movzx ecx,dl" \
    "mov [ebx],ecx" \
    CarryToBool \
    parm [eax] [esi] [edi] [ebx] \
    value [eax] \
    modify [ecx edx];

#pragma aux RdosGetCanLoaderVersion = \
    CallGate_get_can_loader_version  \
    "movzx ecx,ah" \
    "mov [esi],ecx" \
    "movzx ecx,al" \
    "mov [edi],ecx" \
    "movzx ecx,dl" \
    "mov [ebx],ecx" \
    CarryToBool \
    parm [eax] [esi] [edi] [ebx] \
    value [eax] \
    modify [ecx edx];

#pragma aux RdosGetCanSerialNumber = \
    CallGate_get_can_serial_number  \
    CarryToBool \
    parm [eax] [edi]

#pragma aux RdosProgramCanModule = \
    CallGate_program_can_module  \
    CarryToBool \
    parm [eax] [edi] \
    value [eax];

#pragma aux RdosOpenHandle = \
    CallGate_open_handle  \
    parm [edi] [ecx] \
    value [ebx];

#pragma aux RdosCloseHandle = \
    CallGate_close_handle  \
    parm [ebx] \
    value [ebx];
    
#pragma aux RdosReadHandle = \
    CallGate_read_handle  \
    parm [ebx] [edi] [ecx]  \
    value [eax];

#pragma aux RdosWriteHandle = \
    CallGate_write_handle  \
    parm [ebx] [edi] [ecx]  \
    value [eax];

#pragma aux RdosDupHandle = \
    CallGate_dup_handle  \
    parm [ebx]  \
    value [ebx];

#pragma aux RdosDup2Handle = \
    CallGate_dup2_handle  \
    parm [ebx] [eax]  \
    value [ebx];

#pragma aux RdosGetHandleSize = \
    CallGate_get_handle_size  \
    parm [ebx]  \
    value [eax];

#pragma aux RdosSetHandleSize = \
    CallGate_set_handle_size  \
    parm [ebx] [eax] \
    value [eax];

#pragma aux RdosGetHandleMode = \
    CallGate_get_handle_mode  \
    parm [ebx]  \
    value [eax];

#pragma aux RdosSetHandleMode = \
    CallGate_set_handle_mode  \
    parm [ebx] [eax] \
    value [eax];

#pragma aux RdosGetHandlePos = \
    CallGate_get_handle_pos  \
    parm [ebx]  \
    value [eax];

#pragma aux RdosSetHandlePos = \
    CallGate_set_handle_pos  \
    parm [ebx] [eax] \
    value [eax];

#pragma aux RdosEofHandle = \
    CallGate_eof_handle  \
    parm [ebx]  \
    value [eax];

#pragma aux RdosIsHandleDevice = \
    CallGate_is_handle_device  \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosGetHandleTime = \
    CallGate_get_handle_time  \
    "mov [esi],edx" \
    "mov [edi],eax" \
    "and eax,edx" \
    parm [ebx] [esi] [edi]  \
    modify [edx] \
    value [eax];

#pragma aux RdosSetHandleTime = \
    CallGate_set_handle_time  \
    parm [ebx] [edx] [eax] \
    value [eax];

#pragma aux RdosOpenFile = \
    CallGate_open_file  \
    ValidateHandle  \
    parm [edi] [cl] \
    value [ebx];

#pragma aux RdosWaitForCanModuleProgramming = \
    CallGate_wait_for_can_module_programming  \
    "movzx edx,dx" \
    "mov [esi],edx" \
    "mov [edi],ecx" \
    parm [eax] [esi] [edi] \
    value [eax] \
    modify [edx];

#pragma aux RdosCreateFile = \
    CallGate_create_file  \
    ValidateHandle  \
    parm [edi] [ecx] \
    value [ebx];

#pragma aux RdosCloseFile = \
    CallGate_close_file  \
    parm [ebx];

#pragma aux RdosIsDevice = \
    CallGate_get_ioctl_data  \
    0x33 0xC0 0xF6 0xC6 0x80 0x74 1 0x40 \
    parm [ebx]  \
    value [eax] \
    modify [dx];

#pragma aux RdosDuplFile = \
    CallGate_dupl_file  \
    ValidateHandle  \
    parm [eax]  \
    value [ebx];

#pragma aux RdosGetFileSize = \
    CallGate_get_file_size  \
    ValidateEax \
    parm [ebx]  \
    value [eax];

#pragma aux RdosSetFileSize = \
    CallGate_set_file_size  \
    parm [ebx] [eax];
    
#pragma aux RdosGetFilePos = \
    CallGate_get_file_pos  \
    ValidateEax \
    parm [ebx]  \
    value [eax];

#pragma aux RdosSetFilePos = \
    CallGate_set_file_pos  \
    parm [ebx] [eax];
    
#pragma aux RdosReadFile = \
    CallGate_read_file  \
    ValidateEax \
    parm [ebx] [edi] [ecx]  \
    value [eax];

#pragma aux RdosWriteFile = \
    CallGate_write_file  \
    ValidateEax \
    parm [ebx] [edi] [ecx]  \
    value [eax];

#pragma aux RdosGetFileTime = \
    CallGate_get_file_time  \
    "mov [esi],edx" \
    "mov [edi],eax" \
    parm [ebx] [esi] [edi]  \
    modify [eax edx];

#pragma aux RdosSetFileTime = \
    CallGate_set_file_time  \
    parm [ebx] [edx] [eax];

#pragma aux RdosCreateMapping = \
    CallGate_create_mapping  \
    ValidateHandle  \
    parm [eax]  \
    value [ebx];

#pragma aux RdosCreateNamedMapping = \
    CallGate_create_named_mapping  \
    ValidateHandle  \
    parm [edi] [eax]  \
    value [ebx];

#pragma aux RdosCreateNamedFileMapping = \
    CallGate_create_named_file_mapping  \
    ValidateHandle  \
    parm [edi] [eax] [ebx]  \
    value [ebx];

#pragma aux RdosOpenNamedMapping = \
    CallGate_open_named_mapping  \
    ValidateHandle  \
    parm [edi] \
    value [ebx];

#pragma aux RdosSyncMapping = \
    CallGate_sync_mapping  \
    parm [ebx];

#pragma aux RdosCloseMapping = \
    CallGate_close_mapping  \
    parm [ebx];

#pragma aux RdosMapView = \
    CallGate_map_view  \
    parm [ebx] [eax] [edi] [ecx];

#pragma aux RdosUnmapView = \
    CallGate_unmap_view  \
    parm [ebx];

#pragma aux RdosSetCurDrive = \
    CallGate_set_cur_drive  \
    CarryToBool \
    parm [eax]  \
    value [eax];

#pragma aux RdosGetCurDrive = \
    CallGate_get_cur_drive  \
    "movzx eax,al"  \
    value [eax];

#pragma aux RdosGetCurDir = \
    CallGate_get_cur_dir  \
    CarryToBool \
    parm [eax] [edi]  \
    value [eax];

#pragma aux RdosSetCurDir = \
    CallGate_set_cur_dir  \
    CarryToBool \
    parm [edi]  \
    value [eax];

#pragma aux RdosMakeDir = \
    CallGate_make_dir  \
    CarryToBool \
    parm [edi]  \
    value [eax];

#pragma aux RdosRemoveDir = \
    CallGate_remove_dir  \
    CarryToBool \
    parm [edi]  \
    value [eax];

#pragma aux RdosRenameFile = \
    CallGate_rename_file  \
    CarryToBool \
    parm [edi] [esi]  \
    value [eax];

#pragma aux RdosDeleteFile = \
    CallGate_delete_file  \
    CarryToBool \
    parm [edi]  \
    value [eax];

#pragma aux RdosGetFileAttribute = \
    CallGate_get_file_attribute  \
    "movzx ecx,cx"  \
    "mov [eax],ecx" \
    CarryToBool \
    parm [edi] [eax]  \
    value [eax] \
    modify [ecx];

#pragma aux RdosSetFileAttribute = \
    CallGate_set_file_attribute  \
    CarryToBool \
    parm [edi] [ecx] \
    value [eax];

#pragma aux RdosOpenDir = \
    CallGate_open_dir  \
    ValidateHandle \
    parm [edi]  \
    value [ebx];

#pragma aux RdosCloseDir = \
    CallGate_close_dir  \
    parm [ebx];

// ReadDir here

#pragma aux RdosReadLongDir = \
    "push eax" \
    CallGate_read_dir  \
    "pop edi" \
    "jc fail" \
    "mov [esi],ecx" \
    "movzx ebx,bx" \
    "mov [edi],ebx" \
    "jmp done" \
    "fail:"\
    "mov eax,-1" \
    "mov edx,eax" \
    "done:" \
    parm [ebx] [edx] [ecx] [edi] [esi] [eax];

#pragma aux RdosDefineFaultSave = \
    CallGate_define_fault_save  \
    parm [eax] [edx] [ecx];

#pragma aux RdosClearFaultSave = \
    CallGate_clear_fault_save;

#pragma aux RdosHasCrashInfo = \
    CallGate_has_crash_info  \
    CarryToBool \
    value [eax];

#pragma aux RdosGetCrashCoreInfo = \
    CallGate_get_crash_core_info  \
    CarryToBool \
    parm [eax] [edi] \
    value [eax];

#pragma aux RdosGetImageHeader = \
    CallGate_get_image_header  \
    CarryToBool \
    parm [eax] [edx] [edi] \
    value [eax];

#pragma aux RdosGetImageData = \
    CallGate_get_image_data  \
    CarryToBool \
    parm [eax] [edx] [edi] \
    value [eax];

#pragma aux RdosGetDeviceInfo = \
    "push edx" \
    CallGate_get_device_info  \
    "mov [ecx],eax" \
    CarryToBool \
    "mov [esi],edx" \
    "pop edx" \
    "mov [edx],bx" \
    parm [ebx] [edi] [ecx] [edx] [esi] \
    modify [ebx edx] \
    value [eax];

#pragma aux RdosGetSelectorInfo = \
    CallGate_get_selector_info  \
    "mov [esi],ecx" \
    "movzx eax,al" \
    "mov [edi],eax" \
    CarryToBool \
    parm [bx] [esi] [edi] \
    modify [ecx] \
    value [eax];

#pragma aux RdosGetFaultThreadState = \
    CallGate_get_fault_thread_state  \
    CarryToBool \
    parm [eax] [edi] \
    value [eax];

#pragma aux RdosGetFaultThreadTss = \
    CallGate_get_fault_thread_tss  \
    CarryToBool \
    parm [eax] [edi] \
    value [eax];

#pragma aux RdosGetThreadCount = \
    CallGate_get_thread_count  \
    "jc def" \
    "movzx eax,ax" \
    "jmp done" \
    "def: " \
    "mov eax,256" \
    "done: " \
    value [eax];

#pragma aux RdosGetThreadState = \
    CallGate_get_thread_state  \
    CarryToBool \
    parm [eax] [edi] \
    value [eax];

#pragma aux RdosSetThreadAction = \
    CallGate_set_thread_action  \
    parm [edi]

#pragma aux RdosGetThreadActionState = \
    CallGate_get_thread_action_state  \
    CarryToBool \
    parm [eax] [edi] \
    value [eax];

#pragma aux RdosSuspendThread = \
    CallGate_suspend_thread  \
    CarryToBool \
    parm [eax] \
    value [eax];

#pragma aux RdosSuspendAndSignalThread = \
    CallGate_suspend_and_signal_thread  \
    CarryToBool \
    parm [eax] \
    value [eax];

#pragma aux RdosMoveToCore = \
    CallGate_move_to_core  \
    parm [eax];

#pragma aux RdosMoveThreadToCore = \
    CallGate_move_thread_to_core  \
    parm [eax ebx];

#pragma aux RdosGetModuleCount = \
    CallGate_get_module_count  \
    "jc fail" \
    "movzx eax,ax" \
    "jmp done" \
    "fail: " \
    "xor eax,eax" \
    "done: " \
    value [eax];

#pragma aux RdosGetModuleInfo = \
    CallGate_get_module_info  \
    CarryToBool \
    "movzx edx,dx" \
    "mov [esi],edx" \
    parm [eax] [esi] [edi] [ecx] \
    modify [edx] \
    value [eax];

#pragma aux RdosGetModuleSel = \
    CallGate_get_module_sel  \
    "jnc ok" \
    "xor eax,eax" \
    "ok: " \
    "movzx eax,ax" \
    parm [ebx] \
    value [eax];

#pragma aux RdosGetModuleBase = \
    CallGate_get_module_base  \
    "jnc ok" \
    "xor eax,eax" \
    "ok: " \
    parm [ebx] \
    value [edx eax];

#pragma aux RdosGetModuleSize = \
    CallGate_get_module_size  \
    "jnc ok" \
    "xor eax,eax" \
    "ok: " \
    parm [ebx] \
    value [edx eax];

#pragma aux RdosGetProcessCount = \
    CallGate_get_process_count  \
    "jc fail" \
    "movzx eax,ax" \
    "jmp done" \
    "fail: " \
    "xor eax,eax" \
    "done: " \
    value [eax];

#pragma aux RdosGetProcessInfo = \
    CallGate_get_process_info  \
    CarryToBool \
    "movzx edx,dx" \
    "mov [esi],edx" \
    parm [eax] [esi] [edi] [ecx] \
    modify [edx] \
    value [eax];

#pragma aux RdosGetProcessThreads = \
    CallGate_get_process_threads  \
    "jnc Ok" \
    "xor ecx,ecx" \
    "Ok: " \
    parm [ebx] [edi] [ecx] \
    value [ecx];

#pragma aux RdosIsProcessRunning = \
    CallGate_is_process_running  \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosGetProcessModules = \
    CallGate_get_process_modules  \
    "jnc Ok" \
    "xor ecx,ecx" \
    "Ok: " \
    parm [ebx] [edi] [ecx] \
    value [ecx];

#pragma aux RdosGetProcessModuleUsage = \
    CallGate_get_process_module_usage  \
    "jnc Ok" \
    "xor ecx,ecx" \
    "Ok: " \
    parm [ebx] [edx] \
    value [ecx];
    
#pragma aux RdosGetProgramCount = \
    CallGate_get_program_count  \
    "jc fail" \
    "movzx eax,ax" \
    "jmp done" \
    "fail: " \
    "xor eax,eax" \
    "done: " \
    value [eax];

#pragma aux RdosGetProgramInfo = \
    CallGate_get_program_info  \
    CarryToBool \
    "movzx edx,dx" \
    "mov [esi],edx" \
    parm [eax] [esi] [edi] [ecx] \
    modify [edx] \
    value [eax];

#pragma aux RdosGetProgramModules = \
    CallGate_get_program_modules  \
    "jnc Ok" \
    "xor ecx,ecx" \
    "Ok: " \
    parm [eax] [edi] [ecx] \
    value [ecx];

#pragma aux RdosGetProgramProcesses = \
    CallGate_get_program_processes  \
    "jnc Ok" \
    "xor ecx,ecx" \
    "Ok: " \
    parm [eax] [edi] [ecx] \
    value [ecx];

#pragma aux RdosHasHardReset = \
    CallGate_has_hard_reset \
    CarryToBool \
    value [eax];

#pragma aux RdosSoftReset = \
    CallGate_soft_reset;

#pragma aux RdosHardReset = \
    CallGate_hard_reset;

#pragma aux RdosPowerFailure = \
    CallGate_power_failure \
    "movzx eax,ax"  \
    value [eax];

#pragma aux RdosIsEmergencyStopped = \
    CallGate_is_emergency_stopped \
    CarryToBool \    
    value [eax];

#pragma aux RdosHasGlobalTimer = \
    CallGate_has_global_timer \
    CarryToBool \    
    value [eax];

#pragma aux RdosGetActiveCores = \
    CallGate_get_active_cores \
    value [eax];

#pragma aux RdosGetCoreLoad = \
    CallGate_get_core_load \
    "mov [esi],ebx" \
    "mov [esi+4],ecx" \
    "mov [edi],eax" \
    "mov [edi+4],edx" \
    CarryToBool \    
    parm [eax] [esi] [edi] \
    value [eax] \
    modify [ebx ecx edx];

#pragma aux RdosGetCoreDuty = \
    CallGate_get_core_duty \
    "mov [esi],eax" \
    "mov [esi+4],edx" \
    "mov [edi],ebx" \
    "mov [edi+4],ecx" \
    CarryToBool \    
    parm [eax] [esi] [edi] \
    value [eax] \
    modify [ebx ecx edx];

#pragma aux RdosGetVersion = \
    CallGate_get_version  \
    "movzx edx,dx"  \
    "mov [ebx],edx" \
    "movzx  eax,ax" \
    "mov [esi],eax" \
    "movzx ecx,cx"  \
    "mov [edi],ecx" \
    parm [ebx] [esi] [edi]  \
    modify [eax ecx edx];

#pragma aux RdosGetCpuVersion = \
    CallGate_get_cpu_version  \
    "movzx  eax,al" \
    "mov [esi],edx" \
    "mov [ecx],ebx" \
    parm [edi] [esi] [ecx] \
    value [eax];

#pragma aux RdosTerminateThread = \
    CallGate_terminate_thread;

#pragma aux RdosGetThreadHandle = \
    CallGate_get_thread_handle \
    "movzx eax,ax"  \
    value [eax];

#pragma aux RdosGetProcessHandle = \
    CallGate_get_process_handle \
    "movzx eax,ax"  \
    value [eax];

#pragma aux RdosIs64BitExe = \
    CallGate_is_64_bit_exe  \
    CarryToBool \    
    parm [esi] \
    value [eax];

#pragma aux RdosFork = \
    CallGate_fork  \
    "movzx eax,ax"  \
    value [eax];

#pragma aux RdosUnloadExe = \
    CallGate_unload_exe  \
    parm [eax];

#pragma aux RdosFatalErrorExit = \
    CallGate_fatal_error_exit;

#pragma aux RdosGetExitCode = \
    CallGate_get_exit_code  \
    "movsx eax,ax" \
    value [eax];

#pragma aux RdosAddWaitForProcessEnd = \
    CallGate_add_wait_for_proc_end  \
    parm [ebx] [eax] [ecx];

#pragma aux RdosGetProcessExitCode = \
    CallGate_get_proc_exit_code  \
    "movsx eax,ax" \
    parm [ebx]  \
    value [eax];

#pragma aux RdosShowExceptionText = \
    CallGate_show_exception_text  \
    value [eax];

#pragma aux RdosWaitMilli = \
    CallGate_wait_milli  \
    parm [eax];

#pragma aux RdosWaitMicro = \
    CallGate_wait_micro  \
    parm [eax];

#pragma aux RdosWaitUntil = \
    CallGate_wait_until  \
    parm [edx] [eax];

#pragma aux RdosGetSysTime = \
    CallGate_get_system_time  \
    "mov [esi],edx" \
    "mov [edi],eax" \
    parm [esi] [edi] \
    modify [eax edx];

#pragma aux RdosGetLongSysTime = \
    CallGate_get_system_time  \
    value [edx eax];

#pragma aux RdosGetTime = \
    CallGate_get_time  \
    "mov [esi],edx" \
    "mov [edi],eax" \
    parm [esi] [edi] \
    modify [eax edx];

#pragma aux RdosGetLongTime = \
    CallGate_get_time  \
    value [edx eax];

#pragma aux RdosSetTime = \
    CallGate_get_system_time  \
    "sub esi,eax" \
    "sbb edi,edx" \
    "mov eax,esi" \
    "mov edx,edi" \
    CallGate_update_time  \
    parm [edi] [esi] \
    modify [eax edx esi edi];

#pragma aux RdosDayOfWeek = \
    "mov ch,al" \
    "xor ebx,ebx"   \
    "xor ah,ah" \
    CallGate_adjust_time  \
    "push edx"   \
    "mov eax,365"   \
    "imul dx"   \
    "push dx"   \
    "push ax"   \
    "pop ebx"   \
    "pop edx"   \
    CallGate_passed_days  \
    "dec edx"   \
    "shr edx,2" \
    "inc edx"   \
    "add ax,dx" \
    "add eax,ebx"   \
    "xor edx,edx"   \
    "add eax,5" \
    "mov ebx,7" \
    "div ebx"   \
    "movzx eax,dl" \
    parm [edx] [eax] [ecx] \
    value [eax] \
    modify [ebx ecx edx];

#pragma aux RdosDosTimeDateToTics = \
    "push ebx" \
    "push ecx" \
    "mov dx,si" \
    "mov ax,dx" \
    "shr dx,9" \
    "add dx,1980" \
    "mov cx,ax" \
    "shr cx,5" \
    "mov ch,cl" \
    "and ch,0Fh" \
    "mov cl,al" \
    "and cl,1Fh" \
    "mov bx,di" \
    "mov ax,bx" \
    "shr bx,11" \
    "mov bh,bl" \
    "shr ax,5" \
    "and al,3Fh" \
    "mov bl,al" \
    "mov ax,di" \
    "mov ah,al" \
    "add ah,ah" \
    "and ah,3Fh" \
    CallGate_time_to_binary  \
    "pop ecx" \
    "pop ebx" \
    "mov [ebx],edx" \
    "mov [ecx],eax" \
    parm [si] [di] [ebx] [ecx] \
    modify [eax edx];

#pragma aux RdosTicsToDosTimeDate = \
    CallGate_binary_to_time  \
    "shl cl,3" \
    "shr cx,3" \
    "sub dx,1980" \
    "mov dh,dl" \
    "shl dh,1" \
    "xor dl,dl" \
    "or dx,cx" \
    "mov al,ah" \
    "shr al,1" \
    "shl bl,2" \
    "shl bx,3" \
    "or bl,al" \
    "mov [esi],dx" \
    "mov [edi],bx" \
    parm [edx] [eax] [esi] [edi] \
    modify [eax ebx ecx edx];

#pragma aux RdosDecodeMsbTics = \
    "push ebx" \
    "push ecx" \
    "xor eax,eax" \
    CallGate_binary_to_time  \
    "movzx eax,bh" \
    "mov [edi],eax" \
    "movzx eax,cl" \
    "mov [esi],eax" \
    "movzx eax,ch" \
    "pop ecx" \
    "pop ebx" \
    "mov [ecx],eax" \
    "movzx eax,dx" \
    "mov [ebx],eax" \
    parm [edx] [ebx] [ecx] [esi] [edi] \
    modify [eax edx];

#pragma aux RdosDecodeLsbTics = \
    "mov edx,60" \
    "mul edx" \
    "mov [ebx],edx" \
    "mov edx,60" \
    "mul edx" \
    "mov [ecx],edx" \
    "mov edx,1000" \
    "mul edx" \
    "mov [esi],edx" \
    "mov edx,1000" \
    "mul edx" \
    "mov [edi],edx" \
    parm [eax] [ebx] [ecx] [esi] [edi] \
    modify [eax edx];

#pragma aux RdosCodeMsbTics = \
    "mov ch,al" \
    "mov bh,bl" \
    "xor bl,bl" \
    "xor ah,ah" \
    CallGate_time_to_binary  \
    parm [edx] [eax] [ecx] [ebx] \
    value [edx] \
    modify [eax bx cx];

#pragma aux RdosCodeLsbTics = \
    "xor dx,dx" \
    "xor cx,cx" \
    "xor bh,bh" \
    "mov ah,al" \
    CallGate_time_to_binary  \
    "mov ebx,eax" \
    "mov eax,1193046" \
    "mul esi" \
    "mov ecx,eax" \
    "mov eax,1193" \
    "mul edi" \
    "add eax,ecx" \
    "xor edx,edx" \
    "mov ecx,1000" \
    "div ecx" \
    "add eax,ebx" \
    parm [ebx] [eax] [esi] [edi] \
    value [eax] \
    modify [ebx ecx edx];

#pragma aux RdosAddTics = \
    "add [esi],eax" \
    "adc [edi],0"  \
    parm [edi] [esi] [eax];

#pragma aux RdosAddMicro = \
    "mov edx,1193"  \
    "imul edx"  \
    "xor edx,edx" \
    "mov ebx,1000" \
    "idiv ebx" \
    "add [esi],eax" \
    "adc dword ptr [edi],0"  \
    parm [edi] [esi] [eax] \
    modify [eax ebx edx];

#pragma aux RdosAddMilli = \
    "mov edx,1193"  \
    "imul edx"  \
    "add [esi],eax" \
    "adc [edi],edx"  \
    parm [edi] [esi] [eax] \
    modify [eax edx];

#pragma aux RdosAddSec = \
    "mov edx,1193046"  \
    "imul edx"  \
    "add [esi],eax" \
    "adc [edi],edx"  \
    parm [edi] [esi] [eax] \
    modify [eax edx];

#pragma aux RdosAddMin = \
    "mov edx,1193046*60"  \
    "imul edx"  \
    "add [esi],eax" \
    "adc [edi],edx"  \
    parm [edi] [esi] [eax] \
    modify [eax edx];

#pragma aux RdosAddHour = \
    "add [edi],eax"  \
        parm [edi] [esi] [eax];

#pragma aux RdosAddDay = \
    "mov edx,24"  \
    "imul edx"  \
    "add [edi],eax"  \
    parm [edi] [esi] [eax] \
    modify [eax edx];

#pragma aux RdosSyncTime = \
    CallGate_sync_time  \
    CarryToBool \
    parm [edx] \
    value [eax];

#pragma aux RdosCreateSection = \
    CallGate_create_named_user_section  \
    "jnc Validate" \
    CallGate_create_user_section  \
    "Validate:" \    
    ValidateHandle  \
    parm [edi] \
    value [ebx];

#pragma aux RdosDeleteSection = \
    CallGate_delete_user_section  \
    parm [ebx];

#pragma aux RdosEnterSection = \
    CallGate_enter_user_section  \
    parm [ebx];

#pragma aux RdosLeaveSection = \
    CallGate_leave_user_section  \
    parm [ebx];

#pragma aux RdosGetFreeHandles = \
    CallGate_get_free_handles  \
    "movzx eax,ax"  \
    value [eax];

#pragma aux RdosGetFreeHandleMem = \
    CallGate_get_free_handle_mem  \
    value [eax];

#pragma aux RdosCreateWait = \
    CallGate_create_wait  \
    ValidateHandle  \
    value [ebx];

#pragma aux RdosCloseWait = \
    CallGate_close_wait  \
    parm [ebx];

#pragma aux RdosCheckWait = \
    CallGate_is_wait_idle  \
    parm [ebx] \
    value [ecx];

#pragma aux RdosWaitForever = \
    CallGate_wait_no_timeout  \
    ValidateEcx \
    parm [ebx] \
    value [ecx]

#pragma aux RdosWaitTimeout = \
    "mov edx,1193" \
    "mul edx"   \
    "push edx"  \
    "push eax"  \
    CallGate_get_system_time \
    "pop ecx"   \
    "add eax,ecx"   \
    "pop ecx"   \
    "adc edx,ecx"   \
    CallGate_wait_timeout  \
    ValidateEcx \
    parm [ebx] [eax] \
    value [ecx] \
    modify [eax edx];

#pragma aux RdosWaitUntilTimeout = \
    CallGate_wait_timeout  \
    ValidateEcx \
    parm [ebx] [edx] [eax] \
    value [ecx];

#pragma aux RdosStopWait = \
    CallGate_stop_wait  \
    parm [ebx];

#pragma aux RdosRemoveWait = \
    CallGate_remove_wait  \
    parm [ebx] [ecx];

#pragma aux RdosAddWaitForKeyboard = \
    CallGate_add_wait_for_keyboard  \
    parm [ebx] [ecx];

#pragma aux RdosAddWaitForMouse = \
    CallGate_add_wait_for_mouse  \
    parm [ebx] [ecx];

#pragma aux RdosAddWaitForCom = \
    CallGate_add_wait_for_com  \
    parm [ebx] [eax] [ecx];

#pragma aux RdosAddWaitForSyslog = \
    CallGate_add_wait_for_syslog  \
    parm [ebx] [eax] [ecx];

#pragma aux RdosAddWaitForAdc = \
    CallGate_add_wait_for_adc  \
    parm [ebx] [eax] [ecx];

#pragma aux RdosCreateSignal = \
    CallGate_create_signal  \
    ValidateHandle  \
    value [ebx];

#pragma aux RdosResetSignal = \
    CallGate_reset_signal  \
    parm [ebx];

#pragma aux RdosSetSignal = \
    CallGate_set_signal  \
    parm [ebx];

#pragma aux RdosIsSignalled = \
    CallGate_is_signalled  \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosFreeSignal = \
    CallGate_free_signal  \
    parm [ebx];

#pragma aux RdosAddWaitForSignal = \
    CallGate_add_wait_for_signal  \
    parm [ebx] [eax] [ecx];

#pragma aux RdosGetDhcpEntry = \
    CallGate_get_dhcp_entry  \
    "jnc save" \
    "xor eax,eax" \
    "xor edx,edx" \
    "xor ebx,ebx" \
    "save:" \
    "mov [esi],edx" \
    "mov [edi],eax" \
    parm [eax] [esi] [edi] \
    value [ebx] \
    modify [edx eax];

#pragma aux RdosGetIp = \
    CallGate_get_ip_address  \
    value [edx];

#pragma aux RdosGetGateway = \
    CallGate_get_gateway  \
    value [edx];

#pragma aux RdosNameToIp = \
    CallGate_name_to_ip  \
    parm [edi] \
    value [edx];

#pragma aux RdosIpToName = \
    CallGate_ip_to_name  \
    parm [edx] [edi] [ecx] \
    value [eax];

#pragma aux RdosSendUdp = \
    CallGate_send_udp  \
    parm [edx] [esi] [ebx] [edi] [ecx];

#pragma aux RdosBroadcastUdp = \
    CallGate_broadcast_udp  \
    parm [esi] [ebx] [edi] [ecx];

#pragma aux RdosBroadcastQueryUdp = \
    "push eax" \
    CallGate_broadcast_query_udp  \
    ValidateEdx \
    "pop esi" \
    "mov [esi],eax" \
    "mov eax,edx" \
    parm [esi] [ecx] [edi] [eax] [ebx] [edx] \
    value [eax] \
    modify [edx esi];

#pragma aux RdosOpenUdpConnection = \
    CallGate_open_udp_connection  \
    ValidateHandle \
    parm [edx] [esi] [edi] \
    value [ebx];

#pragma aux RdosCloseUdpConnection = \
    CallGate_close_udp_connection  \
    parm [ebx];

#pragma aux RdosSendUdpConnection = \
    CallGate_send_udp_connection  \
    parm [ebx] [edi] [ecx];

#pragma aux RdosPeekUdpConnection = \
    CallGate_peek_udp_connection  \
    ValidateEcx \
    parm [ebx] \
    value [ecx];

#pragma aux RdosReadUdpConnection = \
    CallGate_read_udp_connection  \
    ValidateEax \
    parm [ebx] [edi] [ecx] \
    value [eax];

#pragma aux RdosAddWaitForUdpConnection = \
    CallGate_add_wait_for_udp_connection  \
    parm [ebx] [eax] [ecx];

#pragma aux RdosCreateTcpListen = \
    CallGate_create_tcp_listen  \
    ValidateHandle \
    parm [esi] [eax] [ecx] \
    value [ebx];

#pragma aux RdosGetTcpListen = \
    CallGate_get_tcp_listen  \
    "movzx ebx,ax" \
    ValidateHandle \
    parm [ebx] \
    value [ebx] \
    modify [ax];

#pragma aux RdosCloseTcpListen = \
    CallGate_close_tcp_listen  \
    parm [ebx];

#pragma aux RdosAddWaitForTcpListen = \
    CallGate_add_wait_for_tcp_listen  \
    parm [ebx] [eax] [ecx];

#pragma aux RdosCreateUdpListen = \
    CallGate_create_udp_listen  \
    ValidateHandle \
    parm [esi] [eax] \
    value [ebx];

#pragma aux RdosGetUdpListenSize = \
    CallGate_get_udp_listen_size  \
    ValidateEax \
    parm [ebx] \
    value [eax];

#pragma aux RdosGetUdpListenIp = \
    CallGate_get_udp_listen_ip  \
    ValidateEax \
    parm [ebx] \
    value [eax];

#pragma aux RdosGetUdpListenPort = \
    CallGate_get_udp_listen_port  \
    "movzx eax,ax" \
    ValidateEax \
    parm [ebx] \
    value [eax];

#pragma aux RdosGetUdpListenData = \
    CallGate_get_udp_listen_data  \
    ValidateEax \
    parm [ebx] [edi] [ecx] \
    value [eax];

#pragma aux RdosClearUdpListen = \
    CallGate_clear_udp_listen  \
    parm [ebx];

#pragma aux RdosCloseUdpListen = \
    CallGate_close_udp_listen  \
    parm [ebx];

#pragma aux RdosAddWaitForUdpListen = \
    CallGate_add_wait_for_udp_listen  \
    parm [ebx] [eax] [ecx];

#pragma aux RdosOpenTcpConnection = \
    CallGate_open_tcp_connection  \
    ValidateHandle \
    parm [edx] [esi] [edi] [eax] [ecx] \
    value [ebx];

#pragma aux RdosWaitForTcpConnection = \
    CallGate_wait_for_tcp_connection  \
    CarryToBool \
    parm [ebx] [eax] \
    value [eax];

#pragma aux RdosAddWaitForTcpConnection = \
    CallGate_add_wait_for_tcp_connection  \
    parm [ebx] [eax] [ecx];

#pragma aux RdosCloseTcpConnection = \
    CallGate_close_tcp_connection  \
    parm [ebx];

#pragma aux RdosDeleteTcpConnection = \
    CallGate_delete_tcp_connection  \
    parm [ebx];

#pragma aux RdosAbortTcpConnection = \
    CallGate_abort_tcp_connection  \
    parm [ebx];

#pragma aux RdosPushTcpConnection = \
    CallGate_push_tcp_connection  \
    parm [ebx];

#pragma aux RdosIsTcpConnectionClosed = \
    CallGate_is_tcp_connection_closed  \
    "cmc"   \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosIsTcpConnectionIdle = \
    CallGate_is_tcp_connection_idle  \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosGetRemoteTcpConnectionIP = \
    CallGate_get_remote_tcp_connection_ip  \
    ValidateEax \
    parm [ebx] \
    value [eax];

#pragma aux RdosGetRemoteTcpConnectionPort = \
    CallGate_get_remote_tcp_connection_port  \
    "movzx eax,ax"\
    ValidateEax \
    parm [ebx] \
    value [eax];

#pragma aux RdosGetLocalTcpConnectionPort = \
    CallGate_get_local_tcp_connection_port  \
    "movzx eax,ax"\
    ValidateEax \
    parm [ebx] \
    value [eax];

#pragma aux RdosReadTcpConnection = \
    CallGate_read_tcp_connection  \
    ValidateEax \
    parm [ebx] [edi] [ecx] \
    value [eax];

#pragma aux RdosWriteTcpConnection = \
    CallGate_write_tcp_connection  \
    ValidateEax \
    parm [ebx] [edi] [ecx] \
    value [eax];

#pragma aux RdosPollTcpConnection = \
    CallGate_poll_tcp_connection  \
    ValidateEax \
    parm [ebx] \
    value [eax];

#pragma aux RdosCreateTcpSocket = \
    CallGate_create_tcp_socket  \
    ValidateHandle \
    value [ebx];

#pragma aux RdosCreateUdpSocket = \
    CallGate_create_udp_socket  \
    ValidateHandle \
    value [ebx];

#pragma aux RdosIsIpv4Socket = \
    CallGate_is_ipv4_socket  \
    ValidateEax \
    parm [ebx] \
    value [eax];

#pragma aux RdosConnectIpv4Socket = \
    CallGate_connect_ipv4_socket  \
    ValidateEax \
    parm [ebx] [edx] [si] \
    value [eax];

#pragma aux RdosBindIpv4Socket = \
    CallGate_bind_ipv4_socket  \
    ValidateEax \
    parm [ebx] [si] \
    value [eax];

#pragma aux RdosListenSocket = \
    CallGate_listen_socket  \
    ValidateEax \
    parm [ebx] [ecx] \
    value [eax];

#pragma aux RdosAcceptIpv4Socket = \
    CallGate_accept_ipv4_socket  \
    "mov [esi],edx" \
    "mov [edi],ax" \
    ValidateEax \
    parm [esi] [edi] \
    value [eax];

#pragma aux RdosGetLocalMailslot = \
    CallGate_get_local_mailslot  \
    ValidateHandle \
    parm [edi] \
    value [ebx];

#pragma aux RdosGetRemoteMailslot = \
    CallGate_get_remote_mailslot  \
    ValidateHandle \
    parm [edx] [edi] \
    value [ebx];

#pragma aux RdosFreeMailslot = \
    CallGate_free_mailslot  \
    parm [ebx];

#pragma aux RdosSendMailslot = \
    CallGate_send_mailslot  \
    ValidateEcx \
    parm [ebx] [esi] [ecx] [edi] [eax] \
    value [ecx];

#pragma aux RdosDefineMailslot = \
    CallGate_define_mailslot  \
    parm [edi] [ecx];

#pragma aux RdosReceiveMailslot = \
    CallGate_receive_mailslot  \
    parm [edi] \
    value [ecx];

#pragma aux RdosReplyMailslot = \
    CallGate_reply_mailslot  \
    parm [edi] [ecx];

#pragma aux RdosSetFocus = \
    CallGate_set_focus  \
    parm [al];

#pragma aux RdosSetKeyMap = \
    CallGate_set_key_layout \
    CarryToBool \
    parm [edi] \
    value [eax];

#pragma aux RdosGetKeyMap = \
    CallGate_get_key_layout \
    parm [edi];

#pragma aux RdosClearKeyboard = \
    CallGate_flush_keyboard;

#pragma aux RdosPollKeyboard = \
    CallGate_poll_keyboard  \
    CarryToBool \
    value [eax];

#pragma aux RdosReadKeyboard = \
    CallGate_read_keyboard  \
    "movzx eax,ax" \
    value [eax];

#pragma aux RdosGetKeyboardState = \
    CallGate_get_keyboard_state  \
    "movzx eax,ax" \
    value [eax];

#pragma aux RdosPutKeyboard = \
    "mov dh,cl" \
    CallGate_put_keyboard_code  \
    parm [eax] [edx] [ecx] \
    modify [dh];

#pragma aux RdosPeekKeyEvent = \
    "push ecx" \
    CallGate_peek_key_event  \
    "movzx eax,ax" \
    "mov [ebx],eax" \
    "movzx eax,cx" \
    "pop ecx" \
    "mov [ecx],eax" \
    "movzx eax,dl" \
    "mov [esi],eax" \
    "movzx eax,dh" \
    "mov [edi],eax" \
    CarryToBool \
    parm [ebx] [ecx] [esi] [edi] \
    value [eax] \
    modify [dx];

#pragma aux RdosReadKeyEvent = \
    "push ecx" \
    CallGate_read_key_event  \
    "movzx eax,ax" \
    "mov [ebx],eax" \
    "movzx eax,cx" \
    "pop ecx" \
    "mov [ecx],eax" \
    "movzx eax,dl" \
    "mov [esi],eax" \
    "movzx eax,dh" \
    "mov [edi],eax" \
    CarryToBool \
    parm [ebx] [ecx] [esi] [edi] \
    value [eax] \
    modify [dx];

#pragma aux RdosHideMouse = \
    CallGate_hide_mouse;

#pragma aux RdosShowMouse = \
    CallGate_show_mouse;

#pragma aux RdosGetMousePosition = \
    CallGate_get_mouse_position \
    "movzx ecx,cx" \
    "mov [esi],ecx" \
    "movzx edx,dx" \
    "mov [edi],edx" \
    parm [esi] [edi] \
    modify [ecx edx];

#pragma aux RdosSetMousePosition = \
    CallGate_set_mouse_position \
    parm [ecx] [edx];

#pragma aux RdosSetMouseWindow = \
    CallGate_set_mouse_window \
    parm [eax] [ebx] [ecx] [edx];

#pragma aux RdosSetMouseMickey = \
    CallGate_set_mouse_mickey \
    parm [ecx] [edx];

#pragma aux RdosGetLeftButton = \
    CallGate_get_left_button \
    CarryToBool \
    value [eax];

#pragma aux RdosGetRightButton = \
    CallGate_get_right_button \
    CarryToBool \
    value [eax];

#pragma aux RdosGetLeftButtonPressPosition = \
    CallGate_get_left_button_press_position \
    "movzx ecx,cx" \
    "mov [esi],ecx" \
    "movzx edx,dx" \
    "mov [edi],edx" \
    parm [esi] [edi] \
    modify [ecx edx];

#pragma aux RdosGetRightButtonPressPosition = \
    CallGate_get_right_button_press_position \
    "movzx ecx,cx" \
    "mov [esi],ecx" \
    "movzx edx,dx" \
    "mov [edi],edx" \
    parm [esi] [edi] \
    modify [ecx edx];

#pragma aux RdosGetLeftButtonReleasePosition = \
    CallGate_get_left_button_release_position \
    "movzx ecx,cx" \
    "mov [esi],ecx" \
    "movzx edx,dx" \
    "mov [edi],edx" \
    parm [esi] [edi] \
    modify [ecx edx];

#pragma aux RdosGetRightButtonReleasePosition = \
    CallGate_get_right_button_release_position \
    "movzx ecx,cx" \
    "mov [esi],ecx" \
    "movzx edx,dx" \
    "mov [edi],edx" \
    parm [esi] [edi] \
    modify [ecx edx];

#pragma aux RdosClearText = \
    CallGate_clear_text;

#pragma aux RdosGetTextSize = \
    CallGate_get_text_size \
    "movzx ecx,cx" \
    "mov [esi],ecx" \
    "movzx edx,dx" \
    "mov [edi],edx" \
    parm [edi] [esi] \
    modify [ecx edx];

#pragma aux RdosGetCursorPosition = \
    CallGate_get_cursor_position \
    "movzx ecx,cx" \
    "mov [esi],ecx" \
    "movzx edx,dx" \
    "mov [edi],edx" \
    parm [edi] [esi] \
    modify [ecx edx];

#pragma aux RdosSetCursorPosition = \
    CallGate_set_cursor_position \
    parm [edx] [ecx];

#pragma aux RdosGetConsoleCursorPosition = \
    CallGate_get_console_cursor_position \
    "movzx ecx,cx" \
    "mov [esi],ecx" \
    "movzx edx,dx" \
    "mov [edi],edx" \
    parm [edi] [esi] \
    modify [ecx edx];

#pragma aux RdosSetConsoleCursorPosition = \
    CallGate_set_console_cursor_position \
    parm [edx] [ecx];

#pragma aux RdosWriteChar = \
    CallGate_write_char \
    parm [al];

#pragma aux RdosWriteSizeString = \
    CallGate_write_size_string \
    parm [edi] [ecx];

#pragma aux RdosWriteAttributeString = \
    CallGate_write_attrib_string \
    parm [edx] [eax] [edi] [ecx];

#pragma aux RdosWriteString = \
    CallGate_write_asciiz  \
    parm [edi];

#pragma aux RdosPing = \
    CallGate_ping  \
    CarryToBool \
    parm [edx] [eax] \
    value [eax];

#pragma aux RdosGetIdeDisc = \
    CallGate_get_ide_disc  \
    ValidateDisc \
    parm [ebx] \
    value [eax];

#pragma aux RdosGetFloppyDisc = \
    CallGate_get_floppy_disc  \
    ValidateDisc \
    parm [ebx] \
    value [eax];

#pragma aux RdosSetDiscInfo = \
    CallGate_set_disc_info  \
    CarryToBool \
    parm [eax] [ecx] [edx] [esi] [edi] \
    value [eax];

#pragma aux RdosGetDiscInfo = \
    "push edi" \
    "push esi" \
    "push edx" \
    "push ecx" \
    "push eax" \
    CallGate_get_disc_info  \
    "jnc UseNew"\
    "UseOld:"\
    "pop eax" \
    CallGate_get_old_disc_info  \
    "pop ebx"\
    "movzx ecx,cx"\
    "mov [ebx],ecx"\
    "pop ebx"\
    "mov [ebx],edx"\
    "mov dword ptr [ebx+4],0"\
    "jmp Common"\
    "UseNew:"\
    "pop ebx" \
    "pop ebx" \
    "movzx ecx,cx" \
    "mov [ebx],ecx" \
    "pop ebx" \
    "mov [ebx],eax" \
    "mov [ebx+4],edx" \
    "Common:"\
    "pop ebx" \
    "movzx esi,si" \
    "mov [ebx],esi" \
    "pop ebx" \
    "movzx edi,di" \
    "mov [ebx],edi" \
    CarryToBool \
    parm [eax] [ecx] [edx] [esi] [edi] \
    value [eax] \
    modify [ebx ecx edx esi edi];

#pragma aux RdosGetDiscCacheSize = \
    CallGate_get_disc_cache_size  \
    "jnc Ok"\
    "xor eax,eax"\
    "Ok:"\
    parm [eax] \
    value [eax];

#pragma aux RdosGetDiscVendorInfo = \
    CallGate_get_disc_vendor_info  \
    "jnc Ok" \
    "xor al,al" \
    "mov [edi],al" \
    "Ok:" \
    parm [eax] [edi] [ecx];

#pragma aux RdosIsDiscIdle = \
    CallGate_is_disc_idle  \
    CarryToBool \
    parm [eax]  \
    value [eax];

#pragma aux RdosOpenDisc = \
    CallGate_open_disc  \
    parm [eax];

#pragma aux RdosCloseDisc = \
    CallGate_close_disc  \
    parm [eax];

#pragma aux RdosReadDisc = \
    CallGate_read_long_disc  \
    CarryToBool \
    parm [ebx] [edx eax] [edi] [ecx] \
    value [eax];

#pragma aux RdosWriteDisc = \
    CallGate_write_long_disc  \
    CarryToBool \
    parm [ebx] [edx eax] [edi] [ecx] \
    value [eax];

#pragma aux RdosGetRdfsInfo = \
    "push gs" \
    "mov ax,ds" \
    "mov gs,ax" \
    CallGate_get_rdfs_info  \
    "pop gs" \
    parm [esi] [edi] [ebx] \
    modify [ax];

#pragma aux RdosGetFileCacheSize = \
    CallGate_get_file_cache_size  \
    "jnc Ok"\
    "xor eax,eax"\
    "Ok:"\
    value [eax];

#pragma aux RdosDemandLoadDrive = \
    CallGate_demand_load_drive  \
    parm [eax];

#pragma aux RdosFormatDrive = \
    CallGate_format_drive  \
    CarryToBool \
    parm [eax] [edx] [ecx] [edi] \
    value [eax];

#pragma aux RdosAllocateFixedDrive = \
    CallGate_allocate_fixed_drive  \
    CarryToBool \
    parm [eax] \
    value [eax];

#pragma aux RdosAllocateStaticDrive = \
    CallGate_allocate_static_drive  \
    ValidateDisc \
    value [eax];

#pragma aux RdosAllocateDynamicDrive = \
    CallGate_allocate_dynamic_drive  \
    ValidateDisc \
    value [eax];

#pragma aux RdosGetDriveInfo = \
    CallGate_get_drive_info  \
    "mov [ebx],eax" \
    "movzx ecx,cx" \
    "mov [esi],ecx" \
    "mov [edi],edx" \
    CarryToBool \
    parm [eax] [ebx] [esi] [edi] \
    value [eax] \
    modify [ecx edx];

#pragma aux RdosGetDriveDiscParam = \
    CallGate_get_drive_disc_param  \
    "movzx eax,al" \
    "mov [ebx],eax" \
    "mov [esi],edx" \
    "mov [edi],ecx" \
    CarryToBool \
    parm [eax] [ebx] [esi] [edi] \
    value [eax] \
    modify [ecx edx];

#pragma aux RdosCreateFileDrive = \
    CallGate_create_file_drive  \
    CarryToBool \
    parm [eax] [ecx] [esi] [edi] \
    value [eax];

#pragma aux RdosOpenFileDrive = \
    CallGate_open_file_drive  \
    CarryToBool \
    parm [eax] [edi] \
    value [eax];

#pragma aux RdosCreateCrc = \
    CallGate_create_crc  \
    ValidateHandle \
    parm [ax] \
    value [ebx];

#pragma aux RdosCloseCrc = \
    CallGate_close_crc  \
    parm [ebx];

#pragma aux RdosCalcCrc = \
    CallGate_calc_crc  \
    parm [ebx] [ax] [edi] [ecx] \
    value [ax];

#pragma aux RdosCalcCrc32 = \
    CallGate_calc_crc32  \
    parm [eax] [edi] [ecx] \
    value [eax];
    
#pragma aux RdosGetCurrentDllHandle = \
    CallGate_get_current_dll  \
    ValidateHandle \
    value [ebx];

#pragma aux RdosGetModuleHandle = \
    "mov eax,fs:[0x24]" \
    value [eax];

#pragma aux RdosGetExeName = \
    CallGate_get_exe_name  \
    ValidateEdi \
    value [edi];

#pragma aux RdosGetCmdLine = \
    CallGate_get_cmd_line  \
    ValidateEdi \
    value [edi];

#pragma aux RdosLoadDll = \
    CallGate_load_dll  \
    ValidateHandle \
    parm [edi] \
    value [ebx];

#pragma aux RdosFreeDll = \
    CallGate_free_dll  \
    parm [ebx];

#pragma aux RdosGetModuleName = \
    CallGate_get_module_name  \
    ValidateEax \
    parm [ebx] [edi] [ecx] \
    value [eax];

// ReadResource here
// ReadBinaryResource here

#pragma aux RdosDuplModuleFileHandle = \
    CallGate_dupl_module_file_handle  \
    ValidateHandle \
    parm [ebx] \
    value [ebx];

#pragma aux RdosGetModuleProc = \
    CallGate_get_module_proc  \
    ValidateEsi \
    parm [ebx] [edi] \
    value [esi];

#pragma aux RdosGetModuleFocusKey = \
    CallGate_get_module_focus_key  \
    parm [ebx] \
    value [al];

#pragma aux RdosAddWaitForDebugEvent = \
    CallGate_add_wait_for_debug_event  \
    parm [ebx] [eax] [ecx];

#pragma aux RdosGetDebugEvent = \
    CallGate_get_debug_event  \
    "movzx eax,ax" \
    "mov [esi],eax" \
    parm [ebx] [esi] \
    value [bl] \
    modify [eax];

#pragma aux RdosGetDebugEventData = \
    CallGate_get_debug_event_data  \
    parm [ebx] [edi];

#pragma aux RdosClearDebugEvent = \
    CallGate_clear_debug_event  \
    parm [ebx];

#pragma aux RdosAbortDebug = \
    CallGate_abort_debug  \
    parm [ebx];

#pragma aux RdosContinueDebugEvent = \
    CallGate_continue_debug_event  \
    parm [ebx] [eax];

#pragma aux RdosOpenSyslog = \
    CallGate_open_syslog  \
    ValidateHandle \
    value [ebx];

#pragma aux RdosCloseSyslog = \
    CallGate_close_syslog  \
    parm [ebx];

#pragma aux RdosGetSyslog = \
    "push edi" \
    "push esi" \
    "push edx" \
    "push eax" \
    CallGate_get_syslog  \
    "pop edi" \
    "mov [edi],eax" \
    "pop edi" \
    "mov [edi],edx" \
    "pop edi" \
    "movzx eax,si" \
    "xor ah,ah" \
    "mov [edi],eax" \
    "movzx eax,si" \
    "mov al,ah" \
    "xor ah,ah" \        
    parm [ebx] [esi] [edx] [eax] [edi] [ecx] \
    value [eax];

#pragma aux RdosOpenAdc = \
    CallGate_open_adc  \
    ValidateHandle \
    parm [eax] \
    value [ebx];

#pragma aux RdosCloseAdc = \
    CallGate_close_adc  \
    parm [ebx];

#pragma aux RdosDefineAdcTime = \
    CallGate_define_adc_time  \
    parm [ebx] [edx] [eax];

#pragma aux RdosReadAdc = \
    CallGate_read_adc  \
    parm [ebx] \
    value [eax];

#pragma aux RdosReadSerialLines = \
    "mov dh,cl" \
    CallGate_read_serial_lines  \
    "movzx eax,al" \
    "mov [esi],eax" \
    CarryToBool \
    parm [ecx] [esi] \
    value [eax] \
    modify [dh];

#pragma aux RdosToggleSerialLine = \
    "mov dh,cl" \
    CallGate_toggle_serial_line  \
    CarryToBool \
    parm [ecx] [edx] \
    value [eax] \
    modify [dh];

#pragma aux RdosSetSerialLine = \
    "mov dh,cl" \
    CallGate_set_serial_line  \
    CarryToBool \
    parm [ecx] [edx] \
    value [eax] \
    modify [dh];

#pragma aux RdosResetSerialLine = \
    "mov dh,cl" \
    CallGate_reset_serial_line  \
    CarryToBool \
    parm [ecx] [edx] \
    value [eax] \
    modify [dh];

#pragma aux RdosReadSerialVal = \
    "mov dh,cl" \
    CallGate_read_serial_val  \
    "pushf" \
    "shl eax,8" \
    "mov [esi],eax" \
    "popf" \
    CarryToBool \
    parm [ecx] [edx] [esi] \
    value [eax] \
    modify [dh];

#pragma aux RdosWriteSerialVal = \
    "mov dh,cl" \
    "sar eax,8" \
    CallGate_write_serial_val  \
    CarryToBool \
    parm [ecx] [edx] [eax] \
    value [eax] \
    modify [dh];

#pragma aux RdosReadSerialRaw = \
    "mov dh,cl" \
    CallGate_read_serial_val  \
    "jc readfailed" \
    "test eax,800000h" \
    "jz readsignok" \
    "or eax,0FF000000h" \
    "readsignok:" \
    "mov [esi],eax" \
    "mov eax,1" \
    "jmp readdone" \
    "readfailed:" \
    "xor eax,eax" \
    "readdone:" \    
    parm [ecx] [edx] [esi] \
    value [eax] \
    modify [dh];

#pragma aux RdosWriteSerialRaw = \
    "mov dh,cl" \
    CallGate_write_serial_val  \
    CarryToBool \
    parm [ecx] [edx] [eax] \
    value [eax] \
    modify [dh];

#pragma aux RdosOpenSysEnv = \
    CallGate_open_sys_env \
    ValidateHandle \
    value [ebx];

#pragma aux RdosOpenProcessEnv = \
    CallGate_open_proc_env \
    ValidateHandle \
    value [ebx];

#pragma aux RdosCloseEnv = \
    CallGate_close_env \
    parm [ebx];

#pragma aux RdosAddEnvVar = \
    CallGate_add_env_var \
    parm [ebx] [esi] [edi];

#pragma aux RdosDeleteEnvVar = \
    CallGate_delete_env_var \
    parm [ebx] [esi];

#pragma aux RdosFindEnvVar = \
    CallGate_find_env_var \
    CarryToBool \
    parm [ebx] [esi] [edi] \
    value [eax];

#pragma aux RdosGetEnvSize = \
    CallGate_get_env_size \
    parm [ebx] \
    value [eax];

#pragma aux RdosGetEnvData = \
    "xor ax,ax" \
    "mov es:[edi],ax" \
    CallGate_get_env_data \
    parm [ebx] [edi] \
    modify [ax];

#pragma aux RdosSetEnvData = \
    CallGate_set_env_data \
    parm [ebx] [edi];

#pragma aux RdosOpenSysIni = \
    CallGate_open_sys_ini \
    ValidateHandle \
    value [ebx];

#pragma aux RdosOpenIni = \
    CallGate_open_ini \
    ValidateHandle \
    parm [edi] \
    value [ebx];

#pragma aux RdosCloseIni = \
    CallGate_close_ini \
    parm [ebx];

#pragma aux RdosDupIni = \
    CallGate_dup_ini \
    ValidateHandle \
    parm [ebx] \
    value [ebx];

#pragma aux RdosGotoIniSection = \
    CallGate_goto_ini_section \
    CarryToBool \
    parm [ebx] [edi] \
    value [eax];

#pragma aux RdosRemoveIniSection = \
    CallGate_remove_ini_section \
    CarryToBool \
    parm [ebx] [edi] \
    value [eax];

#pragma aux RdosReadIni = \
    CallGate_read_ini \
    CarryToBool \
    parm [ebx] [esi] [edi] [ecx] \
    value [eax];

#pragma aux RdosWriteIni = \
    CallGate_write_ini \
    CarryToBool \
    parm [ebx] [esi] [edi] \
    value [eax];

#pragma aux RdosDeleteIni = \
    CallGate_delete_ini \
    CarryToBool \
    parm [ebx] [esi] \
    value [eax];

#pragma aux RdosGotoFirstIniVar = \
    CallGate_goto_first_inivar \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosGotoNextIniVar = \
    CallGate_goto_next_inivar \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosGetCurrIniVar = \
    CallGate_get_curr_inivar \
    CarryToBool \
    parm [ebx] [edi] [ecx] \
    value [eax];

#pragma aux RdosEnableStatusLED = \
    CallGate_enable_status_led;

#pragma aux RdosDisableStatusLED = \
    CallGate_disable_status_led;

#pragma aux RdosStartWatchdog = \
    CallGate_start_watchdog \
    parm [eax];

#pragma aux RdosKickWatchdog = \
    CallGate_kick_watchdog;

#pragma aux RdosStopWatchdog = \
    CallGate_stop_watchdog;

#pragma aux RdosStartDebugger = \
    CallGate_start_debugger \
    parm [eax];

#pragma aux RdosKickDebugger = \
    CallGate_kick_debugger;

#pragma aux RdosStopDebugger = \
    CallGate_stop_debugger;

#pragma aux RdosStartNetCapture = \
    CallGate_start_net_capture \
    parm [ebx];

#pragma aux RdosStopNetCapture = \
    CallGate_stop_net_capture;

#pragma aux RdosStartCanCapture = \
    CallGate_start_can_capture \
    parm [ebx];

#pragma aux RdosStopCanCapture = \
    CallGate_stop_can_capture;

#pragma aux RdosStartLonCapture = \
    CallGate_start_lon_capture \
    parm [ebx];

#pragma aux RdosStopLonCapture = \
    CallGate_stop_lon_capture;

#pragma aux RdosIsCanOnline = \
    CallGate_is_can_online \
    ValidateEax \
    value [eax];

#pragma aux RdosGetUsbDevice = \
    CallGate_get_usb_device \
    ValidateEax \
    parm [ebx] [eax] [edi] [ecx] \
    value [eax];

#pragma aux RdosGetUsbConfig = \
    CallGate_get_usb_config \
    ValidateEax \
    parm [ebx] [eax] [edx] [edi] [ecx] \
    value [eax];

#pragma aux RdosGetUsbInterface = \
    CallGate_get_usb_interface \
    "movzx ecx,cl" \
    ValidateEdx \
    parm [ebx] [eax] [edx] \
    value [ecx];

#pragma aux RdosOpenUsbPipe = \
    CallGate_open_usb_pipe \
    ValidateHandle \
    parm [ebx] [eax] [edx] \
    value [ebx];

#pragma aux RdosCloseUsbPipe = \
    CallGate_close_usb_pipe \
    parm [ebx];

#pragma aux RdosResetUsbPipe = \
    CallGate_reset_usb_pipe \
    parm [ebx];

#pragma aux RdosAddWaitForUsbPipe = \
    CallGate_add_wait_for_usb_pipe \
    parm [ebx] [eax] [ecx];

#pragma aux RdosWriteUsbControl = \
    CallGate_write_usb_control \
    parm [ebx] [edi] [ecx];

#pragma aux RdosReqUsbData = \
    CallGate_req_usb_data \
    parm [ebx] [edi] [ecx];

#pragma aux RdosGetUsbDataSize = \
    CallGate_get_usb_data_size \
    "movzx eax,ax" \
    parm [ebx] \
    value [eax];

#pragma aux RdosWriteUsbData = \
    CallGate_write_usb_data \
    parm [ebx] [edi] [ecx];

#pragma aux RdosReqUsbStatus = \
    CallGate_req_usb_status \
    parm [ebx];

#pragma aux RdosWriteUsbStatus = \
    CallGate_write_usb_status \
    parm [ebx];

#pragma aux RdosIsUsbConnected = \
    CallGate_is_usb_connected \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosIsUsbTransactionDone = \
    CallGate_is_usb_trans_done \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosWasUsbTransactionOk = \
    CallGate_was_usb_trans_ok \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosStartOneUsbTransaction = \
    CallGate_start_one_usb_trans \
    parm [ebx];

#pragma aux RdosGetAllocatedUsbBlocks = \
    CallGate_get_allocated_usb_blocks \
    value [eax];

#pragma aux RdosGetUsbCloseCount = \
    CallGate_get_usb_close_count \
    value [eax];

#pragma aux RdosOpenHid = \
    CallGate_open_hid \
    ValidateHandle \
    parm [ebx] [eax] \
    value [ebx];

#pragma aux RdosCloseHid = \
    CallGate_close_hid \
    parm [ebx];

#pragma aux RdosGetHidPipe = \
    CallGate_get_hid_pipe \
    "jc Fail" \
    "movzx eax,al" \
    "jmp Done" \
    "Fail:" \
    "xor eax,eax" \
    "Done:" \
    parm [ebx] \
    value [eax];

#pragma aux RdosReadHid = \
    CallGate_read_hid \
    CarryToBool \
    parm [ebx] [edi] [ecx] [eax] \
    value [eax];

#pragma aux RdosWriteHid = \
    CallGate_write_hid \
    CarryToBool \
    parm [ebx] [edi] [ecx] \
    value [eax];
    
#pragma aux RdosHasICSP = \
    CallGate_has_icsp \
    CarryToBool \
    value [eax];
    
#pragma aux RdosOpenICSP = \
    CallGate_open_icsp \
    ValidateHandle \
    parm [eax] \
    value [ebx];

#pragma aux RdosCloseICSP = \
    CallGate_close_icsp \
    parm [ebx];

#pragma aux RdosResetICSP = \
    CallGate_reset_icsp \
    parm [ebx];

#pragma aux RdosWriteICSPCommand = \
    CallGate_write_icsp_cmd \
    CarryToBool \
    parm [ebx] [eax] \
    value [eax];

#pragma aux RdosWriteICSPData = \
    CallGate_write_icsp_data \
    CarryToBool \
    parm [ebx] [eax] \
    value [eax];

#pragma aux RdosReadICSPData = \
    CallGate_read_icsp_data \
    "mov [edi],eax" \
    CarryToBool \
    parm [ebx] [edi] \
    value [eax];

#pragma aux RdosHasAudio = \
    CallGate_has_audio \
    CarryToBool \
    value [eax];

#pragma aux RdosSetCodecGpio0 = \
    CallGate_set_codec_gpio0 \
    parm [eax] \
    modify [eax];

#pragma aux RdosGetOutputVolume = \
    CallGate_get_output_volume \
    "mov [esi],eax" \    
    "mov [edi],edx" \    
    parm [esi] [edi] \
    modify [eax edx];

#pragma aux RdosSetOutputVolume = \
    CallGate_set_output_volume \
    parm [eax] [edx] \

#pragma aux RdosGetMasterVolume = \
    CallGate_get_master_volume \
    "mov cx,ax" \
    "mov dl,0x7F" \
    "sub dl,al" \
    "movsx edx,dl" \ 
    "mov eax,200" \
    "imul edx" \
    "sar eax,8" \
    "mov [esi],eax" \    
    "mov dl,0x7F" \
    "sub dl,ch" \
    "movsx edx,dl" \ 
    "mov eax,200" \
    "imul edx" \
    "sar eax,8" \
    "mov [edi],eax" \    
    parm [esi] [edi] \
    modify [eax cx edx];

#pragma aux RdosSetMasterVolume = \
    "mov ecx,edx" \
    "mov esi,eax" \
    "xor edx,edx" \
    "shl eax,8" \
    "sbb edx,0" \
    "mov esi,200" \
    "idiv esi" \
    "mov bl,0x7F" \
    "sub bl,al" \
    "adc bl,0" \
    "mov eax,ecx" \
    "mov esi,eax" \
    "xor edx,edx" \
    "shl eax,8" \
    "sbb edx,0" \
    "mov esi,200" \
    "idiv esi" \
    "mov bh,0x7F" \
    "sub bh,al" \
    "adc bh,0" \
    "mov ax,bx" \
    CallGate_set_master_volume \
    parm [eax] [edx] \
    modify [eax ebx ecx edx esi];

#pragma aux RdosGetLineOutVolume = \
    CallGate_get_line_out_volume \
    "mov cx,ax" \
    "mov dl,0x7F" \
    "sub dl,al" \
    "movsx edx,dl" \ 
    "mov eax,200" \
    "imul edx" \
    "sar eax,8" \
    "mov [esi],eax" \    
    "mov dl,0x7F" \
    "sub dl,ch" \
    "movsx edx,dl" \ 
    "mov eax,200" \
    "imul edx" \
    "sar eax,8" \
    "mov [edi],eax" \    
    parm [esi] [edi] \
    modify [eax cx edx];

#pragma aux RdosSetLineOutVolume = \
    "mov ecx,edx" \
    "mov esi,eax" \
    "xor edx,edx" \
    "shl eax,8" \
    "sbb edx,0" \
    "mov esi,200" \
    "idiv esi" \
    "mov bl,0x7F" \
    "sub bl,al" \
    "adc bl,0" \
    "mov eax,ecx" \
    "mov esi,eax" \
    "xor edx,edx" \
    "shl eax,8" \
    "sbb edx,0" \
    "mov esi,200" \
    "idiv esi" \
    "mov bh,0x7F" \
    "sub bh,al" \
    "adc bh,0" \
    "mov ax,bx" \
    CallGate_set_line_out_volume \
    parm [eax] [edx] \
    modify [eax ebx ecx edx esi];

#pragma aux RdosCreateAudioOutChannel = \
    "push eax" \
    "mov eax,edx" \
    "shl eax,16" \
    "xor edx,edx" \
    "mov ebx,100" \
    "div ebx" \
    "sub eax,1" \
    "adc eax,0" \
    "mov dx,ax" \
    "pop eax" \
    CallGate_create_audio_out_channel \
    ValidateHandle \
    parm [eax] [ecx] [edx] \
    modify [ebx] \
    value [ebx];

#pragma aux RdosCloseAudioOutChannel = \
    CallGate_close_audio_out_channel \
    parm [ebx];

#pragma aux RdosWriteAudio = \
    CallGate_write_audio \
    parm [ebx] [ecx] [esi] [edi];

#pragma aux RdosOpenFm = \
    CallGate_open_fm \
    ValidateHandle \
    parm [eax] \
    value [ebx];

#pragma aux RdosCloseFm = \
    CallGate_close_fm \
    parm [ebx];

#pragma aux RdosFmWait = \
    CallGate_fm_wait \
    parm [ebx] [eax];

#pragma aux RdosCreateFmInstrument = \
    CallGate_create_fm_instrument \
    ValidateHandle \
    parm [ebx] [eax] [edx] [8087] \
    value [ebx];

#pragma aux RdosFreeFmInstrument = \
    CallGate_free_fm_instrument \
    parm [ebx];

#pragma aux RdosSetFmAttack = \
    CallGate_set_fm_attack \
    parm [ebx] [eax];

#pragma aux RdosSetFmSustain = \
    CallGate_set_fm_sustain \
    parm [ebx] [eax] [edx];

#pragma aux RdosSetFmRelease = \
    CallGate_set_fm_release \
    parm [ebx] [eax] [edx];

#pragma aux RdosPlayFmNote = \
    CallGate_play_fm_note \
    parm [ebx] [8087] [eax] [edx] [ecx];

#pragma aux RdosHasTouch = \
    CallGate_has_touch \
    CarryToBool \
    value [eax];

#pragma aux RdosResetTouchCalibrate = \
    CallGate_reset_touch_cal;

#pragma aux RdosSetTouchCalibrateDividend = \
    CallGate_set_touch_cal_dividend \
    parm [edx];

#pragma aux RdosSetTouchCalibrateX = \
    CallGate_set_touch_cal_x \
    parm [esi] [edi] [edx];

#pragma aux RdosSetTouchCalibrateY = \
    CallGate_set_touch_cal_y \
    parm [esi] [edi] [edx];

#pragma aux RdosCreateBigNum = \
    CallGate_create_bignum  \
    ValidateHandle \
    value [ebx];

#pragma aux RdosDeleteBigNum = \
    CallGate_delete_bignum  \
    parm [ebx];

#pragma aux RdosAddBigNum = \
    CallGate_add_bignum  \
    parm [ebx] [eax] \
    value [ebx];

#pragma aux RdosSubBigNum = \
    CallGate_sub_bignum  \
    parm [ebx] [eax] \
    value [ebx];

#pragma aux RdosMulBigNum = \
    CallGate_mul_bignum  \
    parm [ebx] [eax] \
    value [ebx];

#pragma aux RdosDivBigNum = \
    CallGate_div_bignum  \
    parm [ebx] [eax] \
    value [ebx];

#pragma aux RdosModBigNum = \
    CallGate_mod_bignum  \
    parm [ebx] [eax] \
    value [ebx];

#pragma aux RdosPowModBigNum = \
    CallGate_pow_mod_bignum  \
    parm [ebx] [eax] [edx] \
    value [ebx];

#pragma aux RdosLoadSignedBigNum = \
    CallGate_load_signed_bignum  \
    parm [ebx] [edi] [ecx];

#pragma aux RdosLoadUnsignedBigNum = \
    CallGate_load_unsigned_bignum  \
    parm [ebx] [edi] [ecx];

#pragma aux RdosSaveSignedBigNum = \
    CallGate_save_signed_bignum  \
    parm [ebx] [edi] [ecx];

#pragma aux RdosSaveUnsignedBigNum = \
    CallGate_save_unsigned_bignum  \
    parm [ebx] [edi] [ecx];

#pragma aux RdosLoadDecStrBigNum = \
    CallGate_load_dec_str_bignum  \
    parm [ebx] [edi];

#pragma aux RdosGetDecStrSizeBigNum = \
    CallGate_get_dec_str_size_bignum  \
    parm [ebx] \
    value [ecx];

#pragma aux RdosSaveDecStrBigNum = \
    CallGate_save_dec_str_bignum  \
    parm [ebx] [edi] [ecx];

#pragma aux RdosLoadHexStrBigNum = \
    CallGate_load_hex_str_bignum  \
    parm [ebx] [edi];

#pragma aux RdosGetHexStrSizeBigNum = \
    CallGate_get_hex_str_size_bignum  \
    parm [ebx] \
    value [ecx];

#pragma aux RdosSaveHexStrBigNum = \
    CallGate_save_hex_str_bignum  \
    parm [ebx] [edi] [ecx];

#pragma aux RdosCreateRandomBigNum = \
    CallGate_create_random_bignum  \
    ValidateHandle \
    parm [ecx] \
    value [ebx];

#pragma aux RdosAddSignedBigNum = \
    CallGate_add_signed_bignum  \
    parm [ebx] [edi] [ecx];

#pragma aux RdosAddUnsignedBigNum = \
    CallGate_add_unsigned_bignum  \
    parm [ebx] [edi] [ecx];

#pragma aux RdosSubSignedBigNum = \
    CallGate_sub_signed_bignum  \
    parm [ebx] [edi] [ecx];

#pragma aux RdosSubUnsignedBigNum = \
    CallGate_sub_unsigned_bignum  \
    parm [ebx] [edi] [ecx];

#pragma aux RdosCreateRandomOddBigNum = \
    CallGate_create_random_odd_bignum  \
    ValidateHandle \
    parm [ecx] \
    value [ebx];

#pragma aux RdosFactorPow2BigNum = \
    CallGate_factor_pow2_bignum  \
    "jc Fail" \
    "mov [esi],ecx" \
    "jmp Done" \
    "Fail:" \
    "xor ebx,ebx" \
    "Done:" \
    modify [ecx] \
    parm [ebx] [esi] \
    value [ebx];
