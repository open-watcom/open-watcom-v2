
#ifndef _RDOSDEV_H
#define _RDOSDEV_H

#pragma pack( __push, 1 )

#ifdef __cplusplus
extern "C" {
#endif

#include "rdk.h"

#define GATE_DS_IN      1
#define GATE_ES_IN      2
#define GATE_FS_IN      4
#define GATE_GS_IN      8

#define GATE_DS_OUT     0x10
#define GATE_ES_OUT     0x20
#define GATE_FS_OUT     0x40
#define GATE_GS_OUT     0x80

// handle signatures

#define ADC_HANDLE         0x15FC
#define FM_INSTR_HANDLE    0x17DE
#define DLL_HANDLE16       0x26CF
#define DLL_HANDLE32       0x26DF
#define SPRITE_HANDLE      0x2CF5
#define CRC_HANDLE         0x367A
#define FILE_HANDLE        0x3AB6
#define SERIAL_HANDLE      0x5A45
#define ENV_HANDLE         0x5FAE
#define RW_SECTION_HANDLE  0x67AF
#define WAIT_HANDLE        0x6AFE
#define USB_REQ_HANDLE     0x6B8E
#define SYSLOG_HANDLE      0x703A
#define SECTION_HANDLE     0x7A87
#define IPC_HANDLE         0x7B5A
#define TCP_LISTEN_HANDLE  0x7FAE
#define TCP_SOCKET_HANDLE  0x847F
#define FONT_HANDLE        0x9AF4
#define DIR_HANDLE         0xA765
#define XMS_HANDLE         0xA560
#define SIGNAL_HANDLE      0xADEF
#define PRINTER_HANDLE     0xB63A
#define MEMMAP_HANDLE      0xBA54
#define FM_HANDLE          0xBCAF
#define AUDIO_OUT_HANDLE   0xCEDA
#define HID_HANDLE         0xD736
#define BITMAP_HANDLE      0xDB57
#define INI_HANDLE             0xEAF3
#define USB_PIPE_HANDLE    0xFA3E

// special user-mode gates

#define UserGate_free_mem 0x3e 0x67 0x9a 2 0 0 0 3 0
#define UserGate_create_thread 0x3e 0x67 0x9a 28 0 0 0 3 0

// callback pragmas

typedef void __far (__rdos_gate_callback)();

typedef void __far (__rdos_swap_callback)(char level);

#pragma aux __rdos_swap_callback "*" \
                    __parm __caller [__al] \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_thread_callback)(void *);

#pragma aux __rdos_thread_callback "*" \
                    __parm __caller [__gs __ebx] \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_timer_callback)(int sel, unsigned long expire_msb, unsigned long expire_lsb);

#pragma aux __rdos_timer_callback "*" \
                    __parm __caller [__ecx] [__edx] [__eax] \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_wait_callback)(int wait_obj);

#pragma aux __rdos_wait_callback "*" \
                    __parm __caller [__es] \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_hook_callback)();

#pragma aux __rdos_hook_callback "*" \
                    __parm __caller \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_hook_state_callback)(int thread, char *buf);

#pragma aux __rdos_hook_state_callback "*" \
                    __parm __caller [__ebx] [__es __edi] \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_irq_callback)();

#pragma aux __rdos_irq_callback "*" \
                    __parm __caller \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_handle_delete_callback)(int handle);

#pragma aux __rdos_handle_delete_callback "*" \
                    __parm __caller [__ebx] \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_net_prot_callback)(int size, short packet_type, void *ads, int selector);

#pragma aux __rdos_net_prot_callback "*" \
                    __parm __caller [__ecx] [__dx] [__ds __esi] [__es] \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_net_preview_callback)();

#pragma aux __rdos_net_preview_callback "*" \
                    __parm __caller \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef char* __far (__rdos_net_receive_callback)(int size, char *buf);

#pragma aux __rdos_net_receive_callback "*" \
                    __parm __caller [__ecx] [__es __edi] \
                    __value __struct __routine [__es __edi] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_net_remove_callback)(int size);

#pragma aux __rdos_net_remove_callback "*" \
                    __parm __caller [__ecx] \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef char* __far (__rdos_net_get_buf_callback)(int size);

#pragma aux __rdos_net_get_buf_callback "*" \
                    __parm __caller [__ecx] \
                    __value __struct __routine [__es __edi] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_net_send_callback)(int size, short packet_type, void *dest_ads, int buf_sel);

#pragma aux __rdos_net_send_callback "*" \
                    __parm __caller [__ecx] [__dx] [__ds __esi] [__es] \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef char* __far (__rdos_net_address_callback)();

#pragma aux __rdos_net_address_callback "*" \
                    __parm __caller \
                    __value __struct __routine [__es __edi] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_net_get_address_callback)(int buf_sel);

#pragma aux __rdos_net_get_address_callback "*" \
                    __parm __caller [__es] \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_net_broadcast_callback)(int class_sel, int driver_handle);

#pragma aux __rdos_net_broadcast_callback "*" \
                    __parm __caller [__ds] [__fs] \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_ip_callback)(short opt_size, int data_size, long source_ip, char *opt_data, char *ip_data);

#pragma aux __rdos_ip_callback "*" \
                    __parm __caller [__ax] [__ecx] [__edx] [__ds __esi] [__es __edi] \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_dhcp_option_callback)(int size, char *data);

#pragma aux __rdos_dhcp_option_callback "*" \
                    __parm __caller [__ecx] [__es __edi] \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_disc_assign_callback)();

#pragma aux __rdos_disc_assign_callback "*" \
                    __parm __caller \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_drive_assign_callback)(int disc_handle);

#pragma aux __rdos_drive_assign_callback "*" \
                    __parm __caller [__ebx]  \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_drive_mount_callback)(int disc_handle);

#pragma aux __rdos_drive_mount_callback "*" \
                    __parm __caller [__ebx]  \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_drive_erase_callback)(int disc_handle, int start_sector, int sector_count);

#pragma aux __rdos_drive_erase_callback "*" \
                    __parm __caller [__ebx] [__edx] [__ecx]  \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_disc_change_callback)(int disc_sel);

#pragma aux __rdos_disc_change_callback "*" \
                    __parm __caller [__fs]  \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_init_fs_callback)();

#pragma aux __rdos_init_fs_callback "*" \
                    __parm __caller  \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_fs_format_callback)(void *drive_data, char *fs_name, int sectors);

#pragma aux __rdos_fs_format_callback "*" \
                    __parm __caller [__fs __edx] [__es __edi] [__ecx]  \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void * __far (__rdos_fs_mount_callback)(void *drive_data);

#pragma aux __rdos_fs_mount_callback "*" \
                    __parm __caller [__fs __edx] \
                    __value __struct __routine [__ds __esi] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_fs_flush_callback)(void *fs_data);

#pragma aux __rdos_fs_flush_callback "*" \
                    __parm __caller [__ds __esi]  \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_fs_dismount_callback)(void *fs_data);

#pragma aux __rdos_fs_dismount_callback "*" \
                    __parm __caller [__ds __esi]  \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef long __far (__rdos_fs_info_callback)(void *fs_data);

#pragma aux __rdos_fs_info_callback "*" \
                    __parm __caller [__ds __esi]  \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef int __far (__rdos_fs_allocate_dir_sel_callback)(void *fs_data, char drive, int parent_dir_sel, int parent_dir_entry);

#pragma aux __rdos_fs_allocate_dir_sel_callback "*" \
                    __parm __caller [__ds __esi] [__al] [__ebx] [__edx]  \
                    __value __struct __routine [__ebx] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_fs_free_dir_sel_callback)(void *fs_data, int dir_sel);

#pragma aux __rdos_fs_free_dir_sel_callback "*" \
                    __parm __caller [__ds __esi] [__ebx]  \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_fs_cache_dir_callback)(void *fs_data, int dir_sel, int dir_entry);

#pragma aux __rdos_fs_cache_dir_callback "*" \
                    __parm __caller [__ds __esi] [__ebx] [__edx]  \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_fs_update_dir_callback)(void *fs_data, int dir_dir_entry);

#pragma aux __rdos_fs_update_dir_callback "*" \
                    __parm __caller [__ds __esi] [__edx]  \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_fs_update_file_callback)(void *fs_data, int dir_file_entry);

#pragma aux __rdos_fs_update_file_callback "*" \
                    __parm __caller [__ds __esi] [__edx]  \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef long __far (__rdos_fs_create_dir_callback)(void *fs_data, char *name, int dir_sel);

#pragma aux __rdos_fs_create_dir_callback "*" \
                    __parm __caller [__ds __esi] [__es __edi] [__ebx]  \
                    __value __struct __routine [__edx] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_fs_delete_dir_callback)(void *fs_data, int dir_sel, int dir_entry);

