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
* Description:  Serial link communications core.
*
****************************************************************************/


#include "trpimp.h"
#include "trperr.h"
#include "stddef.h"
#include "string.h"
#include "serial.h"
#include "packet.h"

static int BaudCounter;           /* baud rate counter */
static int LastResponse = NAK;    /* last response holder */
static int Errors = 0;            /* Errors so far */
static int PrevErrors = 0;        /* Errors of previous BlockSend() operation */
static unsigned SendBlkNo = 0;    /* next block number to send */
static unsigned ReceiveBlkNo = 0; /* next block number to receive */
static unsigned BytesReceived;    /* # bytes from last receive */

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

extern char     *ParsePortSpec( char * * );
extern char     *InitSys( void );
extern void     ResetSys( void );
extern bool     Baud( int );
extern void     ClearCom( void );
extern int      GetByte( void );
extern void     SendByte( int );
extern void     DonePort( void );
extern bool     CheckPendingError( void );
extern void     ClearLastChar( void );
extern void     ZeroWaitCount( void );
extern unsigned WaitCount( void );
extern int      WaitByte( unsigned );
extern void     Wait( unsigned );
extern void     StartBlockTrans( void );
extern void     StopBlockTrans( void );
#ifdef SERVER
extern void     ServMessage( char * );
#endif

void SyncPoint( unsigned tick )
{
    Wait( tick - WaitCount() );
}


static int SenderHandshake( void )
{
    unsigned wait_time;     /* used to test for time-out */
    int      reply;         /* storing data received from other machine */

    wait_time = WaitCount() + SYNC_TIME_OUT;   /* limit for time out */
    if( MaxBaud == MIN_BAUD ) wait_time += SEC(1);
    SendByte( SYNC_BYTE );      /* send SYNC_BYTE */
    for( ;; ) {                 /* loop until ACK received or time out */
        reply = WaitByte( 1 );  /* get reply */
        if( reply == ACK ) break;       /* ACK received; go to next operation */
        if( reply == HI ) {             /* return HI received */
            SendByte( HI );
        } else if( WaitCount() > wait_time ) {    /* time out */
            return( FAIL );
        }                            /* not yet time out; loop */
    }
    SendByte( SYNC_END );
    return( SUCCESS );
}

/* This routine is one side of the set baud rate routine -- sender
   Result:   SUCCESS or FAIL    */


static int SetBaudSender( void )
{
    int     data;           /* storing sync string data to other machine */
    int     i;              /* loop index */
    int     wait_time;

    if( !SenderHandshake() ) return( FAIL );
    /* sync byte received ... send string */
    StartBlockTrans();
    for( i = data = 0; i < SYNC_LEN; i++, data = (data + SYNC_INC) & 0xff ) {
        SendByte( data );            /* send sync string bytes */
    }
    StopBlockTrans();
    wait_time = WaitCount() + SYNC_TIME_OUT;    /* limit for time out */
    /* If MaxBaud == MIN_BAUD, we're talking over a modem and it might
       have buffered characters that haven't been transmitted yet. */
    if( MaxBaud == MIN_BAUD ) wait_time += SEC(2);
    for( ;; ) {
        if( WaitByte( 1 ) == TAK ) {
            SendByte( ACK );
            if( WaitByte( SEC(1)/2 ) == TAK ) {
                return( SUCCESS );
            } else {
                return( FAIL );
            }
        } else if( WaitCount() >= wait_time ) {
              /* break not found; other end have not acknowledged string */
            return( FAIL );
        }
    }
}


/*========================================================================*/
/*========================================================================*/


/* This routine determines if speed sync string received ok.
   Input:    Receive buffer should have sync string
   Return:   SUCCESS or FAIL  */

