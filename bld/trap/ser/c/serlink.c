/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Serial link communications core.
*
****************************************************************************/


#include <string.h>
#include "trpimp.h"
#include "trperr.h"
#include "serial.h"
#include "packet.h"
#ifdef SERVER
#include "servio.h"
#endif
#include "serlink.h"

static int BaudCounter;                 /* baud rate counter */
static int LastResponse = SDATA_NAK;    /* last response holder */
static int Errors = 0;                  /* Errors so far */
static int PrevErrors = 0;              /* Errors of previous BlockSend() operation */
static unsigned SendBlkNo = 0;          /* next block number to send */
static unsigned ReceiveBlkNo = 0;       /* next block number to receive */
static unsigned BytesReceived;          /* # bytes from last receive */

int MaxBaud;

#define BAUD_ENTRY( x ) #x, sizeof( #x ) - 1, TEST_TIME( x )

baud_entry BaudTable[] = {
    BAUD_ENTRY( 115200 ),
    BAUD_ENTRY( 57600 ),
    BAUD_ENTRY( 38400 ),
    BAUD_ENTRY( 19200 ),
    BAUD_ENTRY( 9600 ),
    BAUD_ENTRY( 4800 ),
    BAUD_ENTRY( 2400 ),
    BAUD_ENTRY( 1200 ),
    "0", 1, 0,
};

static void SyncPoint( unsigned tick )
{
    unsigned    new_tick;

    new_tick = GetTimerTicks();
    if( tick > new_tick ) {
        Wait( tick - new_tick );
    }
}


static bool SenderHandshake( void )
{
    unsigned wait_time;     /* used to test for time-out */
    int      reply;         /* storing data received from other machine */

    wait_time = GetTimerTicks() + SYNC_TIME_OUT;   /* limit for time out */
    if( MaxBaud == MIN_BAUD )
        wait_time += SEC2TICK( 1 );
    SendByte( SYNC_BYTE );      /* send SYNC_BYTE */
    for( ;; ) {                 /* loop until ACK received or time out */
        reply = WaitByte( 1 );  /* get reply */
        if( reply == SDATA_ACK )
            break;       /* ACK received; go to next operation */
        if( reply == SDATA_HI ) {             /* return HI received */
            SendByte( SDATA_HI );
        } else if( GetTimerTicks() > wait_time ) {    /* time out */
            return( false );
        }                            /* not yet time out; loop */
    }
    SendByte( SYNC_END );
    return( true );
}

/* This routine is one side of the set baud rate routine -- sender
   Result:   true or false  */

static bool SetBaudSender( void )
{
    int         data;           /* storing sync string data to other machine */
    int         i;              /* loop index */
    unsigned    wait_time;

    if( !SenderHandshake() )
        return( false );
    /* sync byte received ... send string */
    StartBlockTrans();
    for( i = data = 0; i < SYNC_LEN; i++, data = (data + SYNC_INC) & 0xff ) {
        SendByte( data );            /* send sync string bytes */
    }
    StopBlockTrans();
    wait_time = GetTimerTicks() + SYNC_TIME_OUT;    /* limit for time out */
    /* If MaxBaud == MIN_BAUD, we're talking over a modem and it might
       have buffered characters that haven't been transmitted yet. */
    if( MaxBaud == MIN_BAUD )
        wait_time += SEC2TICK( 2 );
    for( ;; ) {
        if( WaitByte( 1 ) == SDATA_TAK ) {
            SendByte( SDATA_ACK );
            return( WaitByte( MSEC2TICK( 500 ) ) == SDATA_TAK );
        } else if( GetTimerTicks() >= wait_time ) {
              /* break not found; other end have not acknowledged string */
            return( false );
        }
    }
}


/*========================================================================*/
/*========================================================================*/


/* This routine determines if speed sync string received ok.
   Input:    Receive buffer should have sync string
   Return:   true or false  */

