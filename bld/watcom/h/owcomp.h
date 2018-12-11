
/* 32-bit compact memory model (device-drivers) */

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
    CallGate_test_gate;

#pragma aux RdosLoad32 = \
    CallGate_load_device32  \

#pragma aux RdosSwapShort = \
    "xchg al,ah"    \
    __parm [__ax]   \
    __value [__ax]

#pragma aux RdosSwapLong = \
    "xchg al,ah"    \
    "rol eax,16"    \
    "xchg al,ah"    \
    __parm [__eax]  \
    __value [__eax]

#pragma aux RdosGetCharSize = \
    "mov al,1" \
    "mov ah,es:[edi]" \
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
    __parm [__es __edi] \
    __value [__eax]

#pragma aux RdosGetLongRandom = \
    CallGate_get_random  \
    __value [__eax]

#pragma aux RdosGetRandom = \
    CallGate_get_random  \
    "mul edx" \
    __parm [__edx] \
    __value [__edx] \
    __modify [__eax]

#pragma aux RdosSetClipRect = \
    CallGate_set_clip_rect  \
    __parm [__ebx] [__ecx] [__edx] [__esi] [__edi]

#pragma aux RdosClearClipRect = \
    CallGate_clear_clip_rect  \
    __parm [__ebx]

#pragma aux RdosSetDrawColor = \
    CallGate_set_drawcolor  \
    __parm [__ebx] [__eax]

#pragma aux RdosSetLGOP = \
    CallGate_set_lgop  \
    __parm [__ebx] [__eax]

#pragma aux RdosSetHollowStyle = \
    CallGate_set_hollow_style  \
    __parm [__ebx]

#pragma aux RdosSetFilledStyle = \
    CallGate_set_filled_style  \
    __parm [__ebx]

#pragma aux RdosAnsiToUtf8 = \
    "push ds" \
    "mov eax,fs" \
    "mov ds,eax" \
    CallGate_ansi_to_utf8  \
    "pop ds" \
    __parm [__fs __esi] [__es __edi] [__ecx] \
    __value [__eax]

#pragma aux RdosUtf8ToAnsi = \
    "push ds" \
    "mov eax,fs" \
    "mov ds,eax" \
    CallGate_utf8_to_ansi  \
    "pop ds" \
    __parm [__fs __esi] [__es __edi] [__ecx] \
    __value [__eax]

#pragma aux RdosAnsiToUtf16 = \
    "push ds" \
    "mov eax,fs" \
    "mov ds,eax" \
    CallGate_ansi_to_utf16  \
    "pop ds" \
    __parm [__fs __esi] [__es __edi] [__ecx]  \
    __value [__eax]

#pragma aux RdosUtf16ToAnsi = \
    "push ds" \
    "mov eax,fs" \
    "mov ds,eax" \
    CallGate_utf16_to_ansi  \
    "pop ds" \
    __parm [__fs __esi] [__es __edi] [__ecx]  \
    __value [__eax]

#pragma aux RdosOpenFont = \
    CallGate_open_font  \
    ValidateHandle \
    __parm [__edx] [__eax]  \
    __value [__ebx]

#pragma aux RdosCloseFont = \
    CallGate_close_font  \
    __parm [__ebx]

#pragma aux RdosGetStringMetrics = \
    CallGate_get_string_metrics  \
    "movzx ecx,cx" \
    "movzx edx,dx" \
    "mov es:[eax],ecx" \
    "mov fs:[esi],edx" \
    __parm [__ebx] [__edi] [__es __eax] [__fs __esi] \
    __modify [__ecx __edx]

#pragma aux RdosSetFont = \
    CallGate_set_font  \
    __parm [__ebx] [__eax]

#pragma aux RdosGetPixel = \
    CallGate_get_pixel  \
    __parm [__ebx] [__ecx] [__edx]  \
    __value [__eax]

#pragma aux RdosSetPixel = \
    CallGate_set_pixel  \
    __parm [__ebx] [__ecx] [__edx]

// Blit here
// DrawMask here

#pragma aux RdosDrawLine = \
    CallGate_draw_line  \
    __parm [__ebx] [__ecx] [__edx] [__esi] [__edi]

#pragma aux RdosDrawString = \
    CallGate_draw_string  \
    __parm [__ebx] [__ecx] [__edx] [__es __edi]

#pragma aux RdosDrawRect = \
    CallGate_draw_rect  \
    __parm [__ebx] [__ecx] [__edx] [__esi] [__edi]

#pragma aux RdosDrawEllipse = \
    CallGate_draw_ellipse  \
    __parm [__ebx] [__ecx] [__edx] [__esi] [__edi]

#pragma aux RdosCreateBitmap = \
    CallGate_create_bitmap  \
    ValidateHandle \
    __parm [__eax] [__ecx] [__edx] \
    __value [__ebx]

#pragma aux RdosDuplicateBitmapHandle = \
    CallGate_dup_bitmap_handle  \
    ValidateHandle  \
    __parm [__ebx]  \
    __value [__ebx]

#pragma aux RdosCloseBitmap = \
    CallGate_close_bitmap  \
    __parm [__ebx]

#pragma aux RdosCreateStringBitmap = \
    CallGate_create_string_bitmap  \
    ValidateHandle  \
    __parm [__ebx] [__es __edi]    \
    __value [__ebx]


// GetBitmapInfo here

#pragma aux RdosCreateSprite = \
    CallGate_create_sprite  \
    ValidateHandle  \
    __parm [__ebx] [__ecx] [__edx] [__eax]    \
    __value [__ebx]

#pragma aux RdosCloseSprite = \
    CallGate_close_sprite  \
    __parm [__ebx]

#pragma aux RdosShowSprite = \
    CallGate_show_sprite  \
    __parm [__ebx]

#pragma aux RdosHideSprite = \
    CallGate_hide_sprite  \
    __parm [__ebx]

#pragma aux RdosMoveSprite = \
    CallGate_move_sprite  \
    __parm [__ebx] [__ecx] [__edx]

#pragma aux RdosSetForeColor = \
    CallGate_set_forecolor  \
    __parm [__eax]

#pragma aux RdosSetBackColor = \
    CallGate_set_backcolor  \
    __parm [__eax]

#pragma aux RdosGetFreePhysical = \
    CallGate_get_free_physical  \
    __value [__edx __eax]

#pragma aux RdosGetFreeGdt = \
    CallGate_get_free_gdt  \
    "movzx eax,ax"  \
    __value [__eax]

#pragma aux RdosGetFreeSmallKernelLinear = \
    CallGate_available_small_linear  \
    __value [__eax]

#pragma aux RdosGetFreeBigKernelLinear = \
    CallGate_available_big_linear  \
    __value [__eax]

#pragma aux RdosGetFreeSmallLocalLinear = \
    CallGate_available_small_local_linear  \
    __value [__eax]

#pragma aux RdosGetFreeBigLocalLinear = \
    CallGate_available_big_local_linear  \
    __value [__eax]

#pragma aux RdosGetMaxComPort = \
    CallGate_get_max_com_port  \
    "jc fail" \
    "movzx eax,al"  \
    "jmp done" \
    "fail:" \
    "xor eax,eax" \
    "done:" \
    __value [__eax]

#pragma aux RdosOpenCom = \
    CallGate_open_com  \
    ValidateHandle  \
    __parm [__al] [__ecx] [__bh] [__ah] [__bl] [__esi] [__edi]  \
    __value [__ebx]

#pragma aux RdosCloseCom = \
    CallGate_close_com  \
    __parm [__ebx]

#pragma aux RdosFlushCom = \
    CallGate_flush_com  \
    __parm [__ebx]

#pragma aux RdosResetCom = \
    CallGate_reset_com  \
    __parm [__ebx]

#pragma aux RdosReadCom = \
    CallGate_read_com  \
    __parm [__ebx]  \
    __value [__al]

