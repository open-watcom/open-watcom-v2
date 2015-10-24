#include "trpcapb.h"
#include "trapglbl.h"
#include "dbgdefn.h"
#include "dbgdata.h"
#include "trpld.h"

/*
 *  Find if the current trap file supports the capabilities service
 */

static bool             Supports8ByteBreakpoints = FALSE;
static bool             SupportsExactBreakpoints = FALSE;

static trap_shandle     SuppCapabilitiesId = 0;

#define SUPP_CAPABILITIES_SERVICE( in, request )   \
        in.supp.core_req        = REQ_PERFORM_SUPPLEMENTARY_SERVICE;    \
        in.supp.id              = SuppCapabilitiesId;       \
        in.req                  = request;

static bool CapabilitiesGet8ByteBreakpointSupport( void )
{
    capabilities_get_8b_bp_req  acc;
    capabilities_get_8b_bp_ret  ret;

    if( SuppCapabilitiesId == 0 ) 
        return( FALSE );

    SUPP_CAPABILITIES_SERVICE( acc, REQ_CAPABILITIES_GET_8B_BP );

    TrapSimpAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    if( ret.err != 0 ) {
        return( FALSE );
    } else {
        Supports8ByteBreakpoints = TRUE;    /* The trap supports 8 byte breakpoints */
        return( TRUE );
    }
}

static bool CapabilitiesSet8ByteBreakpointSupport( bool status )
{
    capabilities_set_8b_bp_req  acc;
    capabilities_set_8b_bp_ret  ret;

    if( SuppCapabilitiesId == 0 ) 
        return( FALSE );

    SUPP_CAPABILITIES_SERVICE( acc, REQ_CAPABILITIES_SET_8B_BP );
    acc.status = status ? TRUE : FALSE;

    TrapSimpAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    if( ret.err != 0 ) {
        return( FALSE );
    } else {
        Supports8ByteBreakpoints = ret.status ? TRUE : FALSE;
        return( TRUE );
    }
}

static bool CapabilitiesGetExactBreakpointSupport( void )
{
    capabilities_get_8b_bp_req  acc;
    capabilities_get_8b_bp_ret  ret;


    if( SuppCapabilitiesId == 0 ) 
        return( FALSE );

    SUPP_CAPABILITIES_SERVICE( acc, REQ_CAPABILITIES_GET_EXACT_BP );

    TrapSimpAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    if( ret.err != 0 ) {
        return( FALSE );
    } else {
        /* The trap may support it, but it is not possible currently */
        SupportsExactBreakpoints = ret.status ? TRUE : FALSE;        
        return( TRUE );
    }
}

static bool CapabilitiesSetExactBreakpointSupport( bool status )
{
    capabilities_set_8b_bp_req  acc;
    capabilities_set_8b_bp_ret  ret;

    if( SuppCapabilitiesId == 0 ) 
        return( FALSE );

    SUPP_CAPABILITIES_SERVICE( acc, REQ_CAPABILITIES_SET_EXACT_BP );
    acc.status = status ? TRUE : FALSE;

    TrapSimpAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    if( ret.err != 0 ) {
        return( FALSE );
    } else {
        _SwitchSet( SW_BREAK_ON_WRITE, ret.status ? TRUE : FALSE ); 
        return( TRUE );
    }
}

bool SetCapabilitiesExactBreakpointSupport( bool status, bool set_switch )
{
    /* Don't change config if it is just the trap file that does not support the option! */
    if( SupportsExactBreakpoints ) {
        if( set_switch ) {
            _SwitchSet( SW_BREAK_ON_WRITE, status );
        }
        if( _IsOn( SW_BREAK_ON_WRITE ) ) {
            return( CapabilitiesSetExactBreakpointSupport( status ) );
        }
    }
    return( FALSE );
}

bool IsExactBreakpointsSupported( void )
{
    if( SupportsExactBreakpoints ) {
        return( _IsOn( SW_BREAK_ON_WRITE ) );
    } else {
        return( FALSE );
    }
}

bool Is8ByteBreakpointsSupported( void )
{
    return( Supports8ByteBreakpoints );
}

bool InitCapabilities( void )
{
    /* Always reset in case of trap switch */
    Supports8ByteBreakpoints = FALSE;
    SupportsExactBreakpoints = FALSE;

    SuppCapabilitiesId = GetSuppId( CAPABILITIES_SUPP_NAME );
    if( SuppCapabilitiesId == 0 ) 
        return( FALSE );

    CapabilitiesGet8ByteBreakpointSupport();
    CapabilitiesGetExactBreakpointSupport();

    if( Supports8ByteBreakpoints )
        CapabilitiesSet8ByteBreakpointSupport( TRUE );

    if( SupportsExactBreakpoints && _IsOn( SW_BREAK_ON_WRITE ) )
        CapabilitiesSetExactBreakpointSupport( TRUE );

    return( TRUE );
}