#pragma aux __rdos_fs_delete_dir_callback "*" \
                    __parm __caller [__ds __esi] [__ebx] [__edx]  \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_fs_delete_file_callback)(void *fs_data, int dir_sel, int dir_entry);

#pragma aux __rdos_fs_delete_file_callback "*" \
                    __parm __caller [__ds __esi] [__ebx] [__edx]  \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_fs_rename_file_callback)(char *from, char *to);

#pragma aux __rdos_fs_rename_file_callback "*" \
                    __parm __caller [__fs __esi] [__es __edi]  \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef int __far (__rdos_fs_create_file_callback)(void *fs_data, char *name, int dir_sel, int attrib);

#pragma aux __rdos_fs_create_file_callback "*" \
                    __parm __caller [__ds __esi] [__es __edi] [__ebx] [__ecx] \
                    __value __struct __routine [__edx] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef int __far (__rdos_fs_get_ioctl_callback)(void *fs_data, int file_sel);

#pragma aux __rdos_fs_get_ioctrl_callback "*" \
                    __parm __caller [__ds __esi] [__ebx] \
                    __value __struct __routine [__edx] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_fs_set_file_size_callback)(void *fs_data, int file_sel, int size);

#pragma aux __rdos_fs_set_file_size_callback "*" \
                    __parm __caller [__ds __esi] [__ebx] [__edx] \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef int __far (__rdos_fs_read_file_callback)(void *fs_data, int file_sel, int start, int size, char *buf);

#pragma aux __rdos_fs_read_file_callback "*" \
                    __parm __caller [__ds __esi] [__ebx] [__edx] [__ecx] [__es __edi] \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef int __far (__rdos_fs_write_file_callback)(void *fs_data, int file_sel, int start, int size, char *buf);

#pragma aux __rdos_fs_write_file_callback "*" \
                    __parm __caller [__ds __esi] [__ebx] [__edx] [__ecx] [__es __edi] \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef int __far (__rdos_fs_allocate_file_list_callback)(void *fs_data, int file_sel);

#pragma aux __rdos_fs_allocate_file_list_callback "*" \
                    __parm __caller [__ds __esi] [__ebx] \
                    __value __struct __routine [__edi] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_fs_free_file_list_callback)(void *fs_data, int file_list);

#pragma aux __rdos_fs_free_file_list_callback "*" \
                    __parm __caller [__ds __esi] [__edi] \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_fs_read_block_callback)(void *fs_data, int file_sel, int start, int size, int file_list);

#pragma aux __rdos_fs_read_block_callback "*" \
                    __parm __caller [__ds __esi] [__ebx] [__edx] [__ecx] [__edi] \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_fs_write_block_callback)(void *fs_data, int file_sel, int start, int size, int file_list);

#pragma aux __rdos_fs_write_block_callback "*" \
                    __parm __caller [__ds __esi] [__ebx] [__edx] [__ecx] [__edi] \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

typedef void __far (__rdos_usb_state_callback)(int controller, char device);

#pragma aux __rdos_usb_state_callback "*" \
                    __parm __caller [__ebx] [__al] \
                    __value __struct __routine [__eax] \
                    __modify [__eax __ebx __ecx __edx __esi __edi]

// structures

struct TSpinlock
{
    short value;
};

struct TKernelSection
{
    short value;
    short list;
    short lock;
};

struct TWaitHeader
{
    __rdos_wait_callback *init_proc;
    __rdos_wait_callback *abort_proc;
    __rdos_wait_callback *clear_proc;
    __rdos_wait_callback *idle_proc;
};

struct THandleHeader
{
    short sign;
    short handle;
};

struct TNetDriverTable
{
    __rdos_net_preview_callback *preview_proc;
    __rdos_net_receive_callback *receive_proc;
    __rdos_net_remove_callback *remove_proc;
    __rdos_net_get_buf_callback *get_buf_proc;
    __rdos_net_send_callback *send_proc;
    __rdos_net_address_callback *address_proc;
    __rdos_net_get_address_callback *get_address_proc;
};

struct TDiscSystemHeader
{
    __rdos_disc_assign_callback *disc_assign_proc;
    __rdos_drive_assign_callback *drive_assign1_proc;
    __rdos_drive_assign_callback *drive_assign2_proc;
    __rdos_drive_mount_callback *mount_proc;
    __rdos_drive_erase_callback *erase_proc;
};

struct TFileSystemTable
{
    __rdos_fs_format_callback *format_proc;
    __rdos_fs_mount_callback *mount_proc;
    __rdos_fs_flush_callback *flush_proc;
    __rdos_fs_dismount_callback *dismount_proc;
    __rdos_fs_info_callback *info_proc;
    __rdos_fs_allocate_dir_sel_callback *allocate_dir_sel_proc;
    __rdos_fs_free_dir_sel_callback *free_dir_sel_proc;
    __rdos_fs_cache_dir_callback *cache_dir_proc;
    __rdos_fs_update_dir_callback *update_dir_proc;
    __rdos_fs_update_file_callback *update_file_proc;
    __rdos_fs_create_dir_callback *create_dir_proc;
    __rdos_fs_delete_dir_callback *delete_dir_proc;
    __rdos_fs_delete_file_callback *delete_file_proc;
    __rdos_fs_rename_file_callback *rename_file_proc;
    __rdos_fs_create_file_callback *create_file_proc;
    __rdos_fs_get_ioctl_callback *get_ioctl_proc;
    __rdos_fs_set_file_size_callback *set_file_size_proc;
    __rdos_fs_read_file_callback *read_file_proc;
    __rdos_fs_write_file_callback *write_file_proc;
    __rdos_fs_allocate_file_list_callback *allocate_file_list_proc;
    __rdos_fs_free_file_list_callback *free_file_list_proc;
    __rdos_fs_read_block_callback *read_block_proc;
    __rdos_fs_write_block_callback *write_block_proc;
};

// function definitions

int RdosGetGateDs();
int RdosGetGateEs();
int RdosGetGateFs();
int RdosGetGateGs();

void RdosSetSuccess();
void RdosSetFailure();

void RdosExtendAx();
void RdosExtendBx();
void RdosExtendCx();
void RdosExtendDx();
void RdosExtendSi();
void RdosExtendDi();

void RdosSaveEax();
void RdosRestoreEax();

int RdosIsValidOsGate(int gate);
void RdosRegisterOsGate(int gate, __rdos_gate_callback *callb_proc, const char *name);
void RdosRegisterBimodalUserGate(int gate, __rdos_gate_callback *callb_proc, const char *name);
void RdosRegisterSegBimodalUserGate(int gate, int transfer, __rdos_gate_callback *callb_proc, const char *name);
void RdosRegisterUserGate(int gate, __rdos_gate_callback *callb_proc16, __rdos_gate_callback *callb_proc32, const char *name);
void RdosRegisterSegUserGate(int gate, int transfer, __rdos_gate_callback *callb_proc16, __rdos_gate_callback *callb_proc32, const char *name);

void RdosReturnOk(void);
void RdosReturnFail(void);

void RdosReloadSelector(int sel);
void *RdosSelectorToPointer(int sel);
void *RdosSelectorOffsetToPointer(int sel, long offset);
void *RdosLinearToPointer(int linear);
int RdosPointerToSelector(void *ptr);
int RdosPointerToOffset(void *ptr);

int RdosAllocateGdt(void);
void RdosFreeGdt(int sel);

int RdosAllocateLdt(void);
void RdosFreeLdt(int sel);

int RdosGetSelectorBaseSize(int sel, long *base, long *limit);
void RdosCreateDataSelector16(int sel, long base, long limit);
void RdosCreateDataSelector32(int sel, long base, long limit);
void RdosCreateCodeSelector16(int sel, long base, long limit);
void RdosCreateCodeSelector32(int sel, long base, long limit);
void RdosCreateConformSelector16(int sel, long base, long limit);
void RdosCreateConformSelector32(int sel, long base, long limit);
void RdosCreateCallGateSelector(int sel, void __far (*dest)(), int count);
void RdosCreateIntGateSelector(int intnum, int dpl, void __far (*dest)());
void RdosCreateTrapGateSelector(int intnum, int dpl, void __far (*dest)());

long long RdosGetPageEntry(long linear);
void RdosSetPageEntry(long linear, long long page);

long RdosGetThreadPageEntry(int thread, long linear);
void RdosSetThreadPageEntry(int thread, long linear, long page);

int RdosAllocateBigGlobalSelector(long size);
int RdosAllocateSmallGlobalSelector(long size);

int RdosAllocateBigLocalSelector(long size);
int RdosAllocateSmallLocalSelector(long size);
int RdosAllocateSmallKernelSelector(long size);

void *RdosAllocateBigGlobalMem(long size);
void *RdosAllocateSmallGlobalMem(long size);