static bool CheckSyncString( void )
{
    int syn_c;         /* correct sync string bytes value */
    int i;             /* loop index */
    unsigned    wait;

    if( CheckPendingError() )
        return( false );
    wait = (MaxBaud == MIN_BAUD) ? SEC2TICK( 2 ) : MSEC2TICK( 250 );
    for( syn_c = i = 0; i < SYNC_LEN; ++i, syn_c = (syn_c + SYNC_INC) & 0xff ) {
        if( WaitByte( wait ) != syn_c ) {  /* error -- timeout or incorrect data */
            return( false );
        }
        wait = MSEC2TICK( 250 );
    }
    return( true );
}


/*========================================================================*/
/*========================================================================*/

static bool ReceiverHandshake( void )
{
    int         reply;         /* storing data received from other machine */
    unsigned    wait_time;

    wait_time = GetTimerTicks() + SYNC_TIME_OUT;
    if( MaxBaud == MIN_BAUD )
        wait_time += SEC2TICK( 1 );
    for( ;; ) {                         /* loop until SYNC_END received or time out */
        reply = WaitByte( 1 );              /* get character */
        if( reply == SYNC_END )
            break;                          /* SYNC_END received; continue */
        if( reply == SYNC_BYTE ) {          /* SYNC_BYTE received; send ACK */
            SendByte( SDATA_ACK );
        } else if( reply == SDATA_HI ) {    /* return HI received */
            SendByte( SDATA_HI );
        } else if( GetTimerTicks() >= wait_time ) { /* 2 sec time out */
            return( false );
        }                               /* not yet time out; loop */
    }
    return( true );
}

/* This routine is one side of the set baud rate routine -- receiver
   Result:   true or false  */

static bool SetBaudReceiver( void )
{
    unsigned    wait;

    if( !ReceiverHandshake() )
        return( false );
    /* sync string should have been received; and in receive buffer now
       CheckSyncString() checks if sync string is received successfully */
    if( CheckSyncString() ) {
        SendByte( SDATA_TAK );
        wait = (MaxBaud == MIN_BAUD) ? SEC2TICK( 2 ) : MSEC2TICK( 500 );
        if( WaitByte( wait ) == SDATA_ACK ) {
            SendByte( SDATA_TAK );
            return( true );
        }
    }
    return( false );
}


/*========================================================================*/
/*========================================================================*/


/* This routine tries to establish reliable connection at a specific
   baud rate.    baud_index contains the baud rate index
   Output:   true or false  */

static bool SetBaud( int baud_index, int *sync_point_p )
{
    int reply;         /* storing data received from other machine */
    int sync_point;

    sync_point = *sync_point_p;
    *sync_point_p += MAX_BAUD_SET_TICKS + 3*SYNC_SLOP;
    if( !Baud( baud_index ) )
        return( false );        /* sets up baud rate */
    SyncPoint( sync_point );
    ClearCom();
    Wait( SYNC_SLOP );
    SendByte( SDATA_HI );
    reply = WaitByte( SYNC_SLOP * 2 );
    if( reply != SDATA_HI ) {
        return( false );
    }
    /* now go the other way */
    *sync_point_p += BaudTable[ baud_index ].full_test_ticks;
#ifdef SERVER
    if( !SetBaudReceiver() )
        return( false );
    if( !SetBaudSender() )
        return( false );
#else
    if( !SetBaudSender() )
        return( false );
    if( !SetBaudReceiver() )
        return( false );
#endif
    return( true );
}


/*========================================================================*/
/*========================================================================*/


static bool MarchToTheSameDrummer( void )
{
    int got;

#ifdef SERVER
    SendByte( SDATA_LOK );
#else
    if( (got = WaitByte( SEC2TICK( 2 ) )) != SDATA_LOK ) {
        return( false );
    }
#endif
#ifndef SERVER
    SendByte( SDATA_ACK );
#else
    if( (got = WaitByte( MSEC2TICK( 500 ) )) != SDATA_ACK ) {
        return( false );
    }
#endif
    Wait( 1 );  /* don't go till the timer hits exactly on the tick */
    ResetTimerTicks();
    return( true );
}