#pragma aux RdosWriteCom = \
    CallGate_write_com  \
    "movsx eax,al"  \
    __parm [__ebx] [__al] \
    __value [__eax]

#pragma aux RdosEnableCts = \
    CallGate_enable_cts  \
    __parm [__ebx]

#pragma aux RdosDisableCts = \
    CallGate_disable_cts  \
    __parm [__ebx]

#pragma aux RdosEnableAutoRts = \
    CallGate_enable_auto_rts  \
    __parm [__ebx]

#pragma aux RdosDisableAutoRts = \
    CallGate_disable_auto_rts  \
    __parm [__ebx]

#pragma aux RdosSetDtr = \
    CallGate_set_dtr  \
    __parm [__ebx]

#pragma aux RdosResetDtr = \
    CallGate_reset_dtr  \
    __parm [__ebx]

#pragma aux RdosSetRts = \
    CallGate_set_rts  \
    __parm [__ebx]

#pragma aux RdosResetRts = \
    CallGate_reset_rts  \
    __parm [__ebx]

#pragma aux RdosGetReceiveBufferSpace = \
    CallGate_get_com_receive_space  \
    ValidateEax \
    __parm [__ebx]  \
    __value [__eax]

#pragma aux RdosGetSendBufferSpace = \
    CallGate_get_com_send_space  \
    ValidateEax \
    __parm [__ebx]  \
    __value [__eax]

#pragma aux RdosWaitForSendCompletedCom = \
    CallGate_wait_for_send_completed_com  \
    __parm [__ebx]

#pragma aux RdosGetMaxPrinters = \
    CallGate_get_max_printer  \
    "jc fail" \
    "movzx eax,al"  \
    "jmp done" \
    "fail:" \
    "xor eax,eax" \
    "done:" \
    __value [__eax]

#pragma aux RdosOpenPrinter = \
    CallGate_open_printer  \
    ValidateHandle  \
    __parm [__al]  \
    __value [__ebx]

#pragma aux RdosClosePrinter = \
    CallGate_close_printer  \
    __parm [__ebx]

#pragma aux RdosGetPrinterName = \
    CallGate_get_printer_name  \
    CarryToBool \
    __parm [__ebx] [__es __edi] \
    __value [__eax]

#pragma aux RdosIsPrinterJammed = \
    CallGate_is_printer_jammed  \
    "cmc" \
    CarryToBool \
    __parm [__ebx] \
    __value [__eax]

#pragma aux RdosIsPrinterPaperLow = \
    CallGate_is_printer_paper_low  \
    "cmc" \
    CarryToBool \
    __parm [__ebx] \
    __value [__eax]

#pragma aux RdosIsPrinterPaperEnd = \
    CallGate_is_printer_paper_end  \
    "cmc" \
    CarryToBool \
    __parm [__ebx] \
    __value [__eax]

#pragma aux RdosIsPrinterCutterJammed = \
    CallGate_is_printer_cutter_jammed  \
    "cmc" \
    CarryToBool \
    __parm [__ebx] \
    __value [__eax]

#pragma aux RdosIsPrinterOk = \
    CallGate_is_printer_ok  \
    CarryToBool \
    __parm [__ebx] \
    __value [__eax]

#pragma aux RdosIsPrinterHeadLifted = \
    CallGate_is_printer_head_lifted  \
    "cmc" \
    CarryToBool \
    __parm [__ebx] \
    __value [__eax]

#pragma aux RdosHasPrinterPaperInPresenter = \
    CallGate_has_printer_paper_in_presenter  \
    "cmc" \
    CarryToBool \
    __parm [__ebx] \
    __value [__eax]

#pragma aux RdosHasPrinterTemperatureError = \
    CallGate_has_printer_temp_error  \
    "cmc" \
    CarryToBool \
    __parm [__ebx] \
    __value [__eax]

#pragma aux RdosHasPrinterFeedError = \
    CallGate_has_printer_feed_error  \
    "cmc" \
    CarryToBool \
    __parm [__ebx] \
    __value [__eax]

#pragma aux RdosPrintTest = \
    CallGate_print_test  \
    __parm [__ebx]

#pragma aux RdosCreatePrinterBitmap = \
    CallGate_create_printer_bitmap  \
    "mov ebx,eax" \
    ValidateHandle \
    __parm [__ebx] [__edx] \
    __value [__ebx]

#pragma aux RdosPrintBitmap = \
    CallGate_print_bitmap  \
    __parm [__ebx] [__eax]

#pragma aux RdosPresentPrinterMedia = \
    CallGate_present_printer_media  \
    __parm [__ebx] [__eax]

#pragma aux RdosEjectPrinterMedia = \
    CallGate_eject_printer_media  \
    __parm [__ebx]

#pragma aux RdosWaitForPrint = \
    CallGate_wait_for_print  \
    __parm [__ebx]

#pragma aux RdosResetPrinter = \
    CallGate_reset_printer  \
    __parm [__ebx]

#pragma aux RdosGetMaxCardDev = \
    CallGate_get_max_carddev  \
    "jc fail" \
    "movzx eax,al"  \
    "jmp done" \
    "fail:" \
    "xor eax,eax" \
    "done:" \
    __value [__eax]

#pragma aux RdosOpenCardDev = \
    CallGate_open_carddev  \
    ValidateHandle  \
    __parm [__al]  \
    __value [__ebx]

#pragma aux RdosCloseCardDev = \
    CallGate_close_carddev  \
    __parm [__ebx]

#pragma aux RdosGetCardDevName = \
    CallGate_get_carddev_name  \
    CarryToBool \
    __parm [__ebx] [__es __edi] \
    __value [__eax]

#pragma aux RdosIsCardDevOk = \
    CallGate_is_carddev_ok  \
    CarryToBool \
    __parm [__ebx] \
    __value [__eax]

#pragma aux RdosIsCardDevBusy = \
    CallGate_is_carddev_busy  \
    CarryToBool \
    __parm [__ebx] \
    __value [__eax]

#pragma aux RdosIsCardDevInserted = \
    CallGate_is_carddev_inserted  \
    CarryToBool \
    __parm [__ebx] \
    __value [__eax]

#pragma aux RdosHadCardDevInserted = \
    CallGate_had_carddev_inserted  \
    CarryToBool \
    __parm [__ebx] \
    __value [__eax]

#pragma aux RdosClearCardDevInserted = \
    CallGate_clear_carddev_inserted  \
    __parm [__ebx]

#pragma aux RdosWaitForCard = \
    CallGate_wait_for_card  \
    CarryToBool \
    __parm [__ebx] [__es __edi] \
    __value [__eax]

#pragma aux RdosOpenFile = \
    CallGate_open_file  \
    ValidateHandle  \
    __parm [__es __edi] [__cl] \
    __value [__ebx]

#pragma aux RdosCreateFile = \
    CallGate_create_file  \
    ValidateHandle  \
    __parm [__es __edi] [__ecx] \
    __value [__ebx]

#pragma aux RdosCloseFile = \
    CallGate_close_file  \
    __parm [__ebx]

#pragma aux RdosIsDevice = \
    CallGate_get_ioctl_data  \
    0x33 0xC0 0xF6 0xC6 0x80 0x74 1 0x40 \
    __parm [__ebx]  \
    __value [__eax] \
    __modify [__dx]

#pragma aux RdosDuplFile = \
    CallGate_dupl_file  \
    ValidateHandle  \
    __parm [__eax]  \
    __value [__ebx]

#pragma aux RdosGetFileSize = \
    CallGate_get_file_size  \
    ValidateEax \
    __parm [__ebx]  \
    __value [__eax]

#pragma aux RdosSetFileSize = \
    CallGate_set_file_size  \
    __parm [__ebx] [__eax]

#pragma aux RdosGetFilePos = \
    CallGate_get_file_pos  \
    ValidateEax \
    __parm [__ebx]  \
    __value [__eax]