void *RdosAllocateBigLocalMem(long size);
void *RdosAllocateSmallLocalMem(long size);
void *RdosAllocateSmallKernelMem(long size);

void RdosFreeMem(int sel);

long RdosAllocateBigGlobalLinear(long size);
long RdosAllocateSmallGlobalLinear(long size);
long RdosAllocateLocalLinear(long size);
long RdosAllocateDebugLocalLinear(long size);
long RdosAllocateVmLinear(long size);

int RdosReserveLocalLinear(long linear, long size);

void RdosFreeLinear(long linear, long size);

long RdosUsedSmallGlobalMem();
long RdosUsedBigGlobalMem();

void *RdosAllocateFixedSystemMem(int sel, long size);
void *RdosAllocateFixedProcessMem(int sel, long size);

long long RdosAllocatePhysical32();
long long RdosAllocatePhysical64();
long long RdosAllocateMultiplePhysical32(int pages);
long long RdosAllocateMultiplePhysical64(int pages);
void RdosFreePhysical(long long ads);

void RdosRegisterSwapProc(__rdos_swap_callback *callb_proc);

void RdosStartTimer(    int sel_id,
                        unsigned long expire_msb,
                        unsigned long expire_lsb,
                        __rdos_timer_callback *callb_proc,
                        int callb_sel);

void RdosStopTimer(     int sel_id);

long RdosGetApicId();
int RdosGetCoreCount();
int RdosGetCore();
int RdosGetCoreNum(int num);
void RdosStartCore(int core);
void RdosSendNmi(int core);
void RdosSendInt(int core, int int_num);
void RdosEnterC3();
void RdosInitFreq();
void RdosUpdateFreq(int diff);

void RdosClearSignal(void);
void RdosSignal(int thread);
void RdosWaitForSignal(void);
void RdosWaitForSignalWithTimeout(long msb, long lsb);

int RdosAddWait(int space_needed, int wait_handle, struct TWaitHeader *wait_table);
void RdosSignalWait(int wait_obj);

void RdosInitSpinlock(struct TSpinlock *spinlock);
short RdosRequestSpinlock(struct TSpinlock *spinlock);
void RdosReleaseSpinlock(struct TSpinlock *spinlock, short flags);

void RdosInitKernelSection(struct TKernelSection *section);
void RdosEnterKernelSection(struct TKernelSection *section);
void RdosLeaveKernelSection(struct TKernelSection *section);
int RdosCondEnterKernelSection(struct TKernelSection *section, int mswait);

void RdosLockScheduler(void);
void RdosUnlockScheduler(void);

void RdosCreateKernelThread(
            int prio,
            int stack,
            __rdos_thread_callback *startup,
            const char *name,
            void *parm);

void RdosCreateKernelProcess(
            int progid,
            int prio,
            int stack,
            __rdos_thread_callback *startup,
            const char *name,
            void *parm);

int RdosThreadToSel(int thread_id);

void RdosHookInitTasking(__rdos_hook_callback *callb_proc);
void RdosHookCreateProcess(__rdos_hook_callback *callb_proc);
void RdosHookTerminateProcess(__rdos_hook_callback *callb_proc);
void RdosHookCreateThread(__rdos_hook_callback *callb_proc);
void RdosHookTerminateThread(__rdos_hook_callback *callb_proc);
void RdosHookInitPci(__rdos_hook_callback *callb_proc);

void RdosHookState(__rdos_hook_state_callback *callb_proc);

void RdosSendEoi(int irq);
int RdosIsIrqFree(int irq);

void RdosRequestIrqHandler(int irq, int prio, __rdos_irq_callback *irq_proc);
void RdosForceLevelIrq(int irq);

void RdosSetupIrqDetect();
int RdosPollIrqDetect();

struct THandleHeader *RdosAllocateHandle(short signature, int size);
void RdosFreeHandle(struct THandleHeader *handle_data);
struct THandleHeader *RdosDerefHandle(short signature, int handle);
void RdosRegisterHandle(short signature, __rdos_handle_delete_callback *delete_proc);

int RdosLockSysEnv(void);
void RdosUnlockSysEnv(void);
int RdosLockProcEnv(void);
void RdosUnlockProcEnv(void);

int RdosGetFocusThread(void);

void RdosRegisterNetClass(char class_id, int ads_size, void *broadcast_ads);
int RdosRegisterNetProtocol(int ads_size, short packet_type, void *my_ads, __rdos_net_prot_callback *packet_callb);
int RdosRegisterNetDriver(char class_id, int max_size, struct TNetDriverTable *table, const char *name);

void RdosNetBroadcast(__rdos_net_broadcast_callback *callb_proc);
void RdosNetReceived(int prot_handle);
void RdosHookIp(char protocol, __rdos_ip_callback *callb_proc);

char *RdosCreateIpHeader(char protocol, char ttl, int data_size, long dest_ip, void *options);
void RdosSendIp(char *data);

char *RdosCreateBroadcastIp(char protocol, char ttl, int data_size, int driver_sel, void *options);
void RdosSendBroadcastIp(char *data, int driver_sel);

void RdosAddDhcpOption(char option_code, __rdos_dhcp_option_callback *callb_proc);

int RdosGetIpCacheSel(long ip);
char *RdosGetIpCacheHostName(long ip);
long RdosGetHostTimeout(int cache_sel);
void RdosUpdateRoundTripTime(int cache_sel, long time);

int RdosQueryUdp(long timeout_ms, short dest_port, long ip, char *buf, int size, char **answer_buf);
void RdosBroadcastDriverUdp(short source, short dest, int driver_sel, char *buf, int size);
void RdosSendDriverUdp(short source, short dest, long ip, int driver_sel, void *driver_dest, char *buf, int size);

void RdosHookInitDisc(struct TDiscSystemHeader *disc_table);
int RdosInstallDisc(int disc_handle, int read_ahead, int *disc_nr);
void RdosRegisterDiscChange(__rdos_disc_change_callback *callb_proc);
void RdosStartDisc(int disc_sel);
void RdosStopDisc(int disc_sel);
void RdosSetDiscParam(  int disc_sel,
                        int bytes_per_sector,
                        int sectors_per_unit,
                        int units,
                        int bios_sectors_per_cyl,
                        int bios_heads);

void RdosWaitForDiscRequest(int disc_sel);
long RdosGetDiscRequest(int disc_sel);
long RdosPollDiscRequest(int disc_sel);
void RdosDiscRequestCompleted(int disc_sel, long disc_handle);
long RdosNewDiscRequest(int disc_sel, int sector, int unit);
long RdosLockDiscRequest(int disc_sel, int sector, int unit);
void RdosModifyDiscRequest(long disc_handle);
void RdosUnlockDiscRequest(long disc_handle);
int RdosGetDiscRequestArray(int disc, int max_entries, long **req_array);

void RdosOpenDrive(int drive, int disc_nr, int start_sector, int sector_count);
void RdosCloseDrive(int drive);
void RdosFlushDrive(int drive);
int RdosGetDriveParam(int drive, int *read_ahead, int *sector_per_unit, int *units);
int RdosLockSector(int drive, int sector, void **data);
void RdosUnlockSector(int handle);
void RdosModifySector(int handle);
void RdosFlushSector(int handle);
int RdosNewSector(int drive, int sector, void **data);
int RdosReqSector(int drive, int sector, void *data);
int RdosDefineSector(int drive, int sector, void *data);
void RdosWaitForSector(int handle);

int RdosCreateDiscSeq(int max_entries);
void RdosModifySeqSector(int seq_handle, int handle);
void RdosPerformDiscSeq(int seq_handle);

void RdosEraseSectors(int drive, int start_sector, int sector_count);
void RdosResetDrive(int drive);

void RdosHookInitFileSystem(__rdos_dhcp_option_callback *callb_proc);
void RdosRegisterFileSystem(char *name, struct TFileSystemTable *table);
void RdosInstallFileSystem(int drive, char *name);
void RdosDemandLoadFileSystem(int drive);
int RdosIsFileSystemAvailable(char *name);
void RdosFormatFileSystem(int drive, char *name, void *mount_data);
void RdosStartFileSystem(int drive, int sectors, void *mount_data);
void RdosStopFileSystem(int drive);

int RdosGetFileListEntry(int file_sel, int pos);
void RdosFreeFileListEntry(int file_sel, int file_list);

int RdosCacheDir(int dir_sel, int dir_entry);
void RdosInsertDirEntry(int dir_sel, int dir_entry);
void RdosInsertFileEntry(int dir_sel, int file_entry);

int RdosGetFileInfo(int handle, char *access, char *drive, int *file_sel);
int RdosDuplFileInfo(char access, char drive, int file_sel);