static bool SetSyncTime( void )
{
    if( MaxBaud != MIN_BAUD ) {
        if( !Baud( LOW_BAUD ) ) {
            return( false );
        }
    }
#ifdef SERVER
    if( WaitByte( MSEC2TICK( 100 ) ) != SDATA_HI ) {
        return( false );
    }
#else
    SendByte( SDATA_HI );
#endif
    return( MarchToTheSameDrummer() );
}

/*========================================================================*/
/*========================================================================*/


/*
 * This routine sets up the highest possible baud rate.
 * BaudCounter contains the baud rate index
 * Returns:  true or false
 */

static bool Speed( void )
{
    int  sync_point;

    if( !MarchToTheSameDrummer() )
        return( false );
    sync_point = MAX_BAUD_SET_TICKS;
    for( ;; ) {
        if( SetBaud( BaudCounter, &sync_point ) )
            break;
        ++BaudCounter;                /*  ... try next slower speed */
        if( BaudCounter >= MIN_BAUD ) {
            BaudCounter = MIN_BAUD;
            return( false );
        }
    }
    return( true );      /* link established */
}


/*========================================================================*/
/*========================================================================*/


/* This function is used by the next routine to calculate the crc for a
   block of data.
   Input:   old_crc -- crc value so far
             data   -- next byte to be incorporated
   Returns:  the new crc value after data is accounted for               */

static word NewCRC( byte data, word old_crc )
{
    word     crc_value;            /* the CRC value of the block */
    int      i;                    /* loop index */
    int      high_bit_on;          /* flag for high bit of value */
    int      data_bit;             /* appropriate bit of data */

    crc_value = old_crc;
    /* Process 0th through 7th bit of the byte */
    for( i = 8; i > 0; --i ) {
        data_bit = data & 1;
        high_bit_on = crc_value & 0x8000;
        crc_value <<= 1;
        crc_value |= data_bit;
        if( high_bit_on ) {
            crc_value ^= CRC_VAL;
        }
        data >>= 1;
    }
    return( crc_value );
}


/*========================================================================*/
/*========================================================================*/


/* This routine calculates the CRC for a block of 8-bit data (plus 3 bytes)
   Input:   extra  points to the extra 3 bytes (blk# & err) that are
                     included in calculating the CRC
              p    is a pointer to the start of the block of data
             num   is the number of data bytes
   Output:   the CRC value                                             */

static word CRC( byte *extra, unsigned len, const void *data )
{
    word     crc_value = 0;        /* the CRC value of the block */
    unsigned i;                    /* loop index */

    /* included blk# & err bytes in the front, and 2 padding characters 0xff */
    for( i = 0; i < 3; ++i, ++extra ) {
        crc_value = NewCRC( *extra, crc_value );
    }
    for( i = 0; i < len; ++i ) {
        crc_value = NewCRC( ((byte *)data)[i], crc_value );
    }
    crc_value = NewCRC( 0xff, crc_value );
    crc_value = NewCRC( 0xff, crc_value );
    return( crc_value );
}


/*========================================================================*/
/*========================================================================*/


/* This routine builds a block and sends it to the other machine.
   Block format: STX CRC(2) len(2) blk#(2) err(1) data(n) ETX
   where: STX=0x02, crc=16 bit error check value, len=length of data block,
   blk#=number of this block, 0 thru 65535, wrapping, err=no. of Errors for
   previous BlockSend, data=send characters, ETX=0x03

   Entry:    p -- pointer to block of data
             num -- number of bytes in data block
             timeout -- time to wait for proper acknowledgement
   Returns:  true or false (due to time-out)                  */

