
#include <miniport.h>
#include <ntddvdeo.h>
#include <video.h>
#include "videomp.h"

/* In this case, vid_in/out cannot be defined as macros because NT uses
 * a pretty funky interface for I/O port access.
 */

static void vid_outb( void *cx, unsigned port, unsigned val )
{
    PHW_DEV_EXT     pExt = cx;
    
    VideoPortWritePortUchar( pExt->IOAddrVGA + port, val );
}

static void vid_outw( void *cx, unsigned port, unsigned val )
{
    PHW_DEV_EXT     pExt = cx;
    
    VideoPortWritePortUshort( (PUSHORT)(pExt->IOAddrVGA + port), val );
}

static unsigned vid_inb( void *cx, unsigned port )
{
    PHW_DEV_EXT     pExt = cx;

    return( VideoPortReadPortUchar( pExt->IOAddrVGA + port ) );
}

static unsigned vid_inw( void *cx, unsigned port )
{
    PHW_DEV_EXT     pExt = cx;

    return( VideoPortReadPortUshort( (PUSHORT)(pExt->IOAddrVGA + port) ) );
}

static unsigned vid_ind( void *cx, unsigned port )
{
    PHW_DEV_EXT     pExt = cx;

    return( VideoPortReadPortUlong( (PULONG)(pExt->IOAddrVGA + port) ) );
}