int RdosOpenKernelFile(const char *FileName, int Mode);
void RdosCloseCFile(int Handle);
long RdosGetCFileSize(int Handle);
void RdosSetCFileSize(int Handle, long Size);
int RdosReadCFile(int Handle, void *Buf, int Size, long Pos);
int RdosWriteCFile(int Handle, const void *Buf, int Size, long Pos);
void RdosGetCFileTime(int Handle, unsigned long *MsbTime, unsigned long *LsbTime);
void RdosSetCFileTime(int Handle, unsigned long MsbTime, unsigned long LsbTime);

void RdosLockFile(int file_sel);
void RdosUnlockFile(int file_sel);

char RdosReadPciByte(char bus, char dev, char func, char reg);
short RdosReadPciWord(char bus, char dev, char func, char reg);
long RdosReadPciDword(char bus, char dev, char func, char reg);

void RdosWritePciByte(char bus, char dev, char func, char reg, char val);
void RdosWritePciWord(char bus, char dev, char func, char reg, short val);
void RdosWritePciDword(char bus, char dev, char func, char reg, long val);

void RdosInitMouse();
void RdosUpdateMouse(int button_state, int delta_x, int delta_y);
void RdosInvertMouse(int delta_x, int delta_y);
void RdosSetMouse(int button_state, int x, int y);
void RdosSetMouseLimit(int max_x, int max_y);

void RdosAddComPort(int controller, int device, int com_sel);
void RdosReserveComLine(char port);
void RdosDeviceSetDtr(char port);
void RdosDeviceResetDtr(char port);
char RdosWaitForLineStateChange(char port);
char RdosGetLineState(char port);

void RdosInitUsbDevice(int usb_dev_sel);
void RdosNotifyUsbAttach(int usb_dev_sel, char port, char speed);
void RdosNotifyUsbDetach(int usb_dev_sel, char port);

void RdosHookUsbAttach(__rdos_usb_state_callback *callb_proc);
void RdosHookUsbDetach(__rdos_usb_state_callback *callb_proc);

int RdosCreateUsbReq(int pipe_handle);
void RdosAddWriteUsbControlReq(int req_handle, int size, int sel);
void RdosAddWriteUsbDataReq(int req_handle, int size, int sel);
void RdosAddReadUsbDataReq(int req_handle, int size, int sel);
void RdosAddUsbStatusInReq(int req_handle);
void RdosAddUsbStatusOutReq(int req_handle);
void RdosStartUsbReq(int req_handle, int signal_thread_sel, int out_buf_size);
void RdosStopUsbReq(int req_handle);
int RdosIsUsbReqStarted(int req_handle);
int RdosIsUsbReqReady(int req_handle);
int RdosGetUsbReqData(int req_handle);
void RdosCloseUsbReq(int req_handle);

short RdosReadCodec(int reg);
void RdosWriteCodec(int reg, short val);

short RdosGetAudioDacRate();
void RdosSetAudioDacRate(short rate);
short RdosGetAudioAdcRate();
void RdosSetAudioAdcRate(short rate);

void RdosOpenAudioOut(short rate);
void RdosCloseAudioOut();
void RdosSendAudioOut(int left_sel, int right_sel, int samples);

int RdosGetUnsignedHidOutput(int Sel, int Usage);
int RdosGetSignedHidOutput(int Sel, int Usage);

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

#pragma aux RdosGetGateDs = \
    "mov eax,[ebp+16]" \
    __value [__eax]

#pragma aux RdosGetGateEs = \
    "mov eax,[ebp+12]" \
    __value [__eax]

#pragma aux RdosGetGateFs = \
    "mov eax,[ebp+8]" \
    __value [__eax]

#pragma aux RdosGetGateGs = \
    "mov eax,[ebp+4]" \
    __value [__eax]

#pragma aux RdosSetSuccess = \
    "clc";

#pragma aux RdosSetFailure = \
    "stc";

#pragma aux RdosSaveEax = \
    "push eax";

#pragma aux RdosRestoreEax = \
    "pop eax";

#pragma aux RdosExtendAx = \
    "movzx eax,ax";

#pragma aux RdosExtendBx = \
    "movzx ebx,bx";

#pragma aux RdosExtendCx = \
    "movzx ecx,cx";

#pragma aux RdosExtendDx = \
    "movzx edx,dx";

#pragma aux RdosExtendSi = \
    "movzx esi,si";

#pragma aux RdosExtendDi = \
    "movzx edi,di";

#pragma aux RdosIsValidOsGate = \
    OsGate_is_valid_osgate  \
    CarryToBool \
    __parm [__eax] \
    __value [__eax]

#pragma aux RdosRegisterOsGate = \
    "push ds" \
    "push cs" \
    "pop ds" \
    OsGate_register_osgate  \
    "pop ds" \
    __parm [__eax] [__esi] [__es __edi]

#pragma aux RdosRegisterBimodalUserGate = \
    "push ds" \
    "push cs" \
    "pop ds" \
    "xor dx,dx" \
    OsGate_register_bimodal_usergate  \
    "pop ds" \
    __parm [__eax] [__esi] [__es __edi]

#pragma aux RdosRegisterSegBimodalUserGate = \
    "push ds" \
    "push cs" \
    "pop ds" \
    OsGate_register_bimodal_usergate  \
    "pop ds" \
    __parm [__eax] [__edx] [__esi] [__es __edi]

#pragma aux RdosRegisterUserGate = \
    "push ds" \
    "push cs" \
    "pop ds" \
    "xor dx,dx" \
    OsGate_register_usergate  \
    "pop ds" \
    __parm [__eax] [__ebx] [__esi] [__es __edi]

#pragma aux RdosRegisterSegUserGate = \
    "push ds" \
    "push cs" \
    "pop ds" \
    OsGate_register_usergate  \
    "pop ds" \
    __parm [__eax] [__edx] [__ebx] [__esi] [__es __edi]

#pragma aux RdosReturnOk = \
    "clc" ;

#pragma aux RdosReturnFail = \
    "stc" ;

#pragma aux RdosReloadSelector = \
    "mov eax,ds" \
    "cmp eax,ebx" \
    "jne NoReloadDs" \
    "mov ds,eax" \
    "NoReloadDs: "\
    "mov eax,es" \
    "cmp eax,ebx" \
    "jne NoReloadEs" \
    "mov es,eax" \
    "NoReloadEs: "\
    "mov eax,fs" \
    "cmp eax,ebx" \
    "jne NoReloadFs" \
    "mov fs,eax" \
    "NoReloadFs: "\
    "mov eax,gs" \
    "cmp eax,ebx" \
    "jne NoReloadGs" \
    "mov gs,eax" \
    "NoReloadGs: "\
    __parm [__ebx] \
    __modify [__eax]

#pragma aux RdosSelectorToPointer = \
    "mov dx,bx" \
    "xor eax,eax" \
    __parm [__ebx] \
    __value [__dx __eax]

#pragma aux RdosSelectorOffsetToPointer = \
    "mov dx,bx" \
    __parm [__ebx] [__eax] \
    __value [__dx __eax]

#pragma aux RdosLinearToPointer = \
    "mov dx,0x20" \
    __parm [__eax] \
    __value [__dx __eax]

#pragma aux RdosPointerToSelector = \
    "movzx ebx,dx" \
    __parm [__edx __eax] \
    __value [__ebx]

#pragma aux RdosPointerToOffset = \
    __parm [__edx __eax] \
    __value [__eax]

#pragma aux RdosAllocateGdt = \
    OsGate_allocate_gdt  \
    "movzx ebx,bx" \
    __value [__ebx]

#pragma aux RdosFreeGdt = \
    OsGate_free_gdt  \
    __parm [__ebx]

#pragma aux RdosAllocateLdt = \
    OsGate_allocate_ldt  \
    "movzx ebx,bx" \
    __value [__ebx]

#pragma aux RdosFreeLdt = \
    OsGate_free_ldt  \
    __parm [__ebx]

#pragma aux RdosGetSelectorBaseSize = \
    OsGate_get_selector_base_size  \
    CarryToBool \
    "mov fs:[esi],edx" \
    "mov es:[edi],ecx" \
    __parm [__ebx] [__fs __esi] [__es __edi] \
    __value [__eax]

#pragma aux RdosCreateDataSelector16 = \
    OsGate_create_data_sel16  \
    __parm [__ebx] [__edx] [__ecx]

#pragma aux RdosCreateDataSelector32 = \
    OsGate_create_data_sel32  \
    __parm [__ebx] [__edx] [__ecx]

#pragma aux RdosCreateCodeSelector16 = \
    OsGate_create_code_sel16  \
    __parm [__ebx] [__edx] [__ecx]

#pragma aux RdosCreateCodeSelector32 = \
    OsGate_create_code_sel32  \
    __parm [__ebx] [__edx] [__ecx]