static bool BlockSend( trap_elen len, const void *data, unsigned timeout )
{
    word            crc_value;          /* crc value of block */
    unsigned        wait_time;          /* timer for testing time-out */
    trap_elen       i;                  /* loop index */
    int             reply;              /* reply message from other machine */
    unsigned char   crc_low, crc_hi;
    unsigned char   len_low, len_hi;
    byte            extra[3];           /* ..[0]=blkno_low, ..[1]=blkno_hi, ..[2]=err */
    unsigned        wait;

    ResetTimerTicks();
    extra[2] = PrevErrors = Errors;
    if( Errors > 255 ) {
        extra[2] = 255;       /* because it is a char, not an int */
    }
    Errors = 0;

    ClearCom();
    /* compose send buffer contents */
    len_low = len & 0xff;              /* low 8 bits of data block length */
    len_hi  = len >> 8;                /* high 8 bits of data block length */
    extra[0] = SendBlkNo & 0xff;      /* low 8 bits of send block no */
    extra[1] = SendBlkNo >> 8;        /* high 8 bits of send block no */
    crc_value = CRC( extra, len, data );  /* calculate crc for (blk#+err+data) */
    crc_low = crc_value & 0xff;        /* low 8 bits of crc_value */
    crc_hi  = crc_value >> 8;          /* high 8 bits of crc_value */

    wait = ( MaxBaud == MIN_BAUD ) ? SEC2TICK( 2 ) : SEC2TICK( 1 );
    for( ;; ) {                 /* send block loop */
        /* send the block */
        StartBlockTrans();
        SendByte( SDATA_STX );
        SendByte( crc_low );
        SendByte( crc_hi );
        SendByte( len_low );
        SendByte( len_hi );
        SendByte( extra[0] );      /* blkno_low */
        SendByte( extra[1] );      /* blkno_hi */
        SendByte( extra[2] );      /* err */
        for( i = 0; i < len; ++i ) {
            SendByte( ((byte *)data)[i] );
        }
        SendByte( SDATA_ETX );
        StopBlockTrans();

        wait_time = GetTimerTicks() + timeout;
        for( ;; ) {                         /* wait proper acknowledgement loop */
            reply = WaitByte( wait );       /* receive reply */
            if( reply == SDATA_NO_DATA ) {
                if( (timeout != FOREVER) && (GetTimerTicks() >= wait_time) ) {
                    return( false );        /* time-out */
                } else {
                    SendByte( SDATA_RLR );  /* request last response */
                    ++Errors;               /* increment error counter */
                }
            } else {
                if( reply == SDATA_ACK ) {
                    ++SendBlkNo;
                    return( true );         /* done, exit from BlockSend() */
                } else if( reply == SDATA_NAK ) {  /* unsuccessful, re-send block */
                    ++Errors;
                    break;     /* break out of acknowledgement loop;
                                  i.e. back to send block loop */
                } else if( reply == SDATA_RLR ) {  /* request last response */
                    SendByte( LastResponse );
                    break;     /* break out ackno loop; re-send block */
                } else {       /* things are totally messed up */
                    while( WaitByte( MSEC2TICK( 750 ) ) != SDATA_NO_DATA )
                        ;             /* discard all characters sent */
                    SendByte( SDATA_RLR );     /* request last response */
                    ++Errors;
                }
            }
        }
    }
}


/*========================================================================*/
/*========================================================================*/


/* This routine receives a block, checks CRC, and ACKs its reception.
   The data and err are placed in the appropriate places as described below.
   STX of block should already have been read and discarded.
   The block format is documented in BlockSend().

   Return:  false   --  not enough buffer space to store data block
            false   --  fails for other reason than FAIL_BUFFER
            true    --  data block is stored at *p; prev err no. at *err */

