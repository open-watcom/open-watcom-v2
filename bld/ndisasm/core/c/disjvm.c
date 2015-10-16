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
* Description:  Instruction decoding for Java Virtual Machine.
*
****************************************************************************/


#include <string.h>
#include <ctype.h>
#include "dis.h"
#include "distypes.h"
#include "distjvm.h"

static unsigned GetUByte( void *d, unsigned off )
{
    unsigned_8  data;

    //NYI: have to error check return code
    DisCliGetData( d, off, sizeof(data), &data );
    return( data );
}

static unsigned GetUShort( void *d, unsigned off )
{
    struct {
        unsigned_8 hi;
        unsigned_8 lo;
    }   data;

    //NYI: have to error check return code
    DisCliGetData( d, off, sizeof(data), &data );
    return( (data.hi << 8) | data.lo );
}

static unsigned long GetULong( void *d, unsigned off )
{
    struct {
        unsigned_8 hi;
        unsigned_8 mid_hi;
        unsigned_8 mid_lo;
        unsigned_8 lo;
    }   data;

    //NYI: have to error check return code
    DisCliGetData( d, off, sizeof(data), &data );
    return( ((unsigned long)data.hi << 24)
        |   ((unsigned long)data.mid_hi << 16)
        |   (data.mid_lo << 8)
        |    data.lo );
}

static int GetSByte( void *d, unsigned off )
{
    return( DisSEX( GetUByte( d, off ), 7 ) );
}

static int GetSShort( void *d, unsigned off )
{
    return( DisSEX( GetUShort( d, off ), 15 ) );
}

dis_handler_return JVMSByte( dis_handle *h, void *d, dis_dec_ins *ins )
{
    ins->size += 2;
    ins->num_ops = 1;
    ins->op[0].type = DO_IMMED;
    ins->op[0].value = GetSByte( d, 1 );
    return( DHR_DONE );
}

dis_handler_return JVMSShort( dis_handle *h, void *d, dis_dec_ins *ins )
{
    ins->size += 3;
    ins->num_ops = 1;
    ins->op[0].type = DO_IMMED;
    ins->op[0].value = GetSShort( d, 1 );
    return( DHR_DONE );
}

dis_handler_return JVMUByte( dis_handle *h, void *d, dis_dec_ins *ins )
{
    ins->num_ops = 1;
    ins->op[0].type = DO_IMMED;
    ins->op[0].value = 0 | GetUByte( d, ins->size + 1 );
    ins->size += 2;
    return( DHR_DONE );
}

dis_handler_return JVMUShort( dis_handle *h, void *d, dis_dec_ins *ins )
{
    ins->num_ops = 1;
    ins->op[0].type = DO_IMMED;
    ins->op[0].value = 0 | GetUShort( d, ins->size + 1 );
    ins->size += 3;
    return( DHR_DONE );
}

dis_handler_return JVMNull( dis_handle *h, void *d, dis_dec_ins *ins )
{
    ins->size += 1;
    ins->num_ops = 0;
    return( DHR_DONE );
}

dis_handler_return JVMIInc( dis_handle *h, void *d, dis_dec_ins *ins )
{
    ins->num_ops = 2;
    ins->op[0].type = DO_MEMORY_ABS;
    if( ins->flags.u.jvm & DIF_JVM_WIDE ) {
        ins->op[0].value = 0 | GetUShort( d, ins->size + 1 );
        ins->size += 1;
    } else {
        ins->op[0].value = 0 | GetUByte( d, ins->size + 1 );
    }
    ins->op[1].type = DO_IMMED;
    ins->op[1].value = GetSByte( d, ins->size + 2 );
    ins->size += 3;
    return( DHR_DONE );
}

dis_handler_return JVMWIndex( dis_handle *h, void *d, dis_dec_ins *ins )
{
    if( ins->flags.u.jvm & DIF_JVM_WIDE ) {
        return( JVMUShort( h, d, ins ) );
    } else {
        return( JVMUByte( h, d, ins ) );
    }
}

dis_handler_return JVMWide( dis_handle *h, void *d, dis_dec_ins *ins )
{
    //this affects the following iload,lload,fload,dload,aload,istore,
    //lstore,fstore,dstore,astore,iinc,ret instructions.
    ins->op[0].extra = GetUByte( d, 1 );
    ins->flags.u.jvm |= DIF_JVM_WIDE;
    ins->size += 1;
    return( DHR_CONTINUE );
}