static int CheckSyncString( void )
{
    int syn_c;         /* correct sync string bytes value */
    int i;             /* loop index */
    unsigned    wait;

    if( CheckPendingError() ) return( FAIL );
    wait = (MaxBaud == MIN_BAUD) ? SEC(2) : (SEC(1)/4);
    for( syn_c = i = 0; i < SYNC_LEN; ++i, syn_c = (syn_c + SYNC_INC) & 0xff ) {
        if( WaitByte( wait ) != syn_c ) {  /* error -- timeout or incorrect data */
            return( FAIL );
        }
        wait = SEC(1)/4;
    }
    return( SUCCESS );
}


/*========================================================================*/
/*========================================================================*/

static int ReceiverHandshake( void )
{
    int reply;         /* storing data received from other machine */
    int wait_time;

    wait_time = WaitCount() + SYNC_TIME_OUT;
    if( MaxBaud == MIN_BAUD ) wait_time += SEC(1);
    for( ;; ) {             /* loop until SYNC_END received or time out */
        reply = WaitByte( 1 );             /* get character */
        if( reply == SYNC_END ) break;     /* SYNC_END received; continue */
        if( reply == SYNC_BYTE ) {         /* SYNC_BYTE received; send ACK */
            SendByte( ACK );
        } else if( reply == HI ) {        /* return HI received */
            SendByte( HI );
        } else if( WaitCount() >= wait_time ) { /* 2 sec time out */
            return( FAIL );
        }                               /* not yet time out; loop */
    }
    return( SUCCESS );
}

/* This routine is one side of the set baud rate routine -- receiver
   Result:   SUCCESS or FAIL    */

static int SetBaudReceiver( void )
{
    unsigned    wait;

    if( !ReceiverHandshake() ) return( FAIL );
    /* sync string should have been received; and in receive buffer now
       CheckSyncString() checks if sync string is received successfully */
    if( CheckSyncString() ) {
        SendByte( TAK );
        wait = (MaxBaud == MIN_BAUD) ? SEC(2) : SEC(1)/2;
        if( WaitByte( wait ) == ACK ) {
            SendByte( TAK );
            return( SUCCESS );
        } else {
            return( FAIL );
        }
    } else {
        return( FAIL );
    }
}


/*========================================================================*/
/*========================================================================*/


/* This routine tries to establish reliable connection at a specific
   baud rate.    baud_index contains the baud rate index
   Output:   SUCCESS or FAIL                           */

static int SetBaud( int baud_index, int *sync_point_p )
{
    int reply;         /* storing data received from other machine */
    int sync_point;

    sync_point = *sync_point_p;
    *sync_point_p += MAX_BAUD_SET_TICKS + 3*SYNC_SLOP;
    if( !Baud( baud_index ) ) return( FAIL );       /* sets up baud rate */
    SyncPoint( sync_point );
    ClearCom();
    Wait( SYNC_SLOP );
    SendByte( HI );
    reply = WaitByte( SYNC_SLOP*2 );
    if( reply != HI ) {
        return( FAIL );
    }
    /* now go the other way */
    *sync_point_p += BaudTable[ baud_index ].full_test_ticks;
#ifdef SERVER
    if( !SetBaudReceiver() ) return( FAIL );
    if( !SetBaudSender() ) return( FAIL );
#else
    if( !SetBaudSender() ) return( FAIL );
    if( !SetBaudReceiver() ) return( FAIL );
#endif
    return( SUCCESS );
}


/*========================================================================*/
/*========================================================================*/


bool MarchToTheSameDrummer( void )
{
    int got;

#ifdef SERVER
    SendByte( LOK );
#else
    if( ( got = WaitByte( SEC( 2 ) ) ) != LOK ) {
        return( FAIL );
    }
#endif
#ifndef SERVER
    SendByte( ACK );
#else
    if( ( got = WaitByte( SEC(1)/2 ) ) != ACK ) {
        return( FAIL );
    }
#endif
    Wait( 1 );  /* don't go till the timer hits exactly on the tick */
    ZeroWaitCount();
    return( SUCCESS );
}


bool SetSyncTime( void )
{
    if( MaxBaud != MIN_BAUD ) {
        if( !Baud( LOW_BAUD ) ) return( FAIL );
    }
#ifdef SERVER
    if( WaitByte( SEC(1)/10 ) != HI ) {
        return( FAIL );
    }
#else
    SendByte( HI );
#endif
    return( MarchToTheSameDrummer() );
}