static bool BlockReceive( byte *err, trap_elen max_len, void *p )
{
    byte            buffer[8];     /* storing bytes other than actual data from blocks */
    trap_elen       i;             /* loop index */
    trap_elen       len;
    word            crc_val;
    unsigned        blkno;
    int             c;
    int             wait;

    ResetTimerTicks();
    BytesReceived = 0;
    wait = (MaxBaud == MIN_BAUD) ? MSEC2TICK( 500 ) : MSEC2TICK( 250 );
    /* Receiving bytes before actual data (up to err byte) */
    for( i = 1; i <= 7; ++i ) {
        c = WaitByte( wait );
        if( c == SDATA_NO_DATA ) {    /* time-out error */
            LastResponse = SDATA_NAK;
            SendByte( SDATA_NAK );  /* send NAK to request resending of block */
            return( false );
        }
        buffer[i] = c;
    }

    len = (buffer[MLEN+1] << 8) | buffer[MLEN];   /* number of data bytes */
    if( len > max_len ) {        /* not enough buffer space to store data */
        ClearCom();
        return( false );
    }

    /* Receiving actual data bytes */
    for( i = 0; i < len; ++i ) {
        c = WaitByte( wait );
        if( c == SDATA_NO_DATA ) {    /* time-out error */
            LastResponse = SDATA_NAK;
            SendByte( SDATA_NAK );  /* send NAK to request resending of block */
            return( false );
        }
        ((byte *)p)[i] = c;
    }

    /* Receiving the last byte: ETX */
    buffer[0] = WaitByte( wait );
    if( buffer[0] != SDATA_ETX ) {    /* time-out error */
        LastResponse = SDATA_NAK;
        SendByte( SDATA_NAK );  /* send NAK to request resending of block */
        return( false );
    }

    *err = buffer[MERR];            /* storing command byte */
    blkno = (buffer[MBLKNO+1] << 8) | buffer[MBLKNO];  /* blk# received */
    crc_val = (buffer[MCRC+1] << 8) | buffer[MCRC];  /* crc received */

    if( CRC( &buffer[MBLKNO], len, p ) != crc_val ) {   /* CRC error */
        LastResponse = SDATA_NAK;
        SendByte( SDATA_NAK );  /* send NAK to request resending of block */
        return( false );
    }

    if( ReceiveBlkNo != blkno ) {      /* incorrect block */
        ClearCom();
        LastResponse = SDATA_ACK;
        SendByte( SDATA_ACK );
        return( false );
    }

    /* Block received successfully */
    LastResponse = SDATA_ACK;
    SendByte( SDATA_ACK );
    ++ReceiveBlkNo;
    BytesReceived = len;
    return( true );
}


/*========================================================================*/
/*========================================================================*/


/* This routine attempts to receive a block within the time limit defined
      in parameter timeout.

   Return:  false   --  not enough buffer space to store block
            false   --  block is not available/received successfully
            true    --  data block is stored at *p; prev err no. at *err */

static bool WaitReceive( byte *err, trap_elen max_len, void *p, unsigned timeout )
{
    unsigned wait_time;     /* timer */
    int      data;          /* data from other machine */

    ResetTimerTicks();
    wait_time = GetTimerTicks() + timeout;
    for( ;; ) {
        data = WaitByte( 1 );
        if( data == SDATA_STX ) {           /* STX received, get block */
            if( BlockReceive( err, max_len, p ) ) {
                return( true );
            }
        } else if( data == SDATA_RLR ) {    /* RLR received */
            SendByte( SDATA_NAK );          /* tell the other end to resend block */
        } else if( (timeout != FOREVER) && (GetTimerTicks() >= wait_time) ) {
            return( false );                /* time-out */
        }
    }
}

/*========================================================================*/
/*========================================================================*/

/*
 * This routine ReSync speed.
 * Returns:  true or false
 */

static bool ReSync( void )
{
    ++BaudCounter;              /* next slower speed */
    if( BaudCounter > MIN_BAUD )
        BaudCounter = MIN_BAUD;
    while( !SetSyncTime() )
        ;
    return( Speed() );  /* sync */
}


/*========================================================================*/
/*========================================================================*/

/* Function to see if two strings (pointed to by far pointers) are
   identical.   Returns true (identical) or false (not identical) */

static bool StrEq( char *s1, char *s2 )
{
    while( (*s1 != '\0') && (*s2 != '\0') && (*s1 == *s2) ) {
        ++s1;
        ++s2;
    }
    return( *s1 == '\0' );
}


/*========================================================================*/
/*========================================================================*/


/* Routine used by RemoteLink() to set maximum baud rate */

static char *SetMaxBaud( char *str )
{
    int i;              /* loop index */

    for( i = 0; i <= MIN_BAUD; ++i ) {
        if( StrEq( str, BaudTable[i].name ) ) {  /* strings are equal */
            MaxBaud = i;
            return( NULL );
        }
    }
    return( TRP_ERR_invalid_maximum_baud_rate );
}


/*========================================================================*/
/*========================================================================*/


/* This routine is called by RemoteLink to set max baud rate */

