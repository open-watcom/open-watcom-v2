/*
 *  Real-mode callbacks for 32-bit Watcom/Borland using PharLap DosX
 *  Metaware uses same functions in the except library (exc_hc.lib)
 *
 *  by G. Vanem 12-Sep-1996
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <dos.h>

#include "wattcp.h"
#include "wdpmi.h"

#if (defined(__WATCOM386__) || defined(__BORLAND386__)) && (DOSX & PHARLAP)
#define __EXC_INTERNAL
#include <mw/exc.h>


#define MAX_WRAPPERS  32
#define MARKER        0xDEADBEEF

typedef enum  {
        RETF = 0xCB,   /* far return       */
        IRET = 0xCF    /* interrupt return */
      } ReturnType;

struct TaskRecord {
       SWI_REGS   rmReg;
       REALPTR    dosReal;
       UINT      *stackStart;
       UINT      *stackTop;
       pmodeHook  callback;
       ULONG      marker;
     } _rmcb [MAX_WRAPPERS];

static char numWrappers = 0;

static REALPTR _dx_alloc_rmode_wrapper (pmodeHook pmHook, 
                                        rmodeHook rmHook, 
                                        int  len, int stack_size,
                                        ReturnType returnType)
{
  #define DOFS  0x2C         /* offset of data section (tiny model)   */
  #define DSIZE 16           /* sizeof data section at wrapper end    */
  #define WOFS  (DOFS+DSIZE) /* offset of userWrapper code (optional) */
                             
  static unsigned char wrapper[] =
  {
    0x16,                     // 00       push ss
    0x06,                     // 01       push es
    0x1E,                     // 02       push ds
    0x0E,                     // 03       push cs
    0x1F,                     // 04       pop  ds           ;DS = CS
    0xF8,                     // 05       clc
    0xE8,0x33,0,              // 06       call userWrapper  ;33 relative (3A-8)
    0x72,0x1C,                // 09       jc short @exit
    0x66,0x60,                // 0B       pushad
    0x66,0xFF,0x36,DOFS+12,0, // 0D       push taskId       ;taskId to call
    0x66,0x6A,0,              // 12       push 0            ;use default selectors
    0xFF,0x36,DOFS+4,0,       // 15       push ourCS
    0x66,0xFF,0x36,DOFS,0,    // 19       push protGlue     ;(*protGlue)()
    0xFF,0x1E,DOFS+8,0,       // 1E       call rm2pmAddr
    0x83,0xC4,14,             // 22       add  sp,14        ;discard used stack
    0x66,0x61,                // 25       popad
    0x1F,                     // 27 @exit:pop  ds
    0x07,                     // 28       pop  es
    0x17,                     // 29       pop  ss
    0xCB,                     // 2A       retf
    0x90,                     // 2B       nop
    0,0,0,0,                  // +0  protGlue  dd ?
    0,0,0,0,                  // +4  ourCS     dw ?,?
    0,0,0,0,                  // +8  rm2pmAddr dd ?
    0,0,0,0                   // +12 taskId    dd ?
  };                          // +16 userWrapper: ..

  FARPTR  pmCbuf, fpWrapper;
  REALPTR rmCbuf, rm2pmAddr, dosReal;
  USHORT  para, paras, left;
  ULONG   cbufSize, i;
  UINT   *stack;

  assert (sizeof(wrapper) == (DOFS+DSIZE));

  if (!pmHook || numWrappers == MAX_WRAPPERS)
     return (0);

  _dx_rmlink_get (&rm2pmAddr,&rmCbuf,&cbufSize,&pmCbuf);
  len  += sizeof(wrapper);
  paras = (len + 15) / 16;
  para  = 0;

  if (_dx_real_above(paras,&para,&left))
  {
    ULONG temp;

    if (_dx_cmem_usage(0,0,&temp,&temp))
       return (0);
    if (_dx_real_above(paras,&para,&left))
       para = 0;
    if (_dx_cmem_usage(0,1,&temp,&temp))
    {
      if (para)
         _dx_real_free (para);
      return (0);
    }
    if (!para)
       return (0);
  }

  for (i = 0; i < MAX_WRAPPERS; i++)
      if (!_rmcb[i].dosReal)
         break;

  *(ULONG*) (wrapper+DOFS+0)  = (ULONG)&PmodeGlue;  /* pmode helper */
  *(ULONG*) (wrapper+DOFS+4)  = My_CS();            /* pmode CS     */
  *(ULONG*) (wrapper+DOFS+8)  = (ULONG)rm2pmAddr;   /* rm->pm addr  */
  *(ULONG*) (wrapper+DOFS+12) = i;                  /* task Id      */
  wrapper [0x2A] = returnType;                      /* RETF or IRET */

  /*
   * lock pages used by callback (for 386|VMM, allthough untested)
   */
  FP_SET (fpWrapper,para << 4, SS_DOSMEM);
  _dx_lock_pgs  (fpWrapper, len);
  _dx_lock_pgsn ((void*)&PmodeGlue, (UINT)&PmGlueEnd - (UINT)&PmodeGlue);

  RP_SET (dosReal,0,para);
  FillRealMem  (dosReal, 0, len);
  WriteRealMem (dosReal, &wrapper, sizeof(wrapper));

  if (rmHook)
       WriteRealMem(dosReal+WOFS,(void*)rmHook,len-sizeof(wrapper));
  else FillRealMem (dosReal+5,0x90,6);

  stack_size += sizeof(UINT) + 3;   /* add a DWORD for marker */
  stack_size &= ~3;                 /* make number of DWORDs  */
  stack = malloc (stack_size);
  if (!stack)
     return (0);

  _rmcb [i].dosReal     = dosReal;
  _rmcb [i].callback    = pmHook;
  _rmcb [i].stackStart  = stack;
  _rmcb [i].stackTop    = stack + stack_size/sizeof(UINT) - 2;
  _rmcb [i].stackTop[1] = MARKER;
  _rmcb [i].marker      = MARKER;
  _dx_lock_pgsn ((void*)stack, stack_size);

#if 0
  printf ("dosReal %08lX, rm2pmAddr %08lX, len %d, stack %08X\n",
          dosReal,rm2pmAddr,len,stack);
#endif
  numWrappers++;

  return (dosReal);
}

/*------------------------------------------------------------------*/

void _dx_free_rmode_wrapper (REALPTR dosAddr)
{
  int i;

  for (i = 0; i < MAX_WRAPPERS; i++)
     if (_rmcb[i].dosReal && _rmcb[i].dosReal == dosAddr)
     {
       struct TaskRecord *tr = _rmcb + i;

       if (tr->marker == MARKER)       /* task marker ok */
          _dx_real_free (RP_SEG(tr->dosReal));

       if (tr->stackTop[1] == MARKER)  /* stack marker ok */
          free (tr->stackStart);
       memset (tr,0,sizeof(*tr));
     }
}

/*------------------------------------------------------------------*/

REALPTR _dx_alloc_rmode_wrapper_retf (pmodeHook pmHook, rmodeHook rmHook,
                                      int len, int stack_size)
{
  return _dx_alloc_rmode_wrapper (pmHook, rmHook, len, stack_size, RETF);
}

REALPTR _dx_alloc_rmode_wrapper_iret (pmodeHook pmHook, int stack_size)
{
  return _dx_alloc_rmode_wrapper (pmHook, NULL, 0, stack_size, IRET);
}

#endif /* (__WATCOM386__ || __BORLAND386__) && (DOSX & PHARLAP) */