#pragma aux RdosCreateConformSelector16 = \
    OsGate_create_conform_sel16  \
    __parm [__ebx] [__edx] [__ecx]

#pragma aux RdosCreateConformSelector32 = \
    OsGate_create_conform_sel32  \
    __parm [__ebx] [__edx] [__ecx]

#pragma aux RdosCreateCallGateSelector = \
    "push ds" \
    "push eax" \
    "mov eax,cs" \
    "mov ds,eax" \
    "pop eax" \
    OsGate_create_call_gate_sel32  \
    "pop ds" \
    __parm [__ebx] [__esi] [__ecx]

#pragma aux RdosCreateIntGateSelector = \
    "push ds" \
    "push eax" \
    "mov eax,cs" \
    "mov ds,eax" \
    "pop eax" \
    OsGate_setup_int_gate  \
    "pop ds" \
    __parm [__eax] [__ebx] [__esi]

#pragma aux RdosCreateTrapGateSelector = \
    "push ds" \
    "push eax" \
    "mov eax,cs" \
    "mov ds,eax" \
    "pop eax" \
    OsGate_setup_trap_gate  \
    "pop ds" \
    __parm [__eax] [__ebx] [__esi]

#pragma aux RdosGetPageEntry = \
    OsGate_get_page_entry  \
    __parm [__edx] \
    __value [__ebx __eax]

#pragma aux RdosSetPageEntry = \
    OsGate_set_page_entry  \
    __parm [__edx] [__ebx __eax]

#pragma aux RdosGetThreadPageEntry = \
    OsGate_get_thread_page_entry  \
    __parm [__ebx] [__edx] \
    __value [__eax]

#pragma aux RdosSetThreadPageEntry = \
    OsGate_set_thread_page_entry  \
    __parm [__ebx] [__edx] [__eax]

#pragma aux RdosAllocateBigGlobalSelector = \
    "push es" \
    OsGate_allocate_global_mem  \
    "mov ebx,es" \
    "pop es" \
    __parm [__eax]  \
    __value [__ebx]

#pragma aux RdosAllocateSmallGlobalSelector = \
    "push es" \
    OsGate_allocate_small_global_mem  \
    "mov ebx,es" \
    "pop es" \
    __parm [__eax]  \
    __value [__ebx]

#pragma aux RdosAllocateBigLocalSelector = \
    "push es" \
    OsGate_allocate_big_mem  \
    "mov ebx,es" \
    "pop es" \
    __parm [__eax]  \
    __value [__ebx]

#pragma aux RdosAllocateSmallLocalSelector = \
    "push es" \
    OsGate_allocate_small_mem  \
    "mov ebx,es" \
    "pop es" \
    __parm [__eax]  \
    __value [__ebx]

#pragma aux RdosAllocateSmallKernelSelector = \
    "push es" \
    OsGate_allocate_small_kernel_mem  \
    "mov ebx,es" \
    "pop es" \
    __parm [__eax]  \
    __value [__ebx]

#pragma aux RdosAllocateBigGlobalMem = \
    "push es" \
    OsGate_allocate_global_mem  \
    "mov dx,es" \
    "xor eax,eax" \
    "pop es" \
    __parm [__eax]  \
    __value [__dx __eax]

#pragma aux RdosAllocateSmallGlobalMem = \
    "push es" \
    OsGate_allocate_small_global_mem  \
    "mov dx,es" \
    "xor eax,eax" \
    "pop es" \
    __parm [__eax]  \
    __value [__dx __eax]

#pragma aux RdosAllocateBigLocalMem = \
    "push es" \
    OsGate_allocate_big_mem  \
    "mov dx,es" \
    "xor eax,eax" \
    "pop es" \
    __parm [__eax]  \
    __value [__dx __eax]

#pragma aux RdosAllocateSmallLocalMem = \
    "push es" \
    OsGate_allocate_small_mem  \
    "mov dx,es" \
    "xor eax,eax" \
    "pop es" \
    __parm [__eax]  \
    __value [__dx __eax]

#pragma aux RdosAllocateSmallKernelMem = \
    "push es" \
    OsGate_allocate_small_kernel_mem  \
    "mov dx,es" \
    "xor eax,eax" \
    "pop es" \
    __parm [__eax]  \
    __value [__dx __eax]

#pragma aux RdosFreeMem = \
    "push eax" \
    "push es" \
    "mov es,ebx" \
    UserGate_free_mem  \
    "pop eax" \
    "verr ax" \
    "jz short es_load" \
    "xor eax,eax" \
    "es_load: " \
    "mov es,eax" \
    "pop eax" \
    __parm [__ebx]

#pragma aux RdosAllocateBigGlobalLinear = \
    OsGate_allocate_big_linear  \
    __parm [__eax]  \
    __value [__edx]

#pragma aux RdosAllocateSmallGlobalLinear = \
    OsGate_allocate_small_linear  \
    __parm [__eax]  \
    __value [__edx]

#pragma aux RdosAllocateLocalLinear = \
    OsGate_allocate_local_linear  \
    __parm [__eax]  \
    __value [__edx]

#pragma aux RdosAllocateDebugLocalLinear = \
    OsGate_allocate_debug_local_linear  \
    __parm [__eax]  \
    __value [__edx]

#pragma aux RdosAllocateVmLinear = \
    OsGate_allocate_vm_linear  \
    __parm [__eax]  \
    __value [__edx]

#pragma aux RdosReserveLocalLinear = \
    OsGate_reserve_local_linear  \
    CarryToBool \
    __parm [__edx] [__eax] \
    __value [__eax]

#pragma aux RdosFreeLinear = \
    OsGate_free_linear  \
    __parm [__edx] [__ecx]

#pragma aux RdosUsedBigGlobalMem = \
    OsGate_used_big_linear  \
    __value [__eax]

#pragma aux RdosUsedSmallGlobalMem = \
    OsGate_used_small_linear  \
    __value [__eax]

#pragma aux RdosAllocateFixedSystemMem = \
    "push es" \
    OsGate_allocate_fixed_system_mem  \
    "mov dx,bx" \
    "xor eax,eax" \
    "pop es" \
    __parm [__ebx] [__eax]  \
    __value [__dx __eax]

#pragma aux RdosAllocateFixedProcessMem = \
    "push es" \
    OsGate_allocate_fixed_process_mem  \
    "mov dx,bx" \
    "xor eax,eax" \
    "pop es" \
    __parm [__ebx] [__eax]  \
    __value [__dx __eax]

#pragma aux RdosAllocatePhysical32 = \
    OsGate_allocate_physical32  \
    __value [__ebx __eax]

#pragma aux RdosAllocatePhysical64 = \
    OsGate_allocate_physical64  \
    __value [__ebx __eax]

#pragma aux RdosAllocateMultiplePhysical32 = \
    OsGate_allocate_physical32  \
    __parm [__ecx] \
    __value [__ebx __eax]

#pragma aux RdosAllocateMultiplePhysical64 = \
    OsGate_allocate_physical64  \
    __parm [__ecx] \
    __value [__ebx __eax]

#pragma aux RdosFreePhysical = \
    OsGate_free_physical  \
    __parm [__ebx __eax]

#pragma aux RdosRegisterSwapProc = \
    OsGate_register_swap_proc  \
    __parm [__es __edi]

#pragma aux RdosStartTimer = \
    OsGate_start_timer  \
    __parm [__ebx] [__edx] [__eax] [__es __edi] [__ecx]

#pragma aux RdosStopTimer = \
    OsGate_stop_timer  \
    __parm [__ebx]

#pragma aux RdosGetApicId = \
    OsGate_get_apic_id  \
    __value [__edx]

#pragma aux RdosStartCore = \
    "push fs" \
    "mov fs,ebx" \
    OsGate_start_core  \
    "pop fs" \
    __parm [__ebx]

#pragma aux RdosEnterC3 = \
    OsGate_enter_c3;

#pragma aux RdosInitFreq = \
    OsGate_init_freq;

#pragma aux RdosUpdateFreq = \
    OsGate_update_freq  \
    __parm [__eax]

#pragma aux RdosSendNmi = \
    "push fs" \
    "mov fs,ebx" \
    OsGate_send_nmi  \
    "pop fs" \
    __parm [__ebx]

#pragma aux RdosSendInt = \
    "push fs" \
    "mov fs,ebx" \
    OsGate_send_int  \
    "pop fs" \
    __parm [__ebx] [__eax]

#pragma aux RdosLockScheduler = \
    OsGate_lock_task;

#pragma aux RdosUnlockScheduler = \
    OsGate_unlock_task;

#pragma aux RdosClearSignal = \
    OsGate_clear_signal;

#pragma aux RdosSignal = \
    OsGate_signal \
    __parm [__ebx]

#pragma aux RdosWaitForSignal = \
    OsGate_wait_for_signal;