#pragma aux RdosSetFilePos = \
    CallGate_set_file_pos  \
    __parm [__ebx] [__eax]

#pragma aux RdosReadFile = \
    CallGate_read_file  \
    ValidateEax \
    __parm [__ebx] [__es __edi] [__ecx]  \
    __value [__eax]

#pragma aux RdosWriteFile = \
    CallGate_write_file  \
    ValidateEax \
    __parm [__ebx] [__es __edi] [__ecx]  \
    __value [__eax]

#pragma aux RdosGetFileTime = \
    CallGate_get_file_time  \
    "mov fs:[esi],edx" \
    "mov es:[edi],eax" \
    __parm [__ebx] [__fs __esi] [__es __edi]  \
    __modify [__eax __edx]

#pragma aux RdosSetFileTime = \
    CallGate_set_file_time  \
    __parm [__ebx] [__edx] [__eax]

#pragma aux RdosCreateMapping = \
    CallGate_create_mapping  \
    ValidateHandle  \
    __parm [__eax]  \
    __value [__ebx]

#pragma aux RdosCreateNamedMapping = \
    CallGate_create_named_mapping  \
    ValidateHandle  \
    __parm [__es __edi] [__eax]  \
    __value [__ebx]

#pragma aux RdosCreateNamedFileMapping = \
    CallGate_create_named_file_mapping  \
    ValidateHandle  \
    __parm [__es __edi] [__eax] [__ebx]  \
    __value [__ebx]

#pragma aux RdosOpenNamedMapping = \
    CallGate_open_named_mapping  \
    ValidateHandle  \
    __parm [__es __edi] \
    __value [__ebx]

#pragma aux RdosSyncMapping = \
    CallGate_sync_mapping  \
    __parm [__ebx]

#pragma aux RdosCloseMapping = \
    CallGate_close_mapping  \
    __parm [__ebx]

#pragma aux RdosMapView = \
    CallGate_map_view  \
    __parm [__ebx] [__eax] [__es __edi] [__ecx]

#pragma aux RdosUnmapView = \
    CallGate_unmap_view  \
    __parm [__ebx]

#pragma aux RdosSetCurDrive = \
    CallGate_set_cur_drive  \
    CarryToBool \
    __parm [__eax]  \
    __value [__eax]

#pragma aux RdosGetCurDrive = \
    CallGate_get_cur_drive  \
    "movzx eax,al"  \
    __value [__eax]

#pragma aux RdosGetCurDir = \
    CallGate_get_cur_dir  \
    CarryToBool \
    __parm [__eax] [__es __edi]  \
    __value [__eax]

#pragma aux RdosSetCurDir = \
    CallGate_set_cur_dir  \
    CarryToBool \
    __parm [__es __edi]  \
    __value [__eax]

#pragma aux RdosMakeDir = \
    CallGate_make_dir  \
    CarryToBool \
    __parm [__es __edi]  \
    __value [__eax]

#pragma aux RdosRemoveDir = \
    CallGate_remove_dir  \
    CarryToBool \
    __parm [__es __edi]  \
    __value [__eax]

#pragma aux RdosRenameFile = \
    "push ds" \
    "mov ds,edx" \
    CallGate_rename_file  \
    CarryToBool \
    "pop ds" \
    __parm [__es __edi] [__edx __esi]  \
    __value [__eax]

#pragma aux RdosDeleteFile = \
    CallGate_delete_file  \
    CarryToBool \
    __parm [__es __edi]  \
    __value [__eax]

#pragma aux RdosGetFileAttribute = \
    CallGate_get_file_attribute  \
    "movzx ecx,cx"  \
    "mov fs:[eax],ecx" \
    CarryToBool \
    __parm [__es __edi] [__fs __eax]  \
    __value [__eax] \
    __modify [__ecx]

#pragma aux RdosSetFileAttribute = \
    CallGate_set_file_attribute  \
    CarryToBool \
    __parm [__es __edi] [__ecx] \
    __value [__eax]

#pragma aux RdosOpenDir = \
    CallGate_open_dir  \
    ValidateHandle \
    __parm [__es __edi]  \
    __value [__ebx]

#pragma aux RdosCloseDir = \
    CallGate_close_dir  \
    __parm [__ebx]

// ReadDir here

#pragma aux RdosGetImageHeader = \
    CallGate_get_image_header  \
    CarryToBool \
    __parm [__eax] [__edx] [__es __edi] \
    __value [__eax]

#pragma aux RdosGetImageData = \
    CallGate_get_image_data  \
    CarryToBool \
    __parm [__eax] [__edx] [__es __edi] \
    __value [__eax]

#pragma aux RdosGetDeviceInfo = \
    "push ds" \
    "mov ds,eax" \
    "push edx" \
    CallGate_get_device_info  \
    "mov [ecx],eax" \
    CarryToBool \
    "mov gs:[esi],edx" \
    "pop edx" \
    "mov fs:[edx],bx" \
    "pop ds" \
    __parm [__ebx] [__es __edi] [__eax __ecx] [__fs __edx] [__gs __esi] \
    __modify [__ebx __edx] \
    __value [__eax]

#pragma aux RdosGetSelectorInfo = \
    CallGate_get_selector_info  \
    "mov fs:[esi],ecx" \
    "movzx eax,al" \
    "mov es:[edi],eax" \
    CarryToBool \
    __parm [__bx] [__fs __esi] [__es __edi] \
    __modify [__ecx] \
    __value [__eax]

#pragma aux RdosGetFaultThreadState = \
    CallGate_get_fault_thread_state  \
    CarryToBool \
    __parm [__eax] [__es __edi] \
    __value [__eax]

#pragma aux RdosGetFaultThreadTss = \
    CallGate_get_fault_thread_tss  \
    CarryToBool \
    __parm [__eax] [__es __edi] \
    __value [__eax]

#pragma aux RdosGetThreadCount = \
    CallGate_get_thread_count  \
    "movzx eax,ax" \
    __value [__eax]

#pragma aux RdosGetThreadState = \
    CallGate_get_thread_state  \
    CarryToBool \
    __parm [__eax] [__es __edi] \
    __value [__eax]

#pragma aux RdosSetThreadAction = \
    CallGate_set_thread_action  \
    __parm [__es __edi]

#pragma aux RdosGetThreadActionState = \
    CallGate_get_thread_action_state  \
    CarryToBool \
    __parm [__eax] [__es __edi] \
    __value [__eax]

#pragma aux RdosSuspendThread = \
    CallGate_suspend_thread  \
    CarryToBool \
    __parm [__eax] \
    __value [__eax]

#pragma aux RdosSuspendAndSignalThread = \
    CallGate_suspend_and_signal_thread  \
    CarryToBool \
    __parm [__eax] \
    __value [__eax]

#pragma aux RdosSoftReset = \
    CallGate_soft_reset;

#pragma aux RdosHardReset = \
    CallGate_hard_reset;

#pragma aux RdosPowerFailure = \
    CallGate_power_failure \
    "movzx eax,ax"  \
    __value [__eax]

#pragma aux RdosIsEmergencyStopped = \
    CallGate_is_emergency_stopped \
    CarryToBool \
    __value [__eax]

#pragma aux RdosHasGlobalTimer = \
    CallGate_has_global_timer \
    CarryToBool \
    __value [__eax]

#pragma aux RdosGetActiveCores = \
    CallGate_get_active_cores \
    __value [__eax]

#pragma aux RdosGetCoreLoad = \
    CallGate_get_core_load \
    "mov fs:[esi],ebx" \
    "mov fs:[esi+4],ecx" \
    "mov es:[edi],eax" \
    "mov es:[edi+4],edx" \
    CarryToBool \
    __parm [__eax] [__fs __esi] [__es __edi] \
    __value [__eax] \
    __modify [__ebx __ecx __edx]

