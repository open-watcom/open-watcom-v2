/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2020-2020 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "global.h"
#include "rccore.h"
#include "param.h"
#include "rcvars.h"
#include "rcmem.h"
#include "cvttable.h"
#include "unitable.h"


static int getcharUTF8( const char **p, uint_32 *c )
{
    int     len;
    int     i;
    uint_32 value;

    value = *c;
    len = CharSetLen[value];
    if( len == 1 ) {
        value &= 0x1F;
    } else if( len == 2 ) {
        value &= 0x0F;
    } else if( len == 3 ) {
        value &= 0x07;
    } else if( len == 4 ) {
        value &= 0x03;
    } else if( len == 5 ) {
        value &= 0x01;
    } else {
        return( 0 );
    }
    for( i = 0; i < len; ++i ) {
        value = ( value << 6 ) + ( **p & 0x3F );
        (*p)++;
    }
    *c = value;
    return( len );
}


static size_t UTF8StringToUnicode( size_t len, const char *str, char *buf )
/*************************************************************************/
{
    size_t          ret;
    size_t          outlen;
    uint_32         unicode;
    size_t          i;

    ret = 0;
    if( len > 0 ) {
        if( buf == NULL ) {
            outlen = 0;
        } else {
            outlen = len;
        }
        for( i = 0; i < len; i++ ) {
            unicode = (unsigned char)*str++;
            i += getcharUTF8( &str, &unicode );
            if( ret < outlen ) {
                *buf++ = (char)unicode;
                *buf++ = (char)( unicode >> 8 );
                ret++;
            }
        }
    }
    return( ret * 2 );
}

void SetUTF8toUnicode( void )
{
    ConvToUnicode = UTF8StringToUnicode;
}

void SetUTF8toMultiByte( void )
{
//    ConvToMultiByte = UTF8StringToMultiByte;
}