/*========================================================================*/
/*========================================================================*/


/* This routine sets up the highest possible baud rate.
   BaudCounter contains the baud rate index
   Returns:  SUCCESS or FAIL                                */

static int Speed( void )
{
    int  sync_point;

    if( !MarchToTheSameDrummer() ) return( FAIL );
    sync_point = MAX_BAUD_SET_TICKS;
    for( ;; ) {
        if( SetBaud( BaudCounter, &sync_point ) ) break;
        ++BaudCounter;                /*  ... try next slower speed */
        if( BaudCounter >= MIN_BAUD ) {
            BaudCounter = MIN_BAUD;
            return( FAIL );
        }
    }
    return( SUCCESS );      /* link established */
}


/*========================================================================*/
/*========================================================================*/


/* This function is used by the next routine to calculate the crc for a
   block of data.
   Input:   old_crc -- crc value so far
             data   -- next byte to be incorporated
   Returns:  the new crc value after data is accounted for               */

static word NewCRC( char data, word old_crc )
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

static word CRC( char *extra, int num, char *p )
{
    word     crc_value = 0;        /* the CRC value of the block */
    int      i;                    /* loop index */

    /* included blk# & err bytes in the front, and 2 padding characters 0xff */
    for( i = 0; i < 3; ++i, ++extra ) {
        crc_value = NewCRC( *extra, crc_value );
    }
    for( i = 0; i < num; ++i, ++p ) {
        crc_value = NewCRC( *p, crc_value );
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
   Returns:  SUCCESS or FAIL (due to time-out)                  */

static int BlockSend( unsigned num, char *p, unsigned timeout )
{
    word        crc_value;           /* crc value of block */
    unsigned wait_time;              /* timer for testing time-out */
    int      i;                      /* loop index */
    int      reply;                  /* reply message from other machine */
    char     crc_low, crc_hi, len_low, len_hi;
    char     extra[3];     /* ..[0]=blkno_low, ..[1]=blkno_hi, ..[2]=err */
    unsigned    wait;

    ZeroWaitCount();
    extra[2] = PrevErrors = Errors;
    if( Errors > 255 ) {
        extra[2] = 255;       /* because it is a char, not an int */
    }
    Errors = 0;

    ClearCom();
    /* compose send buffer contents */
    len_low = num & 0xff;              /* low 8 bits of data block length */
    len_hi  = num >> 8;                /* high 8 bits of data block length */
    extra[0] = SendBlkNo & 0xff;      /* low 8 bits of send block no */
    extra[1] = SendBlkNo >> 8;        /* high 8 bits of send block no */
    crc_value = CRC( extra, num, p );  /* calculate crc for (blk#+err+data) */
    crc_low = crc_value & 0xff;        /* low 8 bits of crc_value */
    crc_hi  = crc_value >> 8;          /* high 8 bits of crc_value */

    wait = (MaxBaud == MIN_BAUD) ? SEC(2) : SEC(1);
    for( ;; ) {                 /* send block loop */
        /* send the block */
        StartBlockTrans();
        SendByte( STX );
        SendByte( crc_low );
        SendByte( crc_hi );
        SendByte( len_low );
        SendByte( len_hi );
        SendByte( extra[0] );      /* blkno_low */
        SendByte( extra[1] );      /* blkno_hi */
        SendByte( extra[2] );      /* err */
        for( i = 0; i < num; ++i ) {
            SendByte( p[i] );
        }
        SendByte( ETX );
        StopBlockTrans();

        wait_time = WaitCount() + timeout;
        for( ;; ) {                 /* wait proper acknowledgement loop */
            reply = WaitByte( wait );          /* receive reply */
            if( reply == NO_DATA ) {
                if( (timeout != FOREVER) && (WaitCount() >= wait_time) ) {
                    return( FAIL );    /* time-out */
                } else {
                    SendByte( RLR );     /* request last response */
                    ++Errors;            /* increment error counter */
                }
            } else {
                if( reply == ACK ) {
                    ++SendBlkNo;
                    return( SUCCESS );    /* done, exit from BlockSend() */
                } else if( reply == NAK ) {  /* unsuccessful, re-send block */
                    ++Errors;
                    break;     /* break out of acknowledgement loop;
                                  i.e. back to send block loop */
                } else if( reply == RLR ) {  /* request last response */
                    SendByte( LastResponse );
                    break;     /* break out ackno loop; re-send block */
                } else {       /* things are totally messed up */
                    while( WaitByte( SEC(3)/4 ) != NO_DATA )
                        ;             /* discard all characters sent */
                    SendByte( RLR );     /* request last response */
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

   Return:  FAIL         --  not enough buffer space to store data block
            FAIL         --  fails for other reason than FAIL_BUFFER
            SUCCESS  --  data block is stored at *p; prev err no. at *err */

static int BlockReceive( char * err, unsigned max_len,
                          char * p )
{
    char buffer[8];     /* storing bytes other than actual data from blocks */
    int  i;             /* loop index */
    unsigned    len;
    word        crc_val;
    unsigned    blkno;
    int         c;
    int         wait;

    ZeroWaitCount();
    BytesReceived = 0;
    wait = (MaxBaud == MIN_BAUD) ? SEC(1)/2 : SEC(1)/4;
    /* Receiving bytes before actual data (up to err byte) */
    for( i = 1; i <= 7; ++i ) {
        c = WaitByte( wait );
        if( c == NO_DATA ) {    /* time-out error */
            LastResponse = NAK;
            SendByte( NAK );  /* send NAK to request resending of block */
            return( FAIL );
        }
        buffer[i] = c;
    }

    len = (buffer[MLEN+1] << 8) | buffer[MLEN];   /* number of data bytes */
    if( len > max_len ) {        /* not enough buffer space to store data */
        ClearCom();
        return( FAIL );
    }

    /* Receiving actual data bytes */
    for( i = 0; i < len; ++i ) {
        c = WaitByte( wait );
        if( c == NO_DATA ) {    /* time-out error */
            LastResponse = NAK;
            SendByte( NAK );  /* send NAK to request resending of block */
            return( FAIL );
        }
        p[i] = c;
    }

    /* Receiving the last byte: ETX */
    buffer[0] = WaitByte( wait );
    if( buffer[0] != ETX ) {    /* time-out error */
        LastResponse = NAK;
        SendByte( NAK );  /* send NAK to request resending of block */
        return( FAIL );
    }

    *err = buffer[MERR];            /* storing command byte */
    blkno = (buffer[MBLKNO+1] << 8) | buffer[MBLKNO];  /* blk# received */
    crc_val = (buffer[MCRC+1] << 8) | buffer[MCRC];  /* crc received */

    if( CRC( &buffer[MBLKNO], len, p ) != crc_val ) {   /* CRC error */
        LastResponse = NAK;
        SendByte( NAK );  /* send NAK to request resending of block */
        return( FAIL );
    }

    if( ReceiveBlkNo != blkno ) {      /* incorrect block */
        ClearCom();
        LastResponse = ACK;
        SendByte( ACK );
        return( FAIL );
    }

    /* Block received successfully */
    LastResponse = ACK;
    SendByte( ACK );
    ++ReceiveBlkNo;
    BytesReceived = len;
    return( SUCCESS );
}


/*========================================================================*/
/*========================================================================*/


/* This routine attempts to receive a block within the time limit defined
      in parameter timeout.

   Return:  FAIL         --  not enough buffer space to store block
            FAIL         --  block is not available/received successfully
            SUCCESS      --  data block is stored at *p; prev err no. at *err */

static int WaitReceive( char * err, unsigned max_len,
                         char * p, unsigned timeout )
{
    unsigned wait_time;               /* timer */
    int      data;                    /* data from other machine */
    int      result;                  /* result of BlockReceive() */

    ZeroWaitCount();
    wait_time = WaitCount() + timeout;
    for( ;; ) {
        data = WaitByte( 1 );
        if( data == STX ) {           /* STX received, get block */
            result = BlockReceive( err, max_len, p );
            if( result ) return( result );
        } else if( data == RLR ) {    /* RLR received */
            SendByte( NAK );          /* tell the other end to resend block */
        } else if( (timeout != FOREVER) && (WaitCount() >= wait_time) ) {
            return( FAIL );           /* time-out */
        }
    }
}

/*========================================================================*/
/*========================================================================*/


/* This routine ReSync speed.  Returns:  SUCCESS or FAIL */

static int ReSync( void )
{
    int result;            /* result of ReSyncing */

    ++BaudCounter;              /* next slower speed */
    if( BaudCounter > MIN_BAUD ) BaudCounter = MIN_BAUD;
    while( SetSyncTime() == FAIL ) ;
    result = Speed();           /* sync */
    return( result );
}


/*========================================================================*/
/*========================================================================*/


/* Function to see if two strings (pointed to by far pointers) are
   identical.   Returns SUCCESS (identical) or FAIL (not identical) */

static int StrEq( char *s1, char *s2 )
{
    while( (*s1 != '\0') && (*s2 != '\0') && (*s1 == *s2) ) {
        ++s1;
        ++s2;
    }
    if( *s1 == '\0' ) {
        return( SUCCESS );
    } else {
        return( FAIL );
    }
}


/*========================================================================*/
/*========================================================================*/


/* Routine used by RemoteLink() to set maximum baud rate */

char *SetMaxBaud( char *str )
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

static char *CollectParm( char *parm, char *arg, int *len )
{
    int  i;

    i = 0;
    while( *parm >= '0' && *parm <= '9' ) {
        if( i < 7 ) arg[i] = *parm;
        ++parm;
        ++i;
    }
    *len = i;
    return( parm );
}


static char *SetLinkParms( char **pparm )
{
    char        *parm;
    char        arg1[7];
    int         arg1_len;
    char        *result;        /* result of ParsePortSpec or SetComPort */

    parm = *pparm;
    MaxBaud = 0;                /* default max baud is 115200 */

    /* strip leading white spaces */
    while( *parm == ' ' || *parm == '\t' ) ++parm;

    result = ParsePortSpec( &parm );
    if( result != NULL ) return( result );

    arg1_len = 0;
    if( *parm == '.' ) {
        ++parm;
        parm = CollectParm( parm, arg1, &arg1_len );
        if( arg1_len >= 7 ) return( TRP_ERR_invalid_baud_rate );
    }
    *pparm = parm;
    if( arg1_len == 0 ) return( NULL );
    arg1[ arg1_len ] = '\0';
    if( StrEq( arg1, "0" ) ) {
        MaxBaud = MIN_BAUD;
        return( NULL );
    }
    if( arg1_len < 2 ) return( TRP_ERR_ambiguous_baud_rate );
    return( SetMaxBaud( arg1 ) );
}


/*========================================================================*/
/*========================================================================*/

static void SlowSend( int ch )
{
    SendByte( ch );
    if( WaitByte( 1 ) != NO_DATA ) Wait( 1 ); /* pickup echoed character */
}

static char *SetupModem( char *parm )
{
    char        *start;
    unsigned    wait;
    unsigned    ch;
    int         data;

    Baud( MaxBaud );
    wait = SEC(3);
    while( *parm == ' ' && *parm == '\t' ) ++parm;
    if( *parm == '\0' ) return( NULL );
    for( ;; ) {
        if( *parm == '(' ) {
            start = ++parm;
            for( ;; ) {
                ch = *parm;
                if( ch == '\0' ) goto done;
                ++parm;
                if( ch == ')' ) break;
                if( ch == '\\' ) {
                    ch = *parm++;
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
                if( data == NO_DATA ) {
                    if( wait != SEC(60) ) {
                        wait = SEC(60);
                    } else {
                        return( TRP_ERR_timeout_on_modem_string );
                    }
                    --parm;
                } else {
                    wait = SEC(3);
                    if( data != ch ) parm = start;
                }
            }
        } else {
            Wait( SEC(1)/5 );
            for( ;; ) {
                ch = *parm;
                if( ch == '\0' ) goto done;
                if( ch == '(' ) break;
                ++parm;
                if( ch == '\\' ) {
                    ch = *parm++;
                    switch( ch ) {
                    case '\0':
                        return( TRP_ERR_invalid_modem_string );
                    case '`':
                        Wait( 1 );
                        break;
                    case '~':
                        Wait( SEC(1) );
                        break;
                    case 'r':
                        SlowSend( '\r' );
                        Wait( SEC(1)/2 );
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
        if( data == EXPECT_CHAR ) break;
        if( data == NO_DATA ) {
            if( wait != SEC(10) ) {
                wait = SEC(10);
                SendByte( SEND_CHAR );
            } else {
                return( TRP_ERR_modem_failed_connection );
            }
        }
    }
    if( wait != SEC(10) ) SendByte( SEND_CHAR );
    return( NULL );
}

/* The format for *parm is "1.9600<modem_connect_string>" */

char *RemoteLink( char *parm, char server )
{
    char *result;

    server = server;
    if( parm == NULL ) parm = "";
    result = SetLinkParms( &parm );  /* set com: port & max baud rate */
    if( result != NULL ) {
        DonePort();
        return( result );
    }
    result = InitSys();
    if( result != NULL ) {
        DonePort();
        return( result );
    }
    result = SetupModem( parm );
    if( result != NULL ) RemoteUnLink();
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


/* Returns: SUCCESS or FAIL */

char RemoteConnect( void )
{
    int     baud_limit;     /* maximum baud that BOTH sides can achieve */
    char    dummy;          /* hold values that we don't need here */
    char    MaxBaud2;       /* MaxBaud at the other machine */

    SendBlkNo = ReceiveBlkNo = 0;
    LastResponse = NAK;
    if( !SetSyncTime() ) return( FALSE );
    /* establish baud limit */
#ifdef SERVER
    if( !WaitReceive( &dummy, 1, &MaxBaud2, SEC( 2 ) ) ) {
        return( FALSE );
    }
    if( !BlockSend( 1, (void *)&MaxBaud, SEC( 2 ) ) ) {
        return( FALSE );
    }
#else
    if( !BlockSend( 1, (void *)&MaxBaud, SEC( 2 ) ) ) {
        return( FALSE );
    }
    if( !WaitReceive( &dummy, 1, &MaxBaud2, SEC( 2 ) ) ) {
        return( FALSE );
    }
#endif
    /* MaxBaud2 now contains the other side's baud rate limit */
    if( MaxBaud > MaxBaud2 ) {
        baud_limit = MaxBaud;
    } else {
        baud_limit = MaxBaud2;
    }

    BaudCounter = baud_limit;
    if( !Speed() ) return( FALSE );
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
    return( TRUE );
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

unsigned RemoteGet( char *rec, unsigned max_len )
{
    unsigned timeout;             /* time limit for getting the data */
    char     err;                 /* storing the # of Errors the other side
                                        experience in sending data block */
    int      result;              /* result of WaitReceive() operation */


    timeout = FOREVER;

    /* Get data block */
    result = WaitReceive( &err, max_len, rec, timeout );
    if( !result ) return( 0 );

   if( err > MAX_ERRORS ) {    /* too many Errors */
        ReSync();
        BytesReceived = 0;
    }
    return( BytesReceived );
}


/*========================================================================*/
/*========================================================================*/


unsigned RemotePut( char *send, unsigned len )
{
    unsigned timeout;             /* time limit for getting the data */
    int      result;              /* result of BlockSend() operation */

    timeout = SEC( 10 );


    /* Sending data block */
    result = BlockSend( len, send, timeout );
    if( !result ) return( REQUEST_FAILED );

    /* ReSync if necessary */
    if( PrevErrors > MAX_ERRORS ) {
        ReSync();
    }
    return( len );
}