#pragma aux RdosGetCoreDuty = \
    CallGate_get_core_duty \
    "mov fs:[esi],eax" \
    "mov fs:[esi+4],edx" \
    "mov es:[edi],ebx" \
    "mov es:[edi+4],ecx" \
    CarryToBool \
    __parm [__eax] [__fs __esi] [__es __edi] \
    __value [__eax] \
    __modify [__ebx __ecx __edx]

#pragma aux RdosGetVersion = \
    CallGate_get_version  \
    "movzx edx,dx"  \
    "mov gs:[ebx],edx" \
    "movzx  eax,ax" \
    "mov fs:[esi],eax" \
    "movzx ecx,cx"  \
    "mov es:[edi],ecx" \
    __parm [__gs __ebx] [__fs __esi] [__es __edi]  \
    __modify [__eax __ecx __edx]

#pragma aux RdosGetCpuVersion = \
    CallGate_get_cpu_version  \
    "movzx  eax,al" \
    "mov fs:[esi],edx" \
    "mov gs:[ecx],ebx" \
    __parm [__es __edi] [__fs __esi] [__gs __ecx] \
    __value [__eax]

#pragma aux RdosTerminateThread = \
    CallGate_terminate_thread;

#pragma aux RdosGetThreadHandle = \
    CallGate_get_thread \
    "movzx eax,ax"  \
    __value [__eax]

#pragma aux RdosWaitMilli = \
    CallGate_wait_milli  \
    __parm [__eax]

#pragma aux RdosWaitMicro = \
    CallGate_wait_micro  \
    __parm [__eax]

#pragma aux RdosWaitUntil = \
    CallGate_wait_until  \
    __parm [__edx] [__eax]

#pragma aux RdosGetSysTime = \
    CallGate_get_system_time  \
    "mov fs:[esi],edx" \
    "mov es:[edi],eax" \
    __parm [__fs __esi] [__es __edi] \
    __modify [__eax __edx]

#pragma aux RdosGetLongSysTime = \
    CallGate_get_system_time  \
    __value [__edx __eax]

#pragma aux RdosGetTime = \
    CallGate_get_time  \
    "mov fs:[esi],edx" \
    "mov es:[edi],eax" \
    __parm [__fs __esi] [__es __edi] \
    __modify [__eax __edx]

#pragma aux RdosSetTime = \
    CallGate_get_system_time  \
    "sub esi,eax" \
    "sbb edi,edx" \
    "mov eax,esi" \
    "mov edx,edi" \
    CallGate_update_time  \
    __parm [__edi] [__esi] \
    __modify [__eax __edx __esi __edi]

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
    __parm [__edx] [__eax] [__ecx] \
    __value [__eax] \
    __modify [__ebx __ecx __edx]

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
    "mov fs:[ebx],edx" \
    "mov es:[ecx],eax" \
    __parm [__si] [__di] [__fs __ebx] [__es __ecx] \
    __modify [__eax __edx]

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
    "mov fs:[esi],dx" \
    "mov es:[edi],bx" \
    __parm [__edx] [__eax] [__fs __esi] [__es __edi] \
    __modify [__eax __ebx __ecx __edx]

#pragma aux RdosDecodeMsbTics = \
    "push ds" \
    "mov ds,eax" \
    "push ebx" \
    "push ecx" \
    "xor eax,eax" \
    CallGate_binary_to_time  \
    "movzx eax,bh" \
    "mov es:[edi],eax" \
    "movzx eax,cl" \
    "mov [esi],eax" \
    "movzx eax,ch" \
    "pop ecx" \
    "pop ebx" \
    "mov gs:[ecx],eax" \
    "movzx eax,dx" \
    "mov fs:[ebx],eax" \
    "pop ds" \
    __parm [__edx] [__fs __ebx] [__gs __ecx] [__eax __esi] [__es __edi] \
    __modify [__eax __edx]

#pragma aux RdosDecodeLsbTics = \
    "push ds" \
    "mov ds,edx" \
    "mov edx,60" \
    "mul edx" \
    "mov fs:[ebx],edx" \
    "mov edx,60" \
    "mul edx" \
    "mov gs:[ecx],edx" \
    "mov edx,1000" \
    "mul edx" \
    "mov [esi],edx" \
    "mov edx,1000" \
    "mul edx" \
    "mov es:[edi],edx" \
    "pop ds" \
    __parm [__eax] [__fs __ebx] [__gs __ecx] [__edx __esi] [__es __edi] \
    __modify [__eax __edx]

#pragma aux RdosCodeMsbTics = \
    "mov ch,al" \
    "mov bh,bl" \
    "xor bl,bl" \
    "xor ah,ah" \
    CallGate_time_to_binary  \
    __parm [__edx] [__eax] [__ecx] [__ebx] \
    __value [__edx] \
    __modify [__eax __bx __cx]

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
    __parm [__ebx] [__eax] [__esi] [__edi] \
    __value [__eax] \
    __modify [__ebx __ecx __edx]

#pragma aux RdosAddTics = \
    "add fs:[esi],eax" \
    "adc es:[edi],0"  \
    __parm [__es __edi] [__fs __esi] [__eax]

#pragma aux RdosAddMicro = \
    "mov edx,1193"  \
    "imul edx"  \
    "xor edx,edx" \
    "mov ebx,1000" \
    "idiv ebx" \
    "add fs:[esi],eax" \
    "adc dword ptr es:[edi],0"  \
    __parm [__es __edi] [__fs __esi] [__eax] \
    __modify [__eax __ebx __edx]

#pragma aux RdosAddMilli = \
    "mov edx,1193"  \
    "imul edx"  \
    "add fs:[esi],eax" \
    "adc es:[edi],edx"  \
    __parm [__es __edi] [__fs __esi] [__eax] \
    __modify [__eax __edx]

#pragma aux RdosAddSec = \
    "mov edx,1193046"  \
    "imul edx"  \
    "add fs:[esi],eax" \
    "adc es:[edi],edx"  \
    __parm [__es __edi] [__fs __esi] [__eax] \
    __modify [__eax __edx]

#pragma aux RdosAddMin = \
    "mov edx,1193046*60"  \
    "imul edx"  \
    "add fs:[esi],eax" \
    "adc es:[edi],edx"  \
    __parm [__es __edi] [__fs __esi] [__eax] \
    __modify [__eax __edx]

#pragma aux RdosAddHour = \
    "add es:[edi],eax"  \
     __parm [__es __edi] [__fs __esi] [__eax]

#pragma aux RdosAddDay = \
    "mov edx,24"  \
    "imul edx"  \
    "add es:[edi],eax"  \
    __parm [__es __edi] [__fs __esi] [__eax] \
    __modify [__eax __edx]

#pragma aux RdosSyncTime = \
    CallGate_sync_time  \
    CarryToBool \
    __parm [__edx] \
    __value [__eax]

#pragma aux RdosCreateSection = \
    CallGate_create_user_section  \
    ValidateHandle  \
    __value [__ebx]

#pragma aux RdosDeleteSection = \
    CallGate_delete_user_section  \
    __parm [__ebx]

#pragma aux RdosEnterSection = \
    CallGate_enter_user_section  \
    __parm [__ebx]

#pragma aux RdosLeaveSection = \
    CallGate_leave_user_section  \
    __parm [__ebx]

#pragma aux RdosGetFreeHandles = \
    CallGate_get_free_handles  \
    "movzx eax,ax"  \
    __value [__eax]

#pragma aux RdosGetFreeHandleMem = \
    CallGate_get_free_handle_mem  \
    __value [__eax]

#pragma aux RdosCreateWait = \
    CallGate_create_wait  \
    ValidateHandle  \
    __value [__ebx]

#pragma aux RdosCloseWait = \
    CallGate_close_wait  \
    __parm [__ebx]

#pragma aux RdosCheckWait = \
    CallGate_is_wait_idle  \
    __parm [__ebx] \
    __value [__ecx]