#pragma aux RdosWaitForSignalWithTimeout = \
    OsGate_wait_for_signal_timeout \
    __parm [__edx] [__eax]

#pragma aux RdosAddWait = \
    "push es" \
    OsGate_add_wait \
    "mov eax,es" \
    "pop es" \
    __parm [__eax] [__ebx] [__es __edi] \
    __value [__eax]

#pragma aux RdosSignalWait = \
    "push es" \
    "mov es,eax" \
    OsGate_signal_wait \
    "pop es" \
    __parm [__eax]

#pragma aux RdosInitSpinlock = \
    "mov word ptr es:[edi],0" \
    __parm [__es __edi]

#pragma aux RdosRequestSpinlock = \
    "pushf" \
    "rs_lock: " \
    "mov ax,es:[edi]" \
    "or ax,ax" \
    "je short rs_get" \
    "sti" \
    0xF3 0x90 \
    "jmp rs_lock" \
    "rs_get: " \
    "cli" \
    "inc ax" \
    "xchg ax,es:[edi]" \
    "or ax,ax" \
    "je rs_done" \
    "jmp rs_lock" \
    "rs_done: "\
    "pop ax" \
    __parm [__es __edi] \
    __value [__ax]

#pragma aux RdosReleaseSpinlock = \
    "push ax" \
    "mov word ptr es:[edi],0" \
    "popf" \
    __parm [__es __edi] [__ax]

#pragma aux RdosInitKernelSection = \
    "mov dword ptr es:[edi],0" \
    "mov word ptr es:[edi+4],0" \
    __parm [__es __edi]

#pragma aux RdosEnterKernelSection = \
    " lock sub word ptr es:[edi],1" \
    " jc short enter_done" \
    " push ds" \
    " push esi" \
    " mov esi,es" \
    " mov ds,esi" \
    " mov esi,edi" \
    OsGate_enter_section \
    " pop esi" \
    " pop ds" \
    "enter_done: " \
    __parm [__es __edi]

#pragma aux RdosLeaveKernelSection = \
    " lock add word ptr es:[edi],1" \
    " jc short leave_done" \
    " push ds" \
    " push esi" \
    " mov esi,es" \
    " mov ds,esi" \
    " mov esi,edi" \
    OsGate_leave_section \
    " pop esi" \
    " pop ds" \
    "leave_done: " \
    __parm [__es __edi]

#pragma aux RdosCondEnterKernelSection = \
    " lock sub word ptr es:[edi],1" \
    " jc short enter_ok" \
    " push ds" \
    " push esi" \
    " mov esi,es" \
    " mov ds,esi" \
    " mov esi,edi" \
    OsGate_cond_enter_section \
    " pop esi" \
    " pop ds" \
    "jc enter_ok" \
    "xor eax,eax" \
    "jmp enter_leave" \
    "enter_ok: " \
    "mov eax,1" \
    "enter_leave: "\
    __parm [__es __edi] [__eax] \
    __value [__eax]

#pragma aux RdosCreateKernelThread = \
    "push ds" \
    "mov edx,fs" \
    "mov ds,edx" \
    "movzx eax,al" \
    UserGate_create_thread \
    "pop ds" \
    __parm [__eax] [__ecx] [__fs __esi] [__es __edi] [__gs __ebx] \
    __modify [__edx]

#pragma aux RdosCreateKernelProcess = \
    "push ds" \
    "mov edx,fs" \
    "mov ds,edx" \
    "movzx eax,al" \
    OsGate_create_process \
    "pop ds" \
    __parm [__eax] [__ecx] [__fs __esi] [__es __edi] [__gs __ebx] \
    __modify [__edx]

#pragma aux RdosThreadToSel = \
    OsGate_thread_to_sel \
    "movzx ebx,bx" \
    __parm [__ebx] \
    __value [__ebx]

#pragma aux RdosHookInitTasking = \
    OsGate_hook_init_tasking \
    __parm [__es __edi]

#pragma aux RdosHookInitPci = \
    OsGate_hook_init_pci \
    __parm [__es __edi]

#pragma aux RdosHookCreateProcess = \
    OsGate_hook_create_process \
    __parm [__es __edi]

#pragma aux RdosHookCreateThread = \
    OsGate_hook_create_thread \
    __parm [__es __edi]

#pragma aux RdosHookTerminateThread = \
    OsGate_hook_terminate_thread \
    __parm [__es __edi]

#pragma aux RdosHookState = \
    OsGate_hook_state \
    __parm [__es __edi]

#pragma aux RdosSendEoi = \
    OsGate_send_eoi \
    __parm [__eax]

#pragma aux RdosForceLevelIrq = \
    OsGate_force_level_irq \
    __parm [__eax]

#pragma aux RdosRequestIrqHandler = \
    "push ds" \
    "push es" \
    "push eax" \
    "mov eax,16" \
    OsGate_allocate_small_global_mem  \
    "mov eax,es" \
    "mov ds,eax" \
    "pop eax" \
    "pop es" \
    "mov ah,dl" \
    OsGate_request_irq_handler \
    "pop ds" \
    __parm [__eax] [__edx] [__es __edi]

#pragma aux RdosSetupIrqDetect = \
    OsGate_setup_irq_detect;

#pragma aux RdosPollIrqDetect = \
    OsGate_setup_irq_detect \
    __value [__eax]

#pragma aux RdosGetApicId = \
    OsGate_get_apic_id \
    __value [__edx]

#pragma aux RdosGetCoreCount = \
    OsGate_get_core_count \
    "movzx ecx,cx" \
    __value [__ecx]

#pragma aux RdosGetCore = \
    "push fs" \
    OsGate_get_core \
    "mov eax,fs" \
    "pop fs" \
    __value [__eax]

#pragma aux RdosGetCoreNum = \
    "push fs" \
    OsGate_get_core_num \
    "mov eax,fs" \
    "pop fs" \
    __parm [__eax] \
    __value [__eax]

#pragma aux RdosSendNmi = \
    "push fs" \
    "mov fs,eax" \
    OsGate_send_nmi \
    "pop fs" \
    __parm [__eax]

#pragma aux RdosSendInt = \
    "push fs" \
    "mov fs,edx" \
    OsGate_send_int \
    "pop fs" \
    __parm [__edx] [__eax]

#pragma aux RdosAllocateHandle = \
    "push ds" \
    OsGate_allocate_handle \
    "mov dx,ds" \
    "pop ds" \
    __parm [__ax] [__ecx] \
    __value [__dx __ebx]

#pragma aux RdosFreeHandle = \
    "push ds" \
    "mov ds,edx" \
    OsGate_free_handle \
    "pop ds" \
    __parm [__dx __ebx]

#pragma aux RdosDerefHandle = \
    "push ds" \
    OsGate_deref_handle \
    "jnc ok" \
    "xor edx,edx" \
    "xor ebx,ebx" \
    "jmp done" \
    "ok: "\
    "mov dx,ds" \
    "done: "\
    "pop ds" \
    __parm [__ax] [__ebx] \
    __value [__dx __ebx]

#pragma aux RdosRegisterHandle = \
    OsGate_register_handle \
    __parm [__ax] [__es __edi]

#pragma aux RdosLockSysEnv = \
    OsGate_lock_sys_env \
    __value [__ebx]

#pragma aux RdosUnlockSysEnv = \
    OsGate_unlock_sys_env;

#pragma aux RdosLockProcEnv = \
    OsGate_lock_proc_env \
    __value [__ebx]

#pragma aux RdosUnlockProcEnv = \
    OsGate_unlock_proc_env;

#pragma aux RdosGetFocusThread = \
    OsGate_get_focus_thread \
    "movzx eax,ax" \
    __value [__eax]

#pragma aux RdosRegisterNetClass = \
    "push ds" \
    "mov ds,edx" \
    OsGate_register_net_class \
    "pop ds" \
    __parm [__al] [__ecx] [__dx __esi]

#pragma aux RdosRegisterNetProtocol = \
    "push ds" \
    "mov ds,ebx" \
    OsGate_register_net_protocol \
    "pop ds" \
    __parm [__ecx] [__dx] [__bx __esi] [__es __edi] \
    __value [__ebx]

#pragma aux RdosRegisterNetDriver = \
    "push ds" \
    "mov ds,edx" \
    OsGate_register_net_protocol \
    "pop ds" \
    __parm [__al] [__ecx] [__dx __esi] [__es __edi] \
    __value [__ebx]

#pragma aux RdosNetBroadcast = \
    OsGate_net_broadcast \
    __parm [__es __edi]

#pragma aux RdosNetReceived = \
    OsGate_net_received \
    __parm [__ebx]

#pragma aux RdosHookIp = \
    OsGate_hook_ip \
    __parm [__al] [__es __edi]