dis_handler_return JVMMultiANewArray( dis_handle *h, void *d, dis_dec_ins *ins )
{
    ins->size += 4;
    ins->num_ops = 2;
    ins->op[0].type = DO_MEMORY_ABS;
    ins->op[0].value = 0 | GetUShort( d, 1 );
    ins->op[1].type = DO_IMMED;
    ins->op[1].value = 0 | GetUByte( d, 3 );
    return( DHR_DONE );
}

dis_handler_return JVMBrShort( dis_handle *h, void *d, dis_dec_ins *ins )
{
    ins->size += 3;
    ins->num_ops = 1;
    ins->op[0].type = DO_RELATIVE;
    ins->op[0].value = 0 | GetUShort( d, 1 );
    return( DHR_DONE );
}

dis_handler_return JVMBrInt( dis_handle *h, void *d, dis_dec_ins *ins )
{
    ins->size += 5;
    ins->num_ops = 1;
    ins->op[0].type = DO_RELATIVE;
    ins->op[0].value = GetULong( d, 1 );
    return( DHR_DONE );
}

dis_handler_return JVMTableSwitch( dis_handle *h, void *d, dis_dec_ins *ins )
{
    unsigned    off;

    //NYI: this instruction can get bigger than 255 bytes (max that can
    // fit in ins->size).
    off = DisCliGetAlign( d, 1, 4 );
    ins->num_ops = 3;
    ins->op[0].type = DO_RELATIVE;
    ins->op[0].value = GetULong( d, off );
    off += 4;
    ins->op[1].type = DO_IMMED;
    ins->op[1].value = GetULong( d, off );
    off += 4;
    ins->op[2].type = DO_IMMED;
    ins->op[2].value = GetULong( d, off );
    off += 4;
    ins->size += off + (ins->op[2].value - ins->op[1].value+1)*4;
    return( DHR_DONE );
}

dis_handler_return JVMLookupSwitch( dis_handle *h, void *d, dis_dec_ins *ins )
{
    unsigned    off;

    //NYI: this instruction can get bigger than 255 bytes (max that can
    // fit in ins->size).
    off = DisCliGetAlign( d, 1, 4 );
    ins->num_ops = 2;
    ins->op[0].type = DO_RELATIVE;
    ins->op[0].value = GetULong( d, off );
    off += 4;
    ins->op[1].type = DO_IMMED;
    ins->op[1].value = GetULong( d, off );
    off += 4;
    ins->size += off + ins->op[1].value * 8;
    return( DHR_DONE );
}

dis_handler_return JVMInterface( dis_handle *h, void *d, dis_dec_ins *ins )
{
    ins->size += 5;
    ins->num_ops = 2;
    ins->op[0].type = DO_MEMORY_ABS;
    ins->op[0].value = 0 | GetUShort( d, 1 );
    ins->op[1].type = DO_IMMED;
    ins->op[1].value = 0 | GetUByte( d, 3 );
    return( DHR_DONE );
}

static size_t JVMInsHook( dis_handle *h, void *d, dis_dec_ins *ins,
        dis_format_flags flags, char *name )
{
    return( 0 );
}

static size_t JVMFlagHook( dis_handle *h, void *d, dis_dec_ins *ins,
        dis_format_flags flags, char *name )
{
    return( 0 );
}

static size_t JVMOpHook( dis_handle *h, void *d, dis_dec_ins *ins,
        dis_format_flags flags, unsigned op_num, char *op_buff, unsigned buff_len )
{
    h = h; d = d; ins = ins; flags = flags; op_num = op_num; op_buff = op_buff; buff_len = buff_len;
    return( 0 );
}

static dis_handler_return JVMDecodeTableCheck( int page, dis_dec_ins *ins )
{
    return( DHR_DONE );
}

static void ByteSwap( dis_handle *h, void *d, dis_dec_ins *ins )
{
    // FIXME !!!!
}

static void JVMPreprocHook( dis_handle *h, void *d, dis_dec_ins *ins )
{
    ByteSwap( h, d, ins );
}

static size_t JVMPostOpHook( dis_handle *h, void *d, dis_dec_ins *ins,
        dis_format_flags flags, unsigned op_num, char *op_buff, unsigned buff_len )
{
    // Nothing to do
    h = h; d = d; ins = ins; flags = flags; op_num = op_num; op_buff = op_buff; buff_len = buff_len;
    return( 0 );
}

const dis_cpu_data JVMData = {
    JVMRangeTable, JVMRangeTablePos, JVMPreprocHook, JVMDecodeTableCheck, JVMInsHook, JVMFlagHook, JVMOpHook, JVMPostOpHook, &JVMMaxInsName, 1
};