#pragma aux RdosWaitForever = \
    CallGate_wait_no_timeout  \
    ValidateEcx \
    __parm [__ebx] \
    __value [__ecx]

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
    __parm [__ebx] [__eax] \
    __value [__ecx] \
    __modify [__eax __edx]

#pragma aux RdosWaitUntilTimeout = \
    CallGate_wait_timeout  \
    ValidateEcx \
    __parm [__ebx] [__edx] [__eax] \
    __value [__ecx]

#pragma aux RdosStopWait = \
    CallGate_stop_wait  \
    __parm [__ebx]

#pragma aux RdosRemoveWait = \
    CallGate_remove_wait  \
    __parm [__ebx] [__ecx]

#pragma aux RdosAddWaitForKeyboard = \
    CallGate_add_wait_for_keyboard  \
    __parm [__ebx] [__ecx]

#pragma aux RdosAddWaitForMouse = \
    CallGate_add_wait_for_mouse  \
    __parm [__ebx] [__ecx]

#pragma aux RdosAddWaitForCom = \
    CallGate_add_wait_for_com  \
    __parm [__ebx] [__eax] [__ecx]

#pragma aux RdosAddWaitForSyslog = \
    CallGate_add_wait_for_syslog  \
    __parm [__ebx] [__eax] [__ecx]

#pragma aux RdosAddWaitForAdc = \
    CallGate_add_wait_for_adc  \
    __parm [__ebx] [__eax] [__ecx]

#pragma aux RdosCreateSignal = \
    CallGate_create_signal  \
    ValidateHandle  \
    __value [__ebx]

#pragma aux RdosResetSignal = \
    CallGate_reset_signal  \
    __parm [__ebx]

#pragma aux RdosSetSignal = \
    CallGate_set_signal  \
    __parm [__ebx]

#pragma aux RdosIsSignalled = \
    CallGate_is_signalled  \
    CarryToBool \
    __parm [__ebx] \
    __value [__eax]

#pragma aux RdosFreeSignal = \
    CallGate_free_signal  \
    __parm [__ebx]

#pragma aux RdosAddWaitForSignal = \
    CallGate_add_wait_for_signal  \
    __parm [__ebx] [__eax] [__ecx]

#pragma aux RdosGetIp = \
    CallGate_get_ip_address  \
    __value [__edx]

#pragma aux RdosGetGateway = \
    CallGate_get_gateway  \
    __value [__edx]

#pragma aux RdosNameToIp = \
    CallGate_name_to_ip  \
    __parm [__es __edi] \
    __value [__edx]

#pragma aux RdosIpToName = \
    CallGate_ip_to_name  \
    __parm [__edx] [__es __edi] [__ecx] \
    __value [__eax]

#pragma aux RdosCreateTcpListen = \
    CallGate_create_tcp_listen  \
    ValidateHandle \
    __parm [__esi] [__eax] [__ecx] \
    __value [__ebx]

#pragma aux RdosGetTcpListen = \
    CallGate_get_tcp_listen  \
    "movzx ebx,ax" \
    ValidateHandle \
    __parm [__ebx] \
    __value [__ebx] \
    __modify [__ax]

#pragma aux RdosCloseTcpListen = \
    CallGate_close_tcp_listen  \
    __parm [__ebx]

#pragma aux RdosAddWaitForTcpListen = \
    CallGate_add_wait_for_tcp_listen  \
    __parm [__ebx] [__eax] [__ecx]

#pragma aux RdosOpenTcpConnection = \
    CallGate_open_tcp_connection  \
    ValidateHandle \
    __parm [__edx] [__esi] [__edi] [__eax] [__ecx] \
    __value [__ebx]

#pragma aux RdosWaitForTcpConnection = \
    CallGate_wait_for_tcp_connection  \
    CarryToBool \
    __parm [__ebx] [__eax] \
    __value [__eax]

#pragma aux RdosAddWaitForTcpConnection = \
    CallGate_add_wait_for_tcp_connection  \
    __parm [__ebx] [__eax] [__ecx]

#pragma aux RdosCloseTcpConnection = \
    CallGate_close_tcp_connection  \
    __parm [__ebx]

#pragma aux RdosDeleteTcpConnection = \
    CallGate_delete_tcp_connection  \
    __parm [__ebx]

#pragma aux RdosAbortTcpConnection = \
    CallGate_abort_tcp_connection  \
    __parm [__ebx]

#pragma aux RdosPushTcpConnection = \
    CallGate_push_tcp_connection  \
    __parm [__ebx]

#pragma aux RdosIsTcpConnectionClosed = \
    CallGate_is_tcp_connection_closed  \
    "cmc"   \
    CarryToBool \
    __parm [__ebx] \
    __value [__eax]

#pragma aux RdosIsTcpConnectionIdle = \
    CallGate_is_tcp_connection_idle  \
    CarryToBool \
    __parm [__ebx] \
    __value [__eax]

#pragma aux RdosGetRemoteTcpConnectionIP = \
    CallGate_get_remote_tcp_connection_ip  \
    ValidateEax \
    __parm [__ebx] \
    __value [__eax]

#pragma aux RdosGetRemoteTcpConnectionPort = \
    CallGate_get_remote_tcp_connection_port  \
    "movzx eax,ax"\
    ValidateEax \
    __parm [__ebx] \
    __value [__eax]

#pragma aux RdosGetLocalTcpConnectionPort = \
    CallGate_get_local_tcp_connection_port  \
    "movzx eax,ax"\
    ValidateEax \
    __parm [__ebx] \
    __value [__eax]

#pragma aux RdosReadTcpConnection = \
    CallGate_read_tcp_connection  \
    ValidateEax \
    __parm [__ebx] [__es __edi] [__ecx] \
    __value [__eax]

#pragma aux RdosWriteTcpConnection = \
    CallGate_write_tcp_connection  \
    "mov eax,1" \
    ValidateEax \
    __parm [__ebx] [__es __edi] [__ecx] \
    __value [__eax]

#pragma aux RdosPollTcpConnection = \
    CallGate_poll_tcp_connection  \
    ValidateEax \
    __parm [__ebx] \
    __value [__eax]

#pragma aux RdosGetLocalMailslot = \
    CallGate_get_local_mailslot  \
    ValidateHandle \
    __parm [__es __edi] \
    __value [__ebx]

#pragma aux RdosGetRemoteMailslot = \
    CallGate_get_remote_mailslot  \
    ValidateHandle \
    __parm [__edx] [__es __edi] \
    __value [__ebx]

#pragma aux RdosFreeMailslot = \
    CallGate_free_mailslot  \
    __parm [__ebx]

#pragma aux RdosSendMailslot = \
    "push ds" \
    "mov ds,edx" \
    CallGate_send_mailslot  \
    ValidateEcx \
    "pop ds" \
    __parm [__ebx] [__edx __esi] [__ecx] [__es __edi] [__eax] \
    __value [__ecx]

#pragma aux RdosDefineMailslot = \
    CallGate_define_mailslot  \
    __parm [__es __edi] [__ecx]

#pragma aux RdosReceiveMailslot = \
    CallGate_receive_mailslot  \
    __parm [__es __edi] \
    __value [__ecx]

#pragma aux RdosReplyMailslot = \
    CallGate_reply_mailslot  \
    __parm [__es __edi] [__ecx]

#pragma aux RdosSetFocus = \
    CallGate_set_focus  \
    __parm [__al]

#pragma aux RdosSetKeyMap = \
    CallGate_set_key_layout \
    CarryToBool \
    __parm [__es __edi] \
    __value [__eax]

#pragma aux RdosGetKeyMap = \
    CallGate_get_key_layout \
    __parm [__es __edi]

#pragma aux RdosClearKeyboard = \
    CallGate_flush_keyboard;

#pragma aux RdosPollKeyboard = \
    CallGate_poll_keyboard  \
    CarryToBool \
    __value [__eax]

