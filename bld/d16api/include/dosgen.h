/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  DOS/16M and DOS/4G generic include file.
*
****************************************************************************/


#ifndef DOSGEN_INCLUDED
#define DOSGEN_INCLUDED

#ifdef NOFAR
#define FarPtr  *
#define HugePtr *
#define NearPtr *
#define cdecl
#define Near
#define far
#define huge
#else
#define FarPtr  __far *
#define HugePtr __huge *
#define NearPtr __near *
#define Near    __near
#endif

#if defined( _M_I86 )
  #define FAR16 __far
#elif defined( _M_IX86 )
  #define FAR16
#else
  #error Unsupported target platform!
#endif

#define FP16        FAR16 *
#define CDECL_FAR16 __cdecl FAR16
#define CDECL_FP16  __cdecl FAR16 *

#ifdef KERNEL
#define NODPMI 1
#endif

#ifdef NODPMI   /* Hard-coded selector values */
#define gdt_sel         0x8         /* GDT */
#define idt_sel         0x10        /* IDT */
#define kernel_sel      0x18        /* Kernel code */
#define kernel_data_sel 0x20        /* Kernel data */
#define psp_sel         0x28        /* PSP */
#define env_sel         0x30        /* Environment */
#define mem0_sel        0x38        /* Phys mem 0 */
#define bios_data_sel   0x40        /* BDA (40:0) */
#define gdti_sel        0x50        /* GDT aux info */
#define debug_gdt       (0x60/8)    /* Debug scratch area */
#define debug_sel       0x60
#define kernelx_sel     0x70        /* Kernel code in extended memory */
#define user_sel        0x80        /* First user selector */
#define user_gdt        16          /* First user GDT index */
#else       /* Indirect selector references */
#define gdt_sel         __d16_selectors.gdt
#define idt_sel         __d16_selectors.idt
#define kernel_sel      __d16_selectors.kernel
#define kernel_data_sel __d16_selectors.kerneld
#define psp_sel         __d16_selectors.psp
#define env_sel         __d16_selectors.env
#define mem0_sel        __d16_selectors.mem0
#define bios_data_sel   __d16_selectors.biosdata
#define gdti_sel        __d16_selectors.gdti
#define kernelx_sel     __d16_selectors.kernelx
#define user_sel        __d16_selectors.user
#define user_gdt        (user_sel >> 3)
#endif

/* Selector access bits */
#define acc_code    0x9A    /* Present, DPL0, exec, read, accessed */
#define acc_cmask   0x83
#define acc_data    0x92    /* Present, DPL0, data, write, accessed */
#define acc_dmask   0x83
#define acc_DPL     0x60    /* DPL mask */
#define acc_LDT     0x82    /* LDT */
#define acc_INT     0x8E    /* Interrupt gate */
#define acc_TSS     0x85    /* TSS */
#define acc_write   0x02    /* X/W access */

#define is_code(a)      (((a) & 0x18) == 0x18)
#define is_data(a)      (((a) & 0x18) == 0x10)
#define is_system(a)    (((a) & 0x10) == 0)
#define Present         0x80    /* Present bit */
#define is_present(a)   (((a) & Present) != 0)

#define SelMask         0xFFF8  /* Selector number mask */

/* Memory allocation strategies */
#define MPreferExt      0
#define MPreferLow      1
#define MForceExt       2
#define MForceLow       3
#define MTransparent    4
#define MTransStack     5


/* DOS/16M types */
typedef char            CHAR;
typedef unsigned char   UCHAR;
typedef UCHAR           BYTE;
typedef short           SHORT;
typedef unsigned short  USHORT;
typedef long            LONG;
typedef unsigned long   ULONG;

typedef USHORT          SELECTOR;
typedef USHORT          OFFSET16;
typedef ULONG           OFFSET32;
typedef ULONG           LINEAR;
typedef ULONG           PHYSICAL;

#endif  /* DOSGEN_INCLUDED */