static const char *CollectParm( const char *parm, char *arg, int *len )
{
    int  i;

    i = 0;
    while( *parm >= '0' && *parm <= '9' ) {
        if( i < 7 )
            arg[i] = *parm;
        ++parm;
        ++i;
    }
    *len = i;
    return( parm );
}


static char *SetLinkParms( const char **pparm )
{
    const char  *parm;
    char        arg1[7];
    int         arg1_len;
    char        *result;        /* result of ParsePortSpec or SetComPort */

    parm = *pparm;
    MaxBaud = 0;                /* default max baud is 115200 */

    /* strip leading white spaces */
    while( *parm == ' ' || *parm == '\t' )
        ++parm;

    result = ParsePortSpec( &parm );
    if( result != NULL )
        return( result );

    arg1_len = 0;
    if( *parm == '.' ) {
        ++parm;
        parm = CollectParm( parm, arg1, &arg1_len );
        if( arg1_len >= 7 ) {
            return( TRP_ERR_invalid_baud_rate );
        }
    }
    *pparm = parm;
    if( arg1_len == 0 )
        return( NULL );
    arg1[ arg1_len ] = '\0';
    if( StrEq( arg1, "0" ) ) {
        MaxBaud = MIN_BAUD;
        return( NULL );
    }
    if( arg1_len < 2 )
        return( TRP_ERR_ambiguous_baud_rate );
    return( SetMaxBaud( arg1 ) );
}


/*========================================================================*/
/*========================================================================*/

static void SlowSend( int ch )
{
    SendByte( ch );
    if( WaitByte( 1 ) != SDATA_NO_DATA ) {
        Wait( 1 ); /* pickup echoed character */
    }
}

static char *SetupModem( const char *parm )
{
    const char  *start;
    unsigned    wait;
    int         ch;
    int         data;

    Baud( MaxBaud );
    wait = SEC2TICK( 3 );
    while( *parm == ' ' || *parm == '\t' )
        ++parm;
    if( *parm == '\0' )
        return( NULL );
    for( ;; ) {
        if( *parm == '(' ) {
            start = ++parm;
            for( ;; ) {
                ch = *(unsigned char *)parm;
                if( ch == '\0' )
                    goto done;
                ++parm;
                if( ch == ')' )
                    break;
                if( ch == '\\' ) {
                    ch = (unsigned char)*parm++;
                    switch( ch ) {
                    case '\0':
                        return( TRP_ERR_invalid_modem_string );
                    case 'r':
                        ch = '\r';
                        break;
                    case 'n':
                        ch = '\n';
                        break;
                    }
                }
                data = WaitByte( wait );
                if( data == SDATA_NO_DATA ) {
                    if( wait != SEC2TICK( 60 ) ) {
                        wait = SEC2TICK( 60 );
                    } else {
                        return( TRP_ERR_timeout_on_modem_string );
                    }
                    --parm;
                } else {
                    wait = SEC2TICK( 3 );
                    if( data != ch ) {
                        parm = start;
                    }
                }
            }
        } else {
            Wait( MSEC2TICK( 200 ) );
            for( ;; ) {
                ch = *(unsigned char *)parm;
                if( ch == '\0' )
                    goto done;
                if( ch == '(' )
                    break;
                ++parm;
                if( ch == '\\' ) {
                    ch = (unsigned char)*parm++;
                    switch( ch ) {
                    case '\0':
                        return( TRP_ERR_invalid_modem_string );
                    case '`':
                        Wait( 1 );
                        break;
                    case '~':
                        Wait( SEC2TICK( 1 ) );
                        break;
                    case 'r':
                        SlowSend( '\r' );
                        Wait( MSEC2TICK( 500 ) );
                        break;
                    case 'n':
                        SlowSend( '\n' );
                        break;
                    default:
                        SlowSend( ch );
                        break;
                    }
                } else {
                    SlowSend( ch );
                }
            }
        }
    }
done:
#ifdef SERVER
  #define SEND_CHAR     '='
  #define EXPECT_CHAR   '-'
#else
  #define SEND_CHAR     '-'
  #define EXPECT_CHAR   '='
#endif
    wait = 1;
    for( ;; ) {
        data = WaitByte( wait );
        if( data == EXPECT_CHAR )
            break;
        if( data == SDATA_NO_DATA ) {
            if( wait != SEC2TICK( 10 ) ) {
                wait = SEC2TICK( 10 );
                SendByte( SEND_CHAR );
            } else {
                return( TRP_ERR_modem_failed_connection );
            }
        }
    }
    if( wait != SEC2TICK( 10 ) )
        SendByte( SEND_CHAR );
    return( NULL );
}