#pragma aux RdosReadKeyboard = \
    CallGate_read_keyboard  \
    "movzx eax,ax" \
    __value [__eax]

#pragma aux RdosGetKeyboardState = \
    CallGate_get_keyboard_state  \
    "movzx eax,ax" \
    __value [__eax]

#pragma aux RdosPutKeyboard = \
    "mov dh,cl" \
    CallGate_put_keyboard_code  \
    __parm [__eax] [__edx] [__ecx] \
    __modify [__dh]

#pragma aux RdosPeekKeyEvent = \
    "push ds" \
    "mov ds,edx" \
    "push ecx" \
    CallGate_peek_key_event  \
    "movzx eax,ax" \
    "mov fs:[ebx],eax" \
    "movzx eax,cx" \
    "pop ecx" \
    "mov gs:[ecx],eax" \
    "movzx eax,dl" \
    "mov [esi],eax" \
    "movzx eax,dh" \
    "mov es:[edi],eax" \
    CarryToBool \
    "pop ds" \
    __parm [__fs __ebx] [__gs __ecx] [__edx __esi] [__es __edi] \
    __value [__eax] \
    __modify [__dx]

#pragma aux RdosReadKeyEvent = \
    "push ds" \
    "mov ds,edx" \
    "push ecx" \
    CallGate_read_key_event  \
    "movzx eax,ax" \
    "mov fs:[ebx],eax" \
    "movzx eax,cx" \
    "pop ecx" \
    "mov gs:[ecx],eax" \
    "movzx eax,dl" \
    "mov [esi],eax" \
    "movzx eax,dh" \
    "mov es:[edi],eax" \
    CarryToBool \
    "pop ds" \
    __parm [__fs __ebx] [__gs __ecx] [__edx __esi] [__es __edi] \
    __value [__eax] \
    __modify [__dx]

#pragma aux RdosHideMouse = \
    CallGate_hide_mouse;

#pragma aux RdosShowMouse = \
    CallGate_show_mouse;

#pragma aux RdosGetMousePosition = \
    CallGate_get_mouse_position \
    "movzx ecx,cx" \
    "mov fs:[esi],ecx" \
    "movzx edx,dx" \
    "mov es:[edi],edx" \
    __parm [__fs __esi] [__es __edi] \
    __modify [__ecx __edx]

#pragma aux RdosSetMousePosition = \
    CallGate_set_mouse_position \
    __parm [__ecx] [__edx]

#pragma aux RdosSetMouseWindow = \
    CallGate_set_mouse_window \
    __parm [__eax] [__ebx] [__ecx] [__edx]

#pragma aux RdosSetMouseMickey = \
    CallGate_set_mouse_mickey \
    __parm [__ecx] [__edx]

#pragma aux RdosGetLeftButton = \
    CallGate_get_left_button \
    CarryToBool \
    __value [__eax]

#pragma aux RdosGetRightButton = \
    CallGate_get_right_button \
    CarryToBool \
    __value [__eax]

#pragma aux RdosGetLeftButtonPressPosition = \
    CallGate_get_left_button_press_position \
    "movzx ecx,cx" \
    "mov fs:[esi],ecx" \
    "movzx edx,dx" \
    "mov es:[edi],edx" \
    __parm [__fs __esi] [__es __edi] \
    __modify [__ecx __edx]

#pragma aux RdosGetRightButtonPressPosition = \
    CallGate_get_right_button_press_position \
    "movzx ecx,cx" \
    "mov fs:[esi],ecx" \
    "movzx edx,dx" \
    "mov es:[edi],edx" \
    __parm [__fs __esi] [__es __edi] \
    __modify [__ecx __edx]

#pragma aux RdosGetLeftButtonReleasePosition = \
    CallGate_get_left_button_release_position \
    "movzx ecx,cx" \
    "mov fs:[esi],ecx" \
    "movzx edx,dx" \
    "mov es:[edi],edx" \
    __parm [__fs __esi] [__es __edi] \
    __modify [__ecx __edx]

#pragma aux RdosGetRightButtonReleasePosition = \
    CallGate_get_right_button_release_position \
    "movzx ecx,cx" \
    "mov fs:[esi],ecx" \
    "movzx edx,dx" \
    "mov es:[edi],edx" \
    __parm [__fs __esi] [__es __edi] \
    __modify [__ecx __edx]

#pragma aux RdosClearText = \
    CallGate_clear_text;

#pragma aux RdosGetTextSize = \
    CallGate_get_text_size \
    "movzx ecx,cx" \
    "mov fs:[esi],ecx" \
    "movzx edx,dx" \
    "mov es:[edi],edx" \
    __parm [__es __edi] [__fs __esi] \
    __modify [__ecx __edx]

#pragma aux RdosGetCursorPosition = \
    CallGate_get_cursor_position \
    "movzx ecx,cx" \
    "mov fs:[esi],ecx" \
    "movzx edx,dx" \
    "mov es:[edi],edx" \
    __parm [__es __edi] [__fs __esi] \
    __modify [__ecx __edx]

#pragma aux RdosSetCursorPosition = \
    CallGate_set_cursor_position \
    __parm [__edx] [__ecx]

#pragma aux RdosWriteChar = \
    CallGate_write_char \
    __parm [__al]

#pragma aux RdosWriteSizeString = \
    CallGate_write_size_string \
    __parm [__es __edi] [__ecx]

#pragma aux RdosWriteAttributeString = \
    CallGate_write_attrib_string \
    __parm [__edx] [__eax] [__es __edi] [__ecx]

#pragma aux RdosWriteString = \
    CallGate_write_asciiz  \
    __parm [__es __edi]

#pragma aux RdosPing = \
    CallGate_ping  \
    CarryToBool \
    __parm [__edx] [__eax] \
    __value [__eax]

#pragma aux RdosGetIdeDisc = \
    CallGate_get_ide_disc  \
    ValidateDisc \
    __parm [__ebx] \
    __value [__eax]

#pragma aux RdosGetFloppyDisc = \
    CallGate_get_floppy_disc  \
    ValidateDisc \
    __parm [__ebx] \
    __value [__eax]

#pragma aux RdosSetDiscInfo = \
    CallGate_set_disc_info  \
    CarryToBool \
    __parm [__eax] [__ecx] [__edx] [__esi] [__edi] \
    __value [__eax]

#pragma aux RdosGetDiscInfo = \
    "push ds" \
    "mov ds,edx" \
    "push edi" \
    "push esi" \
    "push edx" \
    "push ecx" \
    CallGate_get_disc_info  \
    "pop ebx" \
    "movzx ecx,cx" \
    "mov fs:[ebx],ecx" \
    "pop ebx" \
    "mov gs:[ebx],edx" \
    "pop ebx" \
    "movzx esi,si" \
    "mov ds:[ebx],esi" \
    "pop ebx" \
    "movzx edi,di" \
    "mov es:[ebx],edi" \
    CarryToBool \
    "pop ds" \
    __parm [__eax] [__fs __ecx] [__gs __edx] [__edx __esi] [__es __edi] \
    __value [__eax] \
    __modify [__ebx __ecx __edx __esi __edi]

#pragma aux RdosReadDisc = \
    CallGate_read_long_disc  \
    CarryToBool \
    __parm [__ebx] [__edx __eax] [__es __edi] [__ecx] \
    __value [__eax]

#pragma aux RdosWriteDisc = \
    CallGate_write_long_disc  \
    CarryToBool \
    __parm [__ebx] [__edx __eax] [__es __edi] [__ecx] \
    __value [__eax]

#pragma aux RdosGetRdfsInfo = \
    "push gs" \
    "mov gs,edx" \
    CallGate_get_rdfs_info  \
    "pop gs" \
    __parm [__edx __esi] [__es __edi] [__gs __ebx] \
    __modify [__ax]

#pragma aux RdosDemandLoadDrive = \
    CallGate_demand_load_drive  \
    __parm [__eax]