#pragma aux RdosCreateIpHeader = \
    "push ds" \
    "mov ds,ebx" \
    OsGate_create_ip_header \
    "pop ds" \
    __parm [__al] [__ah] [__ecx] [__edx] [__bx __esi] \
    __value [__es __edi]

#pragma aux RdosSendIp = \
    OsGate_send_ip_data \
    __parm [__es __edi]

#pragma aux RdosCreateBroadcastIp = \
    "push ds" \
    "mov ds,ebx" \
    OsGate_create_broadcast_ip \
    "pop ds" \
    __parm [__al] [__ah] [__ecx] [__fs] [__bx __esi] \
    __value [__es __edi]

#pragma aux RdosSendBroadcastIp = \
    OsGate_send_broadcast_ip \
    __parm [__es __edi] [__fs]

#pragma aux RdosAddDhcpOption = \
    OsGate_add_dhcp_option \
    __parm [__al] [__es __edi]

#pragma aux RdosGetIpCacheSel = \
    "push es" \
    OsGate_lookup_ip_cache \
    "mov eax,es" \
    "pop es" \
    __parm [__edx] \
    __value [__eax]

#pragma aux RdosGetIpCacheHostName = \
    OsGate_lookup_ip_cache \
    __parm [__edx] \
    __value [__es __edi]

#pragma aux RdosGetHostTimeout = \
    "push ds" \
    "mov ds,ebx" \
    OsGate_get_host_timeout \
    "pop ds" \
    __parm [__ebx] \
    __value [__eax]

#pragma aux RdosUpdateRoundTripTime = \
    "push ds" \
    "mov ds,ebx" \
    OsGate_update_round_trip_time \
    "pop ds" \
    __parm [__ebx] [__eax]

#pragma aux RdosBroadcastDriverUdp = \
    OsGate_broadcast_driver_udp \
    __parm [__si] [__bx] [__fs] [__es __edi] [__ecx]

#pragma aux RdosSendDriverUdp = \
    "push ds" \
    "push eax" \
    "mov eax,gs" \
    "mov ds,eax" \
    "pop eax" \
    OsGate_send_driver_udp \
    "pop ds" \
    __parm [__ax] [__bx] [__edx] [__fs] [__gs __esi] [__es __edi] [__ecx]

#pragma aux RdosQueryUdp = \
    OsGate_query_udp \
    "mov fs:[esi],edi" \
    "mov fs:[esi+04],es" \
    "movzx ecx,cx" \
    __parm [__eax] [__bx] [__edx] [__es __edi] [__ecx] [__fs __esi] \
    __value [__ecx] \
    __modify [__es __esi]

#pragma aux RdosHookInitDisc = \
    OsGate_hook_init_disc \
    __parm [__es __edi]

#pragma aux RdosInstallDisc = \
    OsGate_install_disc \
    "movzx eax,al" \
    "mov es:[edi],eax" \
    "movzx ebx,bx" \
    __parm [__ebx] [__ecx] [__es __edi] \
    __value [__ebx] \
    __modify [__eax]

#pragma aux RdosRegisterDiscChange = \
    OsGate_register_disc_change \
    __parm [__es __edi]

#pragma aux RdosStartDisc = \
    OsGate_start_disc \
    __parm [__ebx]

#pragma aux RdosStopDisc = \
    OsGate_stop_disc \
    __parm [__ebx]

#pragma aux RdosSetDiscParam = \
    OsGate_set_disc_param \
    __parm [__ebx] [__ecx] [__eax] [__edx] [__esi] [__edi]

#pragma aux RdosWaitForDiscRequest = \
    OsGate_wait_for_disc_request \
    __parm [__ebx]

#pragma aux RdosDiscRequestCompleted = \
    OsGate_disc_request_completed \
    __parm [__ebx] [__edi]

#pragma aux RdosNewDiscRequest = \
    OsGate_new_disc_request \
    __parm [__ebx] [__eax] [__edx] \
    __value [__edi]

#pragma aux RdosLockDiscRequest = \
    OsGate_lock_disc_request \
    __parm [__ebx] [__eax] [__edx] \
    __value [__edi]

#pragma aux RdosModifyDiscRequest = \
    OsGate_modify_disc_request \
    __parm [__edi]

#pragma aux RdosUnlockDiscRequest = \
    OsGate_unlock_disc_request \
    __parm [__edi]

#pragma aux RdosGetDiscRequestArray = \
    OsGate_get_disc_request_array \
    "mov es:[edi],esi" \
    "mov esi,0x20" \
    "mov es:[edi+4],esi" \
    __parm [__ebx] [__ecx] [__es __edi] \
    __value [__ecx] \
    __modify [__esi]

#pragma aux RdosOpenDrive = \
    "mov ah,bl" \
    OsGate_open_drive \
    __parm [__eax] [__ebx] [__edx] [__ecx] \
    __modify [__eax]

#pragma aux RdosCloseDrive = \
    OsGate_close_drive \
    __parm [__eax]

#pragma aux RdosFlushDrive = \
    OsGate_flush_drive \
    __parm [__eax]

#pragma aux RdosGetDriveParam = \
    "push edi" \
    "push esi" \
    OsGate_flush_drive \
    "mov gs:[edx],eax" \
    "movzx eax,si" \
    "pop esi" \
    "mov fs:[esi],eax" \
    "movzx eax,di" \
    "pop edi" \
    "mov es:[edi],eax" \
    __parm [__eax] [__gs __edx] [__fs __esi] [__es __edi] \
    __value [__ecx] \
    __modify [__eax]

#pragma aux RdosLockSector = \
    OsGate_lock_sector \
    "mov es:[edi],esi" \
    "mov esi,0x20" \
    "mov es:[edi+4],esi" \
    __parm [__eax] [__edx] [__es __edi] \
    __value [__ebx] \
    __modify [__esi]

#pragma aux RdosUnlockSector = \
    OsGate_unlock_sector \
    __parm [__ebx]

#pragma aux RdosModifySector = \
    OsGate_modify_sector \
    __parm [__ebx]

#pragma aux RdosFlushSector = \
    OsGate_flush_sector \
    __parm [__ebx]

#pragma aux RdosNewSector = \
    OsGate_new_sector \
    "mov es:[edi],esi" \
    "mov esi,0x20" \
    "mov es:[edi+4],esi" \
    __parm [__eax] [__edx] [__es __edi] \
    __value [__ebx] \
    __modify [__esi]

#pragma aux RdosReqSector = \
    OsGate_req_sector \
    __parm [__eax] [__edx] [__es __esi] \
    __value [__ebx]

#pragma aux RdosDefineSector = \
    OsGate_define_sector \
    __parm [__eax] [__edx] [__es __esi] \
    __value [__ebx]

#pragma aux RdosWaitForSector = \
    OsGate_wait_for_sector \
    __parm [__edx]

#pragma aux RdosCreateDiscSeq = \
    OsGate_create_disc_seq \
    "movzx eax,ax" \
    __parm [__ecx] \
    __value [__eax]

#pragma aux RdosModifySeqSector = \
    OsGate_modify_seq_sector \
    __parm [__eax] [__ebx]

#pragma aux RdosPerformDiscSeq = \
    OsGate_perform_disc_seq \
    __parm [__eax]

#pragma aux RdosEraseSectors = \
    OsGate_erase_sectors \
    __parm [__eax] [__edx] [__ecx]

#pragma aux RdosResetDrive = \
    OsGate_reset_drive \
    __parm [__eax]

#pragma aux RdosHookInitFileSystem = \
    OsGate_hook_init_file_system \
    __parm [__es __edi]

#pragma aux RdosRegisterFileSystem = \
    "push ds" \
    "mov ds,edx" \
    OsGate_register_file_system \
    "pop ds" \
    __parm [__dx __esi] [__es __edi]

#pragma aux RdosInstallFileSystem = \
    OsGate_install_file_system \
    __parm [__eax] [__es __edi]

#pragma aux RdosDemandLoadFileSystem = \
    OsGate_demand_load_file_system \
    __parm [__eax]

#pragma aux RdosIsFileSystemAvailable = \
    OsGate_is_file_system_available \
    CarryToBool \
    __parm [__es __edi] \
    __value [__eax]

#pragma aux RdosFormatFileSystem = \
    OsGate_format_file_system \
    __parm [__eax] [__es __edi] [__fs __edx]

#pragma aux RdosStartFileSystem = \
    OsGate_start_file_system \
    __parm [__eax] [__ecx] [__fs __edx]

#pragma aux RdosStopFileSystem = \
    OsGate_stop_file_system \
    __parm [__eax]

#pragma aux RdosGetFileListEntry = \
    OsGate_get_file_list_entry \
    __parm [__ebx] [__edx] \
    __value [__eax]

