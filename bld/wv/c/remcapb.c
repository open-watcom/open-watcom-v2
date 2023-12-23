#include "trpcapb.h"
#include "trapglbl.h"
#include "dbgdefn.h"
#include "dbgdata.h"
#include "trpld.h"

/*
 *  Find if the current trap file supports the capabilities service
 */

static bool             SupportsExactBreakpoints = false;

static trap_shandle     SuppCapabilitiesId = 0;

#define SUPP_CAPABILITIES_SERVICE( in, request )   \
        in.supp.core_req        = REQ_PERFORM_SUPPLEMENTARY_SERVICE;    \
        in.supp.id              = SuppCapabilitiesId;       \
        in.req                  = request;

static bool CapabilitiesGetExactBreakpointSupport( void )
{
    capabilities_get_exact_bp_req  acc;
    capabilities_get_exact_bp_ret  ret;


    if( SuppCapabilitiesId == 0 )
        return( false );

    SUPP_CAPABILITIES_SERVICE( acc, REQ_CAPABILITIES_GET_EXACT_BP );

    TrapSimpleAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    if( ret.err != 0 ) {
        return( false );
    }
    /* The trap may support it, but it is not possible currently */
    SupportsExactBreakpoints = ret.status;
    return( true );
}

static bool CapabilitiesSetExactBreakpointSupport( bool status )
{
    capabilities_set_exact_bp_req  acc;
    capabilities_set_exact_bp_ret  ret;

    if( SuppCapabilitiesId == 0 )
        return( false );

    SUPP_CAPABILITIES_SERVICE( acc, REQ_CAPABILITIES_SET_EXACT_BP );
    acc.status = status;

    TrapSimpleAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    if( ret.err != 0 ) {
        return( false );
    }
    _SwitchSet( SW_BREAK_ON_WRITE, ret.status );
    return( true );
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
    return( false );
}

bool IsExactBreakpointsSupported( void )
{
    if( SupportsExactBreakpoints ) {
        return( _IsOn( SW_BREAK_ON_WRITE ) );
    } else {
        return( false );
    }
}

bool InitCapabilities( void )
{
    /* Always reset in case of trap switch */
    SupportsExactBreakpoints = false;

    SuppCapabilitiesId = GETSUPPID( CAPABILITIES_SUPP_NAME );
    if( SuppCapabilitiesId == 0 )
        return( false );

    CapabilitiesGetExactBreakpointSupport();

    if( SupportsExactBreakpoints && _IsOn( SW_BREAK_ON_WRITE ) )
        CapabilitiesSetExactBreakpointSupport( true );

    return( true );
}