#pragma aux RdosFormatDrive = \
    CallGate_format_drive  \
    ValidateDisc \
    __parm [__eax] [__edx] [__ecx] [__es __edi] \
    __value [__eax]

#pragma aux RdosAllocateFixedDrive = \
    CallGate_allocate_fixed_drive  \
    CarryToBool \
    __parm [__eax] \
    __value [__eax]

#pragma aux RdosAllocateStaticDrive = \
    CallGate_allocate_static_drive  \
    ValidateDisc \
    __value [__eax]

#pragma aux RdosAllocateDynamicDrive = \
    CallGate_allocate_dynamic_drive  \
    ValidateDisc \
    __value [__eax]

#pragma aux RdosGetDriveInfo = \
    CallGate_get_drive_info  \
    "mov gs:[ebx],eax" \
    "movzx ecx,cx" \
    "mov fs:[esi],ecx" \
    "mov es:[edi],edx" \
    CarryToBool \
    __parm [__eax] [__gs __ebx] [__fs __esi] [__es __edi] \
    __value [__eax] \
    __modify [__ecx __edx]

#pragma aux RdosGetDriveDiscParam = \
    CallGate_get_drive_disc_param  \
    "movzx eax,al" \
    "mov gs:[ebx],eax" \
    "mov fs:[esi],edx" \
    "mov es:[edi],ecx" \
    CarryToBool \
    __parm [__eax] [__gs __ebx] [__fs __esi] [__es __edi] \
    __value [__eax] \
    __modify [__ecx __edx]

#pragma aux RdosCreateFileDrive = \
    "push ds" \
    "mov ds,edx" \
    CallGate_create_file_drive  \
    CarryToBool \
    "pop ds" \
    __parm [__eax] [__ecx] [__edx __esi] [__es __edi] \
    __value [__eax]

#pragma aux RdosOpenFileDrive = \
    CallGate_open_file_drive  \
    CarryToBool \
    __parm [__eax] [__es __edi] \
    __value [__eax]

#pragma aux RdosCreateCrc = \
    CallGate_create_crc  \
    ValidateHandle \
    __parm [__ax] \
    __value [__ebx]

#pragma aux RdosCloseCrc = \
    CallGate_close_crc  \
    __parm [__ebx]

#pragma aux RdosCalcCrc = \
    CallGate_calc_crc  \
    __parm [__ebx] [__ax] [__es __edi] [__ecx] \
    __value [__ax]

#pragma aux RdosOpenSyslog = \
    CallGate_open_syslog  \
    ValidateHandle \
    __value [__ebx]

#pragma aux RdosCloseSyslog = \
    CallGate_close_syslog  \
    __parm [__ebx]

#pragma aux RdosGetSyslog = \
    "push edi" \
    "push esi" \
    "push edx" \
    "push eax" \
    CallGate_get_syslog  \
    "pop edi" \
    "mov gs:[edi],eax" \
    "pop edi" \
    "mov gs:[edi],edx" \
    "pop edi" \
    "movzx eax,si" \
    "xor ah,ah" \
    "mov fs:[edi],eax" \
    "movzx eax,si" \
    "mov al,ah" \
    "xor ah,ah" \
    __parm [__ebx] [__fs __esi] [__gs __edx] [__gs __eax] [__es __edi] [__ecx] \
    __value [__eax]

#pragma aux RdosOpenAdc = \
    CallGate_open_adc  \
    ValidateHandle \
    __parm [__eax] \
    __value [__ebx]

#pragma aux RdosCloseAdc = \
    CallGate_close_adc  \
    __parm [__ebx]

#pragma aux RdosDefineAdcTime = \
    CallGate_define_adc_time  \
    __parm [__ebx] [__edx] [__eax]

#pragma aux RdosReadAdc = \
    CallGate_read_adc  \
    __parm [__ebx] \
    __value [__eax]

#pragma aux RdosReadSerialLines = \
    "mov dh,cl" \
    CallGate_read_serial_lines  \
    "movzx eax,al" \
    "mov fs:[esi],eax" \
    CarryToBool \
    __parm [__ecx] [__fs __esi] \
    __value [__eax] \
    __modify [__dh]

#pragma aux RdosToggleSerialLine = \
    "mov dh,cl" \
    CallGate_toggle_serial_line  \
    CarryToBool \
    __parm [__ecx] [__edx] \
    __value [__eax] \
    __modify [__dh]

#pragma aux RdosReadSerialVal = \
    "mov dh,cl" \
    CallGate_read_serial_val  \
    "pushf" \
    "shl eax,8" \
    "mov fs:[esi],eax" \
    "popf" \
    CarryToBool \
    __parm [__ecx] [__edx] [__fs __esi] \
    __value [__eax] \
    __modify [__dh]

#pragma aux RdosWriteSerialVal = \
    "mov dh,cl" \
    "sar eax,8" \
    CallGate_write_serial_val  \
    CarryToBool \
    __parm [__ecx] [__edx] [__eax] \
    __value [__eax] \
    __modify [__dh]

#pragma aux RdosReadSerialRaw = \
    "mov dh,cl" \
    CallGate_read_serial_val  \
    "mov fs:[esi],eax" \
    CarryToBool \
    __parm [__ecx] [__edx] [__fs __esi] \
    __value [__eax] \
    __modify [__dh]

#pragma aux RdosWriteSerialRaw = \
    "mov dh,cl" \
    CallGate_write_serial_val  \
    CarryToBool \
    __parm [__ecx] [__edx] [__eax] \
    __value [__eax] \
    __modify [__dh]

#pragma aux RdosOpenSysEnv = \
    CallGate_open_sys_env \
    ValidateHandle \
    __value [__ebx]

#pragma aux RdosOpenProcessEnv = \
    CallGate_open_proc_env \
    ValidateHandle \
    __value [__ebx]

#pragma aux RdosCloseEnv = \
    CallGate_close_env \
    __parm [__ebx]

#pragma aux RdosAddEnvVar = \
    "push ds" \
    "mov ds,edx" \
    CallGate_add_env_var \
    "pop ds" \
    __parm [__ebx] [__edx __esi] [__es __edi]

#pragma aux RdosDeleteEnvVar = \
    "push ds" \
    "mov ds,edx" \
    CallGate_delete_env_var \
    "pop ds" \
    __parm [__ebx] [__edx __esi]

#pragma aux RdosFindEnvVar = \
    "push ds" \
    "mov ds,edx" \
    CallGate_find_env_var \
    CarryToBool \
    "pop ds" \
    __parm [__ebx] [__edx __esi] [__es __edi] \
    __value [__eax]

#pragma aux RdosGetEnvSize = \
    CallGate_get_env_size \
    __parm [__ebx] \
    __value [__eax]

#pragma aux RdosGetEnvData = \
    "xor ax,ax" \
    "mov es:[edi],ax" \
    CallGate_get_env_data \
    __parm [__ebx] [__es __edi] \
    __modify [__ax]

#pragma aux RdosSetEnvData = \
    CallGate_set_env_data \
    __parm [__ebx] [__es __edi]

#pragma aux RdosOpenSysIni = \
    CallGate_open_sys_ini \
    ValidateHandle \
    __value [__ebx]

#pragma aux RdosOpenIni = \
    CallGate_open_ini \
    ValidateHandle \
    __parm [__es __edi] \
    __value [__ebx]

#pragma aux RdosCloseIni = \
    CallGate_close_ini \
    __parm [__ebx]

#pragma aux RdosGotoIniSection = \
    CallGate_goto_ini_section \
    CarryToBool \
    __parm [__ebx] [__es __edi] \
    __value [__eax]

#pragma aux RdosRemoveIniSection = \
    CallGate_remove_ini_section \
    CarryToBool \
    __parm [__ebx] [__es __edi] \
    __value [__eax]