#pragma aux RdosFreeFileListEntry = \
    OsGate_free_file_list_entry \
    __parm [__ebx] [__edi]

#pragma aux RdosCacheDir = \
    OsGate_cache_dir \
    __parm [__ebx] [__edx] \
    __value [__ebx]

#pragma aux RdosInsertDirEntry = \
    OsGate_insert_dir_entry \
    __parm [__ebx] [__edx]

#pragma aux RdosInsertFileEntry = \
    OsGate_insert_file_entry \
    __parm [__ebx] [__edx]

#pragma aux RdosGetFileInfo = \
    OsGate_get_file_info \
    "push eax" \
    CarryToBool \
    "mov es:[edi],cl" \
    "mov fs:[esi],ch" \
    "pop ecx" \
    "movzx ecx,cx" \
    "mov gs:[edx],ecx" \
    __parm [__ebx] [__es __edi] [__fs __esi] [__gs __edx] \
    __value [__eax] \
    __modify [__ecx]

#pragma aux RdosDuplFileInfo = \
    OsGate_dupl_file_info \
    __parm [__cl] [__ch] [__eax] \
    __value [__ebx]

#pragma aux RdosLockFile = \
    OsGate_lock_file \
    CarryToBool \
    __parm [__eax] \
    __value [__eax]

#pragma aux RdosUnlockFile = \
    OsGate_unlock_file \
    CarryToBool \
    __parm [__eax] \
    __value [__eax]

#pragma aux RdosOpenKernelFile = \
    OsGate_open_kernel_file \
    ValidateHandle  \
    __parm [__es __edi] [__cx] \
    __value [__ebx]

#pragma aux RdosCloseCFile = \
    OsGate_close_c_file  \
    __parm [__ebx]

#pragma aux RdosGetCFileSize = \
    OsGate_get_c_file_size  \
    ValidateEax \
    __parm [__ebx]  \
    __value [__eax]

#pragma aux RdosSetCFileSize = \
    OsGate_set_c_file_size  \
    __parm [__ebx] [__eax]

#pragma aux RdosReadCFile = \
    OsGate_read_c_file  \
    ValidateEax \
    __parm [__ebx] [__es __edi] [__ecx] [__edx]  \
    __value [__eax] \
    __modify [__edx]

#pragma aux RdosWriteCFile = \
    OsGate_write_c_file  \
    ValidateEax \
    __parm [__ebx] [__es __edi] [__ecx] [__edx]  \
    __value [__eax] \
    __modify [__edx]

#pragma aux RdosGetCFileTime = \
    OsGate_get_c_file_time  \
    "mov fs:[esi],edx" \
    "mov es:[edi],eax" \
    __parm [__ebx] [__fs __esi] [__es __edi]  \
    __modify [__eax __edx]

#pragma aux RdosSetCFileTime = \
    OsGate_set_c_file_time  \
    __parm [__ebx] [__edx] [__eax]

#pragma aux RdosReadPciByte = \
    OsGate_read_pci_byte \
    __parm [__bh] [__bl] [__ch] [__cl] \
    __value [__al]

#pragma aux RdosReadPciWord = \
    OsGate_read_pci_word \
    __parm [__bh] [__bl] [__ch] [__cl] \
    __value [__ax]

#pragma aux RdosReadPciDword = \
    OsGate_read_pci_dword \
    __parm [__bh] [__bl] [__ch] [__cl] \
    __value [__eax]

#pragma aux RdosWritePciByte = \
    OsGate_write_pci_byte \
    __parm [__bh] [__bl] [__ch] [__cl] [__al]

#pragma aux RdosWritePciWord = \
    OsGate_write_pci_word \
    __parm [__bh] [__bl] [__ch] [__cl] [__ax]

#pragma aux RdosWritePciDword = \
    OsGate_write_pci_dword \
    __parm [__bh] [__bl] [__ch] [__cl] [__eax]

#pragma aux RdosInitMouse = \
    OsGate_init_mouse;

#pragma aux RdosUpdateMouse = \
    OsGate_update_mouse \
    __parm [__eax] [__ecx] [__edx]

#pragma aux RdosInvertMouse = \
    OsGate_invert_mouse \
    __parm [__ecx] [__edx]

#pragma aux RdosSetMouse = \
    OsGate_set_mouse \
    __parm [__eax] [__ecx] [__edx]

#pragma aux RdosSetMouseLimit = \
    OsGate_set_mouse_limit \
    __parm [__ecx] [__edx]

#pragma aux RdosAddComPort = \
    "push ds" \
    "mov ds,ebx" \
    OsGate_add_com_port \
    "pop ds" \
    __parm [__ebx] [__eax] [__edx]

#pragma aux RdosReserveComLine = \
    OsGate_reserve_com_line \
    __parm [__al]

#pragma aux RdosDeviceSetDtr = \
    OsGate_device_set_dtr \
    __parm [__al]

#pragma aux RdosDeviceResetDtr = \
    OsGate_device_reset_dtr \
    __parm [__al]

#pragma aux RdosWaitForLineStateChange = \
    OsGate_wait_for_line_state \
    __parm [__al] \
    __value [__al]

#pragma aux RdosGetLineState = \
    OsGate_get_line_state \
    __parm [__al] \
    __value [__al]

#pragma aux RdosInitUsbDevice = \
    "push ds" \
    "mov ds,edx" \
    OsGate_init_usb_device \
    "pop ds" \
    __parm [__edx]

#pragma aux RdosNotifyUsbDetach = \
    "push ds" \
    "mov ds,edx" \
    OsGate_notify_usb_detach \
    "pop ds" \
    __parm [__edx] [__al]

#pragma aux RdosHookUsbAttach = \
    OsGate_hook_usb_attach \
    __parm [__es __edi]

#pragma aux RdosHookUsbDetach = \
    OsGate_hook_usb_detach \
    __parm [__es __edi]

#pragma aux RdosCreateUsbReq = \
    OsGate_create_usb_req \
    __parm [__ebx] \
    __value [__ebx]

#pragma aux RdosAddWriteUsbControlReq = \
    OsGate_add_write_usb_control_req \
    __parm [__ebx] [__ecx] [__es]

#pragma aux RdosAddWriteUsbDataReq = \
    OsGate_add_write_usb_data_req \
    __parm [__ebx] [__ecx] [__es]

#pragma aux RdosAddReadUsbDataReq = \
    OsGate_add_read_usb_data_req \
    __parm [__ebx] [__ecx] [__es]

#pragma aux RdosAddUsbStatusInReq = \
    OsGate_add_usb_status_in_req \
    __parm [__ebx]

#pragma aux RdosAddUsbStatusOutReq = \
    OsGate_add_usb_status_out_req \
    __parm [__ebx]

#pragma aux RdosStartUsbReq = \
    OsGate_start_usb_req \
    __parm [__ebx] [__eax] [__ecx]

#pragma aux RdosStopUsbReq = \
    OsGate_stop_usb_req \
    __parm [__ebx]

#pragma aux RdosIsUsbReqStarted = \
    OsGate_is_usb_req_started \
    CarryToBool \
    __parm [__ebx] \
    __value [__eax]

#pragma aux RdosIsUsbReqReady = \
    OsGate_is_usb_req_ready \
    CarryToBool \
    __parm [__ebx] \
    __value [__eax]

#pragma aux RdosGetUsbReqData = \
    OsGate_get_usb_req_data \
    "movzx ecx,cx" \
    __parm [__ebx] \
    __value [__ecx]

#pragma aux RdosCloseUsbReq = \
    OsGate_close_usb_req \
    __parm [__ebx]

#pragma aux RdosReadCodec = \
    OsGate_read_codec \
    __parm [__ebx] \
    __value [__ax]

#pragma aux RdosWriteCodec = \
    OsGate_write_codec \
    __parm [__ebx] [__ax]

#pragma aux RdosGetAudioDacRate = \
    OsGate_get_audio_dac_rate \
    __value [__ax]

#pragma aux RdosSetAudioDacRate = \
    OsGate_set_audio_dac_rate \
    __parm [__ax]

#pragma aux RdosGetAudioAdcRate = \
    OsGate_get_audio_adc_rate \
    __value [__ax]

#pragma aux RdosSetAudioAdcRate = \
    OsGate_set_audio_adc_rate \
    __parm [__ax]

#pragma aux RdosOpenAudioOut = \
    OsGate_open_audio_out \
    __parm [__ax]

#pragma aux RdosCloseAudioOut = \
    OsGate_close_audio_out;

#pragma aux RdosSendAudioOut = \
    "push ds" \
    "push es" \
    "mov ds,eax" \
    "mov es,edx" \
    OsGate_send_audio_out \
    "pop es" \
    "pop ds" \
    __parm [__eax] [__edx] [__ecx]

#ifdef __cplusplus
}
#endif

#pragma pack( __pop )

#endif