/* The format for *parm is "1.9600<modem_connect_string>" */

const char *RemoteLink( const char *parms, bool server )
{
    const char  *result;

    /* unused parameters */ (void)server;

    result = SetLinkParms( &parms );  /* set com: port & max baud rate */
    if( result != NULL ) {
        DonePort();
        return( result );
    }
    result = InitSys();
    if( result != NULL ) {
        DonePort();
        return( result );
    }
    result = SetupModem( parms );
    if( result != NULL )
        RemoteUnLink();
    return( result );
}


/*========================================================================*/
/*========================================================================*/


void RemoteUnLink( void )
{
    ResetSys();       /* reset system to initial state */
    DonePort();
}


/*========================================================================*/
/*========================================================================*/


/* Returns: true or false */

bool RemoteConnect( void )
{
    int     baud_limit;     /* maximum baud that BOTH sides can achieve */
    byte    err;            /* hold values that we don't need here */
    char    data;
    int     MaxBaud2;

    SendBlkNo = ReceiveBlkNo = 0;
    LastResponse = SDATA_NAK;
    if( !SetSyncTime() )
        return( false );
    /* establish baud limit */
#ifdef SERVER
    if( !WaitReceive( &err, 1, &data, SEC2TICK( 2 ) ) ) {
        return( false );
    }
    MaxBaud2 = (byte)data;
    data = MaxBaud;
    if( !BlockSend( 1, &data, SEC2TICK( 2 ) ) ) {
        return( false );
    }
#else
    data = MaxBaud;
    if( !BlockSend( 1, &data, SEC2TICK( 2 ) ) ) {
        return( false );
    }
    if( !WaitReceive( &err, 1, &data, SEC2TICK( 2 ) ) ) {
        return( false );
    }
    MaxBaud2 = (byte)data;
#endif
    /* MaxBaud2 now contains the other side's baud rate limit */
    if( MaxBaud > MaxBaud2 ) {
        baud_limit = MaxBaud;
    } else {
        baud_limit = MaxBaud2;
    }

    BaudCounter = baud_limit;
    if( !Speed() )
        return( false );
#ifdef SERVER
    {
        char    buff[128];

        if( BaudCounter == MIN_BAUD ) {
            strcpy( buff, "pre-set" );
        } else {
            strcpy( buff, BaudTable[BaudCounter].name );
        }
        strcat( buff, " baud" );
        ServMessage( buff );
    }
#endif
    return( true );
}


/*========================================================================*/
/*========================================================================*/


void RemoteDisco( void )
{
    ClearLastChar();
    ClearCom();
}


/*========================================================================*/
/*========================================================================*/

/* Return:  Number of bytes received                                    */

trap_retval RemoteGet( void *data, trap_elen len )
{
    unsigned        timeout;    /* time limit for getting the data */
    unsigned char   err;        /* storing the # of Errors the other side
                                    experience in sending data block */

    timeout = FOREVER;

    /* Get data block */
    if( !WaitReceive( &err, len, data, timeout ) )
        return( 0 );

    if( err > MAX_ERRORS ) {    /* too many Errors */
        ReSync();
        BytesReceived = 0;
    }
    return( BytesReceived );
}


/*========================================================================*/
/*========================================================================*/


trap_retval RemotePut( void *data, trap_elen len )
{
    unsigned timeout;       /* time limit for getting the data */

    timeout = SEC2TICK( 10 );

    /* Sending data block */
    if( !BlockSend( len, data, timeout ) )
        return( REQUEST_FAILED );

    /* ReSync if necessary */
    if( PrevErrors > MAX_ERRORS ) {
        ReSync();
    }
    return( len );
}