#pragma aux RdosReadIni = \
    "push ds" \
    "mov ds,edx" \
    CallGate_read_ini \
    CarryToBool \
    "pop ds" \
    __parm [__ebx] [__edx __esi] [__es __edi] [__ecx] \
    __value [__eax]

#pragma aux RdosWriteIni = \
    "push ds" \
    "mov ds,edx" \
    CallGate_write_ini \
    CarryToBool \
    "pop ds" \
    __parm [__ebx] [__edx __esi] [__es __edi] \
    __value [__eax]

#pragma aux RdosDeleteIni = \
    "push ds" \
    "mov ds,edx" \
    CallGate_delete_ini \
    CarryToBool \
    "pop ds" \
    __parm [__ebx] [__edx __esi] \
    __value [__eax]

#pragma aux RdosEnableStatusLED = \
    CallGate_enable_status_led;

#pragma aux RdosDisableStatusLED = \
    CallGate_disable_status_led;

#pragma aux RdosStartWatchdog = \
    CallGate_start_watchdog \
    __parm [__eax]

#pragma aux RdosKickWatchdog = \
    CallGate_kick_watchdog;

#pragma aux RdosStopWatchdog = \
    CallGate_stop_watchdog;

#pragma aux RdosStartNetCapture = \
    CallGate_start_net_capture \
    __parm [__ebx]

#pragma aux RdosStopNetCapture = \
    CallGate_stop_net_capture;

#pragma aux RdosGetUsbDevice = \
    CallGate_get_usb_device \
    __parm [__ebx] [__eax] [__es __edi] [__ecx] \
    __value [__eax]

#pragma aux RdosGetUsbConfig = \
    CallGate_get_usb_config \
    __parm [__ebx] [__eax] [__edx] [__es __edi] [__ecx] \
    __value [__eax]

#pragma aux RdosOpenUsbPipe = \
    CallGate_open_usb_pipe \
    ValidateHandle \
    __parm [__ebx] [__eax] [__edx] \
    __value [__ebx]

#pragma aux RdosCloseUsbPipe = \
    CallGate_close_usb_pipe \
    __parm [__ebx]

#pragma aux RdosResetUsbPipe = \
    CallGate_reset_usb_pipe \
    __parm [__ebx]

#pragma aux RdosAddWaitForUsbPipe = \
    CallGate_add_wait_for_usb_pipe \
    __parm [__ebx] [__eax] [__ecx]

#pragma aux RdosWriteUsbControl = \
    CallGate_write_usb_control \
    __parm [__ebx] [__es __edi] [__ecx]

#pragma aux RdosReqUsbData = \
    CallGate_req_usb_data \
    __parm [__ebx] [__es __edi] [__ecx]

#pragma aux RdosGetUsbDataSize = \
    CallGate_get_usb_data_size \
    "movzx eax,ax" \
    __parm [__ebx] \
    __value [__eax]

#pragma aux RdosWriteUsbData = \
    CallGate_write_usb_data \
    __parm [__ebx] [__es __edi] [__ecx]

#pragma aux RdosReqUsbStatus = \
    CallGate_req_usb_status \
    __parm [__ebx]

#pragma aux RdosWriteUsbStatus = \
    CallGate_write_usb_status \
    __parm [__ebx]

#pragma aux RdosIsUsbTransactionDone = \
    CallGate_is_usb_trans_done \
    CarryToBool \
    __parm [__ebx] \
    __value [__eax]

#pragma aux RdosWasUsbTransactionOk = \
    CallGate_was_usb_trans_ok \
    CarryToBool \
    __parm [__ebx] \
    __value [__eax]

#pragma aux RdosGetUsbCloseCount = \
    CallGate_get_usb_close_count \
    __value [__eax]

#pragma aux RdosOpenICSP = \
    CallGate_open_icsp \
    ValidateHandle \
    __parm [__eax] \
    __value [__ebx]

#pragma aux RdosCloseICSP = \
    CallGate_close_icsp \
    __parm [__ebx]

#pragma aux RdosResetICSP = \
    CallGate_reset_icsp \
    __parm [__ebx]

#pragma aux RdosWriteICSPCommand = \
    CallGate_write_icsp_cmd \
    CarryToBool \
    __parm [__ebx] [__eax] \
    __value [__eax]

#pragma aux RdosWriteICSPData = \
    CallGate_write_icsp_data \
    CarryToBool \
    __parm [__ebx] [__eax] \
    __value [__eax]

#pragma aux RdosReadICSPData = \
    CallGate_read_icsp_data \
    "mov es:[edi],eax" \
    CarryToBool \
    __parm [__ebx] [__es __edi] \
    __value [__eax]

#pragma aux RdosSetCodecGpio0 = \
    CallGate_set_codec_gpio0 \
    __parm [__eax] \
    __modify [__eax]

#pragma aux RdosGetMasterVolume = \
    CallGate_get_master_volume \
    "mov cx,ax" \
    "mov dl,0x7F" \
    "sub dl,al" \
    "movsx edx,dl" \
    "mov eax,200" \
    "imul edx" \
    "sar eax,8" \
    "mov fs:[esi],eax" \
    "mov dl,0x7F" \
    "sub dl,ch" \
    "movsx edx,dl" \
    "mov eax,200" \
    "imul edx" \
    "sar eax,8" \
    "mov es:[edi],eax" \
    __parm [__fs __esi] [__es __edi] \
    __modify [__eax __cx __edx]

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
    __parm [__eax] [__edx] \
    __modify [__eax __ebx __ecx __edx __esi]

#pragma aux RdosGetLineOutVolume = \
    CallGate_get_line_out_volume \
    "mov cx,ax" \
    "mov dl,0x7F" \
    "sub dl,al" \
    "movsx edx,dl" \
    "mov eax,200" \
    "imul edx" \
    "sar eax,8" \
    "mov fs:[esi],eax" \
    "mov dl,0x7F" \
    "sub dl,ch" \
    "movsx edx,dl" \
    "mov eax,200" \
    "imul edx" \
    "sar eax,8" \
    "mov es:[edi],eax" \
    __parm [__fs __esi] [__es __edi] \
    __modify [__eax __cx __edx]

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
    __parm [__eax] [__edx] \
    __modify [__eax __ebx __ecx __edx __esi]

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
    __parm [__eax] [__ecx] [__edx] \
    __modify [__ebx] \
    __value [__ebx]

#pragma aux RdosCloseAudioOutChannel = \
    CallGate_close_audio_out_channel \
    __parm [__ebx]

#pragma aux RdosWriteAudio = \
    "push ds" \
    "mov ds,edx" \
    CallGate_write_audio \
    "pop ds" \
    __parm [__ebx] [__ecx] [__edx __esi] [__es __edi]

#pragma aux RdosOpenFm = \
    CallGate_open_fm \
    ValidateHandle \
    __parm [__eax] \
    __value [__ebx]

#pragma aux RdosCloseFm = \
    CallGate_close_fm \
    __parm [__ebx]

#pragma aux RdosFmWait = \
    CallGate_fm_wait \
    __parm [__ebx] [__eax]

#pragma aux RdosCreateFmInstrument = \
    CallGate_create_fm_instrument \
    ValidateHandle \
    __parm [__ebx] [__eax] [__edx] [__8087] \
    __value [__ebx]

#pragma aux RdosFreeFmInstrument = \
    CallGate_free_fm_instrument \
    __parm [__ebx]

#pragma aux RdosSetFmAttack = \
    CallGate_set_fm_attack \
    __parm [__ebx] [__eax]

#pragma aux RdosSetFmSustain = \
    CallGate_set_fm_sustain \
    __parm [__ebx] [__eax] [__edx]

#pragma aux RdosSetFmRelease = \
    CallGate_set_fm_release \
    __parm [__ebx] [__eax] [__edx]

#pragma aux RdosPlayFmNote = \
    CallGate_play_fm_note \
    __parm [__ebx] [__8087] [__eax] [__edx] [__ecx]
