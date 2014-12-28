#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <winbase.h>    /* For GetSystemTime */
#include "trpcore.h"
#include "madregs.h"
#include "trpfile.h"


mad_handle  read_mad_handle = MAD_NIL;

void    DumpPacket( unsigned char * pkt, unsigned short len, int tabit = 0 );

int handle_REQ_CONNECT( unsigned char * pkt, unsigned short len );
int handle_REQ_DISCONNECT( unsigned char * pkt, unsigned short len );
int handle_REQ_SUSPEND( unsigned char * pkt, unsigned short len );
int handle_REQ_RESUME( unsigned char * pkt, unsigned short len );
int handle_REQ_GET_SUPPLEMENTARY_SERVICE( unsigned char * pkt, unsigned short len );
int handle_REQ_PERFORM_SUPPLEMENTARY_SERVICE( unsigned char * pkt, unsigned short len );
int handle_REQ_GET_SYS_CONFIG( unsigned char * pkt, unsigned short len );
int handle_REQ_MAP_ADDR( unsigned char * pkt, unsigned short len );
int handle_REQ_CHECKSUM_MEM( unsigned char * pkt, unsigned short len );
int handle_REQ_READ_MEM( unsigned char * pkt, unsigned short len );
int handle_REQ_WRITE_MEM( unsigned char * pkt, unsigned short len );
int handle_REQ_READ_IO( unsigned char * pkt, unsigned short len );
int handle_REQ_WRITE_IO( unsigned char * pkt, unsigned short len );
int handle_REQ_PROG_GO( unsigned char * pkt, unsigned short len );
int handle_REQ_PROG_STEP( unsigned char * pkt, unsigned short len );
int handle_REQ_PROG_LOAD( unsigned char * pkt, unsigned short len );
int handle_REQ_PROG_KILL( unsigned char * pkt, unsigned short len );
int handle_REQ_SET_WATCH( unsigned char * pkt, unsigned short len );
int handle_REQ_CLEAR_WATCH( unsigned char * pkt, unsigned short len );
int handle_REQ_SET_BREAK( unsigned char * pkt, unsigned short len );
int handle_REQ_CLEAR_BREAK( unsigned char * pkt, unsigned short len );
int handle_REQ_GET_NEXT_ALIAS( unsigned char * pkt, unsigned short len );
int handle_REQ_SET_USER_SCREEN( unsigned char * pkt, unsigned short len );
int handle_REQ_SET_DEBUG_SCREEN( unsigned char * pkt, unsigned short len );
int handle_REQ_READ_USER_KEYBOARD( unsigned char * pkt, unsigned short len );
int handle_REQ_GET_LIB_NAME( unsigned char * pkt, unsigned short len );
int handle_REQ_GET_ERR_TEXT( unsigned char * pkt, unsigned short len );
int handle_REQ_GET_MESSAGE_TEXT( unsigned char * pkt, unsigned short len );
int handle_REQ_REDIRECT_STDIN( unsigned char * pkt, unsigned short len );
int handle_REQ_REDIRECT_STDOUT( unsigned char * pkt, unsigned short len );
int handle_REQ_SPLIT_CMD( unsigned char * pkt, unsigned short len );
int handle_REQ_READ_REGS( unsigned char * pkt, unsigned short len );
int handle_REQ_WRITE_REGS( unsigned char * pkt, unsigned short len );
int handle_REQ_MACHINE_DATA( unsigned char * pkt, unsigned short len );

int handle_REQ_CONNECT_REPLY( unsigned char * pkt, unsigned short len );
int handle_REQ_DISCONNECT_REPLY( unsigned char * pkt, unsigned short len );
int handle_REQ_SUSPEND_REPLY( unsigned char * pkt, unsigned short len );
int handle_REQ_RESUME_REPLY( unsigned char * pkt, unsigned short len );
int handle_REQ_GET_SUPPLEMENTARY_SERVICE_REPLY( unsigned char * pkt, unsigned short len );
int handle_REQ_PERFORM_SUPPLEMENTARY_SERVICE_REPLY( unsigned char * pkt, unsigned short len );
int handle_REQ_GET_SYS_CONFIG_REPLY( unsigned char * pkt, unsigned short len );
int handle_REQ_MAP_ADDR_REPLY( unsigned char * pkt, unsigned short len );
int handle_REQ_CHECKSUM_MEM_REPLY( unsigned char * pkt, unsigned short len );
int handle_REQ_READ_MEM_REPLY( unsigned char * pkt, unsigned short len );
int handle_REQ_WRITE_MEM_REPLY( unsigned char * pkt, unsigned short len );
int handle_REQ_READ_IO_REPLY( unsigned char * pkt, unsigned short len );
int handle_REQ_WRITE_IO_REPLY( unsigned char * pkt, unsigned short len );
int handle_REQ_PROG_GO_REPLY( unsigned char * pkt, unsigned short len );
int handle_REQ_PROG_STEP_REPLY( unsigned char * pkt, unsigned short len );
int handle_REQ_PROG_LOAD_REPLY( unsigned char * pkt, unsigned short len );
int handle_REQ_PROG_KILL_REPLY( unsigned char * pkt, unsigned short len );
int handle_REQ_SET_WATCH_REPLY( unsigned char * pkt, unsigned short len );
int handle_REQ_CLEAR_WATCH_REPLY( unsigned char * pkt, unsigned short len );
int handle_REQ_SET_BREAK_REPLY( unsigned char * pkt, unsigned short len );
int handle_REQ_CLEAR_BREAK_REPLY( unsigned char * pkt, unsigned short len );
int handle_REQ_GET_NEXT_ALIAS_REPLY( unsigned char * pkt, unsigned short len );
int handle_REQ_SET_USER_SCREEN_REPLY( unsigned char * pkt, unsigned short len );
int handle_REQ_SET_DEBUG_SCREEN_REPLY( unsigned char * pkt, unsigned short len );
int handle_REQ_READ_USER_KEYBOARD_REPLY( unsigned char * pkt, unsigned short len );
int handle_REQ_GET_LIB_NAME_REPLY( unsigned char * pkt, unsigned short len );
int handle_REQ_GET_ERR_TEXT_REPLY( unsigned char * pkt, unsigned short len );
int handle_REQ_GET_MESSAGE_TEXT_REPLY( unsigned char * pkt, unsigned short len );
int handle_REQ_REDIRECT_STDIN_REPLY( unsigned char * pkt, unsigned short len );
int handle_REQ_REDIRECT_STDOUT_REPLY( unsigned char * pkt, unsigned short len );
int handle_REQ_SPLIT_CMD_REPLY( unsigned char * pkt, unsigned short len );
int handle_REQ_READ_REGS_REPLY( unsigned char * pkt, unsigned short len );
int handle_REQ_WRITE_REGS_REPLY( unsigned char * pkt, unsigned short len );
int handle_REQ_MACHINE_DATA_REPLY( unsigned char * pkt, unsigned short len );

typedef int ( *RQ_HANDLER )( unsigned char * pkt, unsigned short len );
  
RQ_HANDLER MyHandlers[] = {
    handle_REQ_CONNECT,
    handle_REQ_DISCONNECT,
    handle_REQ_SUSPEND,
    handle_REQ_RESUME,
    handle_REQ_GET_SUPPLEMENTARY_SERVICE,
    handle_REQ_PERFORM_SUPPLEMENTARY_SERVICE,
    handle_REQ_GET_SYS_CONFIG,
    handle_REQ_MAP_ADDR,
    handle_REQ_CHECKSUM_MEM,
    handle_REQ_READ_MEM,
    handle_REQ_WRITE_MEM,
    handle_REQ_READ_IO,
    handle_REQ_WRITE_IO,
    handle_REQ_PROG_GO,
    handle_REQ_PROG_STEP,
    handle_REQ_PROG_LOAD,
    handle_REQ_PROG_KILL,
    handle_REQ_SET_WATCH,
    handle_REQ_CLEAR_WATCH,
    handle_REQ_SET_BREAK,
    handle_REQ_CLEAR_BREAK,
    handle_REQ_GET_NEXT_ALIAS,
    handle_REQ_SET_USER_SCREEN,
    handle_REQ_SET_DEBUG_SCREEN,
    handle_REQ_READ_USER_KEYBOARD,
    handle_REQ_GET_LIB_NAME,
    handle_REQ_GET_ERR_TEXT,
    handle_REQ_GET_MESSAGE_TEXT,
    handle_REQ_REDIRECT_STDIN,
    handle_REQ_REDIRECT_STDOUT,
    handle_REQ_SPLIT_CMD,
    handle_REQ_READ_REGS,
    handle_REQ_WRITE_REGS,
    handle_REQ_MACHINE_DATA,
    NULL
};

RQ_HANDLER MyReplyHandlers[] = {
    handle_REQ_CONNECT_REPLY,
    handle_REQ_DISCONNECT_REPLY,
    handle_REQ_SUSPEND_REPLY,
    handle_REQ_RESUME_REPLY,
    handle_REQ_GET_SUPPLEMENTARY_SERVICE_REPLY,
    handle_REQ_PERFORM_SUPPLEMENTARY_SERVICE_REPLY,
    handle_REQ_GET_SYS_CONFIG_REPLY,
    handle_REQ_MAP_ADDR_REPLY,
    handle_REQ_CHECKSUM_MEM_REPLY,
    handle_REQ_READ_MEM_REPLY,
    handle_REQ_WRITE_MEM_REPLY,
    handle_REQ_READ_IO_REPLY,
    handle_REQ_WRITE_IO_REPLY,
    handle_REQ_PROG_GO_REPLY,
    handle_REQ_PROG_STEP_REPLY,
    handle_REQ_PROG_LOAD_REPLY,
    handle_REQ_PROG_KILL_REPLY,
    handle_REQ_SET_WATCH_REPLY,
    handle_REQ_CLEAR_WATCH_REPLY,
    handle_REQ_SET_BREAK_REPLY,
    handle_REQ_CLEAR_BREAK_REPLY,
    handle_REQ_GET_NEXT_ALIAS_REPLY,
    handle_REQ_SET_USER_SCREEN_REPLY,
    handle_REQ_SET_DEBUG_SCREEN_REPLY,
    handle_REQ_READ_USER_KEYBOARD_REPLY,
    handle_REQ_GET_LIB_NAME_REPLY,
    handle_REQ_GET_ERR_TEXT_REPLY,
    handle_REQ_GET_MESSAGE_TEXT_REPLY,
    handle_REQ_REDIRECT_STDIN_REPLY,
    handle_REQ_REDIRECT_STDOUT_REPLY,
    handle_REQ_SPLIT_CMD_REPLY,
    handle_REQ_READ_REGS_REPLY,
    handle_REQ_WRITE_REGS_REPLY,
    handle_REQ_MACHINE_DATA_REPLY,
    NULL
};

typedef int ( SVC_DECODE )( int request, unsigned char * pkt, unsigned short len );

typedef unsigned service_func(void);

typedef struct tagServiceNameAndId{
    struct tagServiceNameAndId      *next;
    char                            *service_name;
    trap_shandle                    service_id;
    SVC_DECODE                      *service_decode;
}ServiceNameAndId;

char    last_service_name[1024] = { 0 };

ServiceNameAndId *  services = NULL;
ServiceNameAndId *  last_service = NULL;

extern int SSD_Files( int, unsigned char *, unsigned short );
extern int SSD_FileInfo( int, unsigned char * , unsigned short );
extern int SSD_Environment( int, unsigned char * , unsigned short );
extern int SSD_Threads( int , unsigned char * , unsigned short );
extern int SSD_Overlays( int, unsigned char * , unsigned short );
extern int SSD_Capabilities( int, unsigned char * , unsigned short );

static char *mad_desc[] = {
    "Unknown",
#define pick_mad(enum,file,desc) desc,
#include "madarch.h"
#undef pick_mad
};

static char *mad_os_desc[] = {
#define pick_mad(enum,desc) desc,
#include "mados.h"
#undef pick_mad
};

SVC_DECODE * get_supp_service_decoder( const char * service_name )
{
    if( 0 == stricmp( service_name, "Files" ) )
        return &SSD_Files;
    else if( 0 == stricmp( service_name, "FileInfo" ) )
        return &SSD_FileInfo;
    else if( 0 == stricmp( service_name, "Environment" ) )
        return &SSD_Environment;
    else if( 0 == stricmp( service_name, "Threads" ) )
        return &SSD_Threads;
    else if( 0 == stricmp( service_name, "Overlays" ) )
        return &SSD_Overlays;
    else if( 0 == stricmp( service_name, "Capabilities" ) )
        return &SSD_Capabilities;
    
    return NULL;   
}

int main( int argc, char ** argv )
{
    if( argc < 2 )
        return -1;    
    
    FILE * fil = fopen( argv[1], "rb" );
    if( !fil ) {
        fprintf( stderr, "Failed to open file\n" );
        return -1;
    }
    
    unsigned        offset = 0;
    unsigned short  type = 0;
    unsigned short  len = 0;
    unsigned char   buffer[2048];
    unsigned char   last_id = 255;
    SYSTEMTIME      st = {0};
    int             time_valid = 0;
    
    for( ;; ) {
    
        if( sizeof( type ) != fread( &type, 1, sizeof( type ), fil ) ) {
            if( !feof( fil ) )
                fprintf( stderr, "Failed to read type ( uint16 ) at offset 0x%.08x\n", offset );
            break;
        }
        offset += sizeof( type );
        
        if( type > 3 ) {
            time_valid = 1;
            if( sizeof( WORD ) != fread( &st.wHour, 1, sizeof(WORD), fil ) ) {
                fprintf( stderr, "Failed to read len ( uint16 ) at offset 0x%.08x\n", offset );
                break;
            }
            offset += sizeof( WORD );
            if( sizeof( WORD ) != fread( &st.wMinute, 1, sizeof(WORD), fil ) ) {
                fprintf( stderr, "Failed to read len ( uint16 ) at offset 0x%.08x\n", offset );
                break;
            }
            offset += sizeof( WORD );
            if( sizeof( WORD ) != fread( &st.wSecond, 1, sizeof(WORD), fil ) ) {
                fprintf( stderr, "Failed to read len ( uint16 ) at offset 0x%.08x\n", offset );
                break;
            }
            offset += sizeof( WORD );
            if( sizeof( WORD ) != fread( &st.wMilliseconds, 1, sizeof(WORD), fil ) ) {
                fprintf( stderr, "Failed to read len ( uint16 ) at offset 0x%.08x\n", offset );
                break;
            }
            offset += sizeof( WORD );
        }
        
        if( type == 4 )
            type = 1;
        if( type == 5 )
            type = 3;
        
        if( sizeof( type ) != fread( &len, 1, sizeof( len ), fil ) ) {
            fprintf( stderr, "Failed to read len ( uint16 ) at offset 0x%.08x\n", offset );
            break;
        }
        offset += sizeof( len );

        if( feof( fil ) )
            break;
            
        if( len > sizeof( buffer ) ) {
            fprintf( stderr, "Too many bytes to read offset 0x%.08x\n", offset );
            break;
        }
        if( len != fread( buffer, 1, len, fil ) ) {
            fprintf( stderr, "Failed to read byte ( uint16 ) at offset 0x%.08x\n", offset );
            break;
        }
     
        if( time_valid )
            printf( "%.02u:%.02u:%.02u.%3.3u ", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds );
        int rc = 0;         
        
        if( ( type == 1 ) || ( type == 2 ) ) {
            unsigned char rq_id = *buffer;
//            unsigned char no_response = rq_id & 0x80;
            rq_id &= ~0x80;
            
            if( rq_id < REQ__LAST )
                rc = ( MyHandlers[rq_id] )( buffer, len );
            last_id = rq_id;
            
        } else if( type == 3 ){
            if( last_id < REQ__LAST )
                rc = ( MyReplyHandlers[last_id] )( buffer, len );
        }

        if( rc == 0 ) {
            
           
            if( type == 1 )
                printf( "Debugger request(1). Length %u\n", len );
            else if( type == 2 )
                printf( "Debugger request(2). Length %u\n", len );
            else if( type == 3 )
                printf( "Trap response(3). Length %u\n", len );
            else{
                printf( "Unknown type (%u). Length %u\n", type, len );
                exit( -1 );
            }
            printf( "\n" );    
            DumpPacket( buffer, len );
        }
                
        printf( "\n===============================================================================\n\n" );
        
        offset += len;
        
    }
    
    fclose( fil );
    
    return 0;
}

unsigned char to_char( unsigned char x )
{
    if( ( x < 32 ) || ( x > 126 ) )
        return '.';
    return x;    
}

void DumpPacket( unsigned char * pkt, unsigned short len, int tabit )
{
    
    char    output_buffer_1[80] = {0};
    char    output_buffer_2[80] = {0};
    unsigned short  copy_len = len;
    unsigned offset = 0;
    unsigned char * ptr = pkt;

    if( tabit ) printf( "    " );
    printf( "      00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f ; 0123456789abcdef\n" );
    if( tabit ) printf( "    " );
    printf( "      -----------------------------------------------   ----------------\n" );

    while( copy_len >= 16 ) {
        if( tabit ) printf( "    " );
        printf( "%.04x: %.02x %.02x %.02x %.02x %.02x %.02x %.02x %.02x %.02x %.02x %.02x %.02x %.02x %.02x %.02x %.02x ; %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
            offset, 
            ptr[0], ptr[1], ptr[2], ptr[3], ptr[4], ptr[5], ptr[6], ptr[7], 
            ptr[8], ptr[9], ptr[10], ptr[11], ptr[12], ptr[13], ptr[14], ptr[15], 
            to_char( ptr[0] ), to_char( ptr[1] ), to_char( ptr[2] ), to_char( ptr[3] ), 
            to_char( ptr[4] ), to_char( ptr[5] ), to_char( ptr[6] ), to_char( ptr[7] ), 
            to_char( ptr[8] ), to_char( ptr[9] ), to_char( ptr[10] ), to_char( ptr[11] ), 
            to_char( ptr[12] ), to_char( ptr[13] ), to_char( ptr[14] ), to_char( ptr[15] )
        ); 
        copy_len -= 16;
        offset += 16;
        ptr += 16;
    }
    if( copy_len ) {
        sprintf(output_buffer_1, "%.04x: ", offset );
        while( copy_len ){
            char local[4];
            
            sprintf( local, "%.02x ", *ptr );
            strcat( output_buffer_1, local );
            sprintf( local, "%c", to_char( *ptr ) );
            strcat( output_buffer_2, local );
            
            copy_len--;
            ptr++;
        }    
        while( strlen( output_buffer_1 ) < 54 )
            strcat( output_buffer_1, " " );
        strcat( output_buffer_1, "; " );
        strcat( output_buffer_1, output_buffer_2 );
        if( tabit ) printf( "    " );
        printf( "%s\n", output_buffer_1 );
    }
}

int handle_REQ_CONNECT( unsigned char * pkt, unsigned short len )
{
    if( len != sizeof( connect_req ) )
        return 0;
    connect_req * prq = ( connect_req * ) pkt;
    
    printf( "Debugger request: REQ_CONNECT\n" );
    printf( "    Major :     %u\n", prq->ver.major );
    printf( "    Minor :     %u\n", prq->ver.minor );
    printf( "    Remote :    %u\n", prq->ver.remote );
    
    return 1;
}

int handle_REQ_CONNECT_REPLY( unsigned char * pkt, unsigned short len )
{
    connect_ret * pr = ( connect_ret * ) pkt;
    char * err = ( char * ) &pr[1];
    unsigned short errlen = ( len - ( unsigned short ) sizeof( connect_ret ) );

    printf( "Trap Reply: REQ_CONNECT\n" );
    printf( "    Maximum message size:   %u\n", pr->max_msg_size );
    if( errlen && *err ) {
        printf( "    Error string:           %s\n", err );
    }
    
    return 1;
}

int handle_REQ_DISCONNECT( unsigned char * , unsigned short )
{
    printf( "Debugger request: REQ_DISCONNECT\n" );
    return 1;
}

int handle_REQ_DISCONNECT_REPLY( unsigned char *, unsigned short )
{
    return 0;
}

int handle_REQ_SUSPEND( unsigned char * , unsigned short )
{
    printf( "Debugger request: REQ_SUSPEND\n" );
    return 1;
}

int handle_REQ_SUSPEND_REPLY( unsigned char * , unsigned short )
{
    return 0;
}

int handle_REQ_RESUME( unsigned char * , unsigned short )
{
    printf( "Debugger request: REQ_RESUME\n" );
    return 1;
}

int handle_REQ_RESUME_REPLY( unsigned char * , unsigned short )
{
    return 0;
}

int handle_REQ_GET_SUPPLEMENTARY_SERVICE( unsigned char * pkt, unsigned short )
{
    access_req *    req = ( access_req * ) pkt;
    req = &req[1];
    char * service_name = ( char * ) req;

    printf( "Debugger request: REQ_GET_SUPPLEMENTARY_SERVICE\n" );
    printf( "    Service :   %s\n", service_name );
    
    strcpy( last_service_name , service_name );

    return 1;
}

int handle_REQ_GET_SUPPLEMENTARY_SERVICE_REPLY( unsigned char *pkt, unsigned short )
{
    get_supplementary_service_ret *pr = (get_supplementary_service_ret *)pkt;
    
    printf( "Trap reply: REQ_GET_SUPPLEMENTARY_SERVICE\n" );
    printf( "    Error : %u\n", pr->err );
    printf( "    ID:     0x%.08x\n", pr->id );
    
    ServiceNameAndId *new_entry = (ServiceNameAndId *)calloc( 1, sizeof( ServiceNameAndId ) );
    new_entry->service_name = strdup( last_service_name );
    new_entry->service_id = pr->id;
    new_entry->service_decode = get_supp_service_decoder( last_service_name );
    
    new_entry->next = services;
    services = new_entry;
    
    return 1;
}

int handle_REQ_PERFORM_SUPPLEMENTARY_SERVICE( unsigned char * pkt, unsigned short len )
{
    perform_supplementary_service_req * prq = ( perform_supplementary_service_req * ) pkt;
    unsigned char * ssd = ( unsigned char * ) &prq[1];
    unsigned short ssd_len = ( len - ( unsigned short ) sizeof( perform_supplementary_service_req ) );
    
    printf( "Debugger request: REQ_PERFORM_SUPPLEMENTARY_SERVICE\n" );
    printf( "    ID: 0x%.08x", prq->id );
    
    ServiceNameAndId *lookup = services;
    while( lookup ) {
        if( prq->id == lookup->service_id ) {
            printf( " [ %s ]", lookup->service_name );
            break;            
        }
        lookup = lookup->next;
    }
    
    last_service = lookup;

    printf( "\n" );
    
    if( ssd_len ) {
        int rc = 0;
        if( lookup && lookup->service_decode )
            rc = lookup->service_decode( 1, ssd, ssd_len );
        
        if( 0 != rc ) {
            printf( "    supplementary service data:\n" );
            DumpPacket( ssd, ssd_len, 1 );
        }
    }
    
    return 1;
}

int handle_REQ_PERFORM_SUPPLEMENTARY_SERVICE_REPLY( unsigned char * pkt, unsigned short len)
{
    int rc = -1;
    if( last_service ) {
        if( last_service && last_service->service_decode )
            rc = last_service->service_decode( 0, pkt, len );
    }
            
    if( 0 != rc ) {
        printf( "    supplementary service response:\n" );
        DumpPacket( pkt, len, 1 );
    }
    
    return 1;
}

int handle_REQ_GET_SYS_CONFIG( unsigned char * , unsigned short )
{
    printf( "Debugger request: REQ_GET_SYS_CONFIG\n" );
    
    return 1;
}

int handle_REQ_GET_SYS_CONFIG_REPLY( unsigned char * pkt, unsigned short )
{
    system_config * pr = ( system_config * ) pkt;

    printf( "Trap reply: REQ_GET_SYS_CONFIG\n" );
    
    printf( "    CPU:        %u\n", pr->cpu );
    printf( "    FPU:        %u\n", pr->fpu );
    printf( "    OS Major:   %u\n", pr->osmajor );
    printf( "    OS Minor:   %u\n", pr->osminor );
    if( pr->os < MAD_OS_MAX ) {
        printf( "    OS:         %s\n", mad_os_desc[pr->os] );
    } else {
        printf( "    OS:         %u\n", pr->os );
    }
    printf( "    Huge Shift: %u\n", pr->huge_shift );
    if( pr->mad < MAD_MAX ) {
        printf( "    MAD:        %s\n", mad_desc[pr->mad] );
    } else {
        printf( "    MAD:        0x%.04x\n", pr->mad );
    }

    /* Set so we can decode registers */
    read_mad_handle = pr->mad;

    return 1;
}

int handle_REQ_MAP_ADDR( unsigned char * pkt, unsigned short )
{
    map_addr_req * prq = ( map_addr_req * ) pkt;
    printf( "Debugger request: REQ_MAP_ADDR\n" );
    if( prq->in_addr.segment == MAP_FLAT_CODE_SELECTOR )
        printf( "    Address:    FLAT_CODE:%.08x\n", prq->in_addr.offset );
    else if( prq->in_addr.segment == MAP_FLAT_DATA_SELECTOR )
        printf( "    Address:    FLAT_DATA:%.08x\n", prq->in_addr.offset );
    else
        printf( "    Address:    %.04x:%.08x\n", prq->in_addr.segment, prq->in_addr.offset );
    printf( "    MAD Handle: %.08x\n", prq->handle );

    return 1;
}

int handle_REQ_MAP_ADDR_REPLY( unsigned char * pkt, unsigned short )
{
    map_addr_ret * pr = ( map_addr_ret * ) pkt;
    
    printf( "Trap reply: REQ_MAP_ADDR\n" );
    printf( "    Address:    %.04x:%.08x\n", pr->out_addr.segment, pr->out_addr.offset );
    printf( "    Low bound:  %.08x\n", pr->lo_bound );
    printf( "    High bound: %.08x\n", pr->hi_bound );
    
    return 1;
}

int handle_REQ_CHECKSUM_MEM( unsigned char * pkt, unsigned short )
{
    checksum_mem_req * prq = ( checksum_mem_req * ) pkt;
    printf( "Debugger request: REQ_CHECKSUM_MEM\n" );
    printf( "    Address:    %.04x:%.08x\n", prq->in_addr.segment, prq->in_addr.offset );
    printf( "    Length:     %.04x\n", prq->len );
    return 1;
}

int handle_REQ_CHECKSUM_MEM_REPLY( unsigned char * pkt, unsigned short )
{
    checksum_mem_ret * pr = ( checksum_mem_ret * ) pkt;

    printf( "Trap reply: REQ_CHECKSUM_MEM\n" );
    printf( "    Checksum:     %.08x\n", pr->result );
    
    return 1;
}

int handle_REQ_READ_MEM( unsigned char * pkt, unsigned short )
{
    read_mem_req * prq = ( read_mem_req * ) pkt;
    printf( "Debugger request: REQ_READ_MEM\n" );
    printf( "    Address:    %.04x:%.08x\n", prq->mem_addr.segment, prq->mem_addr.offset );
    printf( "    Length:     %.04x\n", prq->len );
    return 1;
}

int handle_REQ_READ_MEM_REPLY( unsigned char * pkt, unsigned short len )
{
    printf( "Trap reply: REQ_READ_MEM\n" );
    printf( "    Length: 0x%.04x\n", len );
    
    if( len ){
        printf( "    Data:\n" );
        DumpPacket( pkt, len, 1 );
    }
    
    return 1;
}

int handle_REQ_WRITE_MEM( unsigned char * pkt, unsigned short len )
{
    write_mem_req * prq = ( write_mem_req * ) pkt;
    unsigned short to_write = ( len - ( unsigned short ) sizeof( write_mem_req ) );
    unsigned char * data = ( unsigned char * ) &prq[1];
    
    printf( "Debugger request: REQ_WRITE_MEM\n" );
    printf( "    Address:    %.04x:%.08x\n", prq->mem_addr.segment, prq->mem_addr.offset );
    printf( "    Length:     %.04x\n", to_write );
    printf( "    Data:\n" );
    DumpPacket( data, to_write, 1 );
    
    return 1;
}

int handle_REQ_WRITE_MEM_REPLY( unsigned char * pkt, unsigned short )
{
    write_mem_ret * pr = ( write_mem_ret * ) pkt;

    printf( "Trap reply: REQ_WRITE_MEM\n" );
    printf( "    Length: 0x%.04x\n", pr->len );

    return 1;
}

int handle_REQ_READ_IO( unsigned char * pkt, unsigned short )
{
    read_io_req * prq = ( read_io_req * ) pkt;
    printf( "Debugger request: REQ_READ_IO\n" );
    printf( "    IO Address: %.08x\n", prq->IO_offset );
    printf( "    Length:     %.02x\n", prq->len );
    return 1;
}

int handle_REQ_READ_IO_REPLY( unsigned char * pkt, unsigned short len )
{
    printf( "Trap reply: REQ_READ_IO\n" );
    printf( "    Length: 0x%.04x\n", len );
    
    if( len ){
        printf( "    Data:\n" );
        DumpPacket( pkt, len, 1 );
    }
    return 1;
}

int handle_REQ_WRITE_IO( unsigned char * pkt, unsigned short len )
{
    write_io_req * prq = ( write_io_req * ) pkt;
    unsigned short to_write = ( len - ( unsigned short ) sizeof( write_io_req ) );
    unsigned char * data = ( unsigned char * ) &prq[1];

    printf( "Debugger request: REQ_WRITE_IO\n" );
    printf( "    Address:    %.08x\n", prq->IO_offset );
    printf( "    Length:     %.04x\n", to_write );
    printf( "    Data:\n" );
    DumpPacket( data, to_write, 1 );
    
    return 1;
}

int handle_REQ_WRITE_IO_REPLY( unsigned char * pkt, unsigned short )
{
    write_io_ret * pr = ( write_io_ret * ) pkt;

    printf( "Trap reply: REQ_WRITE_IO\n" );
    printf( "    Length: %u\n", pr->len );
    
    return 0;
}

int handle_REQ_PROG_GO( unsigned char * , unsigned short )
{
    printf( "Debugger request: REQ_PROG_GO\n" );
    return 1;
}

void PrintConditions( unsigned short cond )
{
    printf( "    Conditions: " );
    if( cond & COND_CONFIG )
        printf( "CONFIG " );
    if( cond & COND_SECTIONS )
        printf( "SECTIONS " );
    if( cond & COND_LIBRARIES )
        printf( "LIBRARIES " );
    if( cond & COND_ALIASING )
        printf( "ALIASING " );
    if( cond & COND_THREAD )
        printf( "THREAD " );
    if( cond & COND_THREAD_EXTRA )
        printf( "THREAD_EXTRA " );
    if( cond & COND_TRACE )
        printf( "TRACE " );
    if( cond & COND_BREAK )
        printf( "BREAK " );
    if( cond & COND_WATCH )
        printf( "WATCH " );
    if( cond & COND_USER )
        printf( "USER " );
    if( cond & COND_TERMINATE )
        printf( "TERMINATE " );
    if( cond & COND_EXCEPTION )
        printf( "EXCEPTION " );
    if( cond & COND_MESSAGE )
        printf( "MESSAGE " );
    if( cond & COND_STOP )
        printf( "STOP " );
    printf( "\n" );
}

int handle_REQ_PROG_GO_REPLY( unsigned char * pkt, unsigned short )
{
    prog_go_ret * pr = ( prog_go_ret * ) pkt;

    printf( "Trap reply: REQ_PROG_GO\n" );
    
    printf( "    Stack:  %.04x:%.08x\n", pr->stack_pointer.segment, pr->stack_pointer.offset );
    printf( "    PC:     %.04x:%.08x\n", pr->program_counter.segment, pr->program_counter.offset );
    PrintConditions( pr->conditions );

    return 1;
}

int handle_REQ_PROG_STEP( unsigned char * , unsigned short )
{
    printf( "Debugger request: REQ_PROG_STEP\n" );
    return 1;
}

int handle_REQ_PROG_STEP_REPLY( unsigned char * pkt, unsigned short )
{
    prog_step_ret * pr = ( prog_step_ret * ) pkt;

    printf( "Trap reply: REQ_PROG_STEP\n" );
    
    printf( "    Stack:  %.04x:%.08x\n", pr->stack_pointer.segment, pr->stack_pointer.offset );
    printf( "    PC:     %.04x:%.08x\n", pr->program_counter.segment, pr->program_counter.offset );
    PrintConditions( pr->conditions );

    return 1;
}

int handle_REQ_PROG_LOAD( unsigned char * pkt, unsigned short len )
{
    prog_load_req   *prq = (prog_load_req *)pkt;
    char            *p = (char *)pkt;
    size_t          idx;
    
    printf( "Debugger request: REQ_PROG_LOAD\n" );
    idx = sizeof( prog_load_req );
    printf( "    Program:    %s\n", p + idx );
    idx += strlen( p + idx ) + 1;
    if( idx < len ) {
        printf( "    Args:       %s\n", p + idx );
        idx += strlen( p + idx ) + 1;
        while( idx < len ) {
            printf( "                %s\n", p + idx );
            idx += strlen( p + idx ) + 1;
        }
    }
    printf( "    True argv:  %u\n", prq->true_argv );
    
    return 1;
}

int handle_REQ_PROG_LOAD_REPLY( unsigned char * pkt, unsigned short )
{
    prog_load_ret * pr = ( prog_load_ret * ) pkt;
    
    printf( "Trap reply: REQ_PROG_LOAD\n" );
    printf( "    Error:          %u\n", pr->err );
    if( !pr->err ) {
        printf( "    Process ID:     0x%.08x\n", pr->task_id );
        printf( "    Module handle:  0x%.08x\n", pr->mod_handle );
        printf( "    Flags:          0x%.02x\n", pr->flags );
        printf( "                    " );
        
        if( pr->flags & LD_FLAG_IS_BIG )            printf( "LD_FLAG_IS_BIG " );
        if( pr->flags & LD_FLAG_IS_PROT )           printf( "LD_FLAG_IS_PROT " );
        if( pr->flags & LD_FLAG_IS_STARTED )        printf( "LD_FLAG_IS_STARTED " );
        if( pr->flags & LD_FLAG_IGNORE_SEGMENTS )   printf( "LD_FLAG_IGNORE_SEGMENTS " );
        if( pr->flags & LD_FLAG_HAVE_RUNTIME_DLLS ) printf( "LD_FLAG_HAVE_RUNTIME_DLLS " );
        if( pr->flags & LD_FLAG_DISPLAY_DAMAGED )   printf( "LD_FLAG_DISPLAY_DAMAGED " );
        printf( "\n" );
    }
    return 1;
}

int handle_REQ_PROG_KILL( unsigned char * pkt, unsigned short )
{
    prog_kill_req * prq = ( prog_kill_req * ) pkt;
    printf( "Debugger request: REQ_PROG_KILL\n" );
    printf( "    Task ID: 0x%.08x\n", prq->task_id );
    
    return 1;
}

int handle_REQ_PROG_KILL_REPLY( unsigned char * pkt, unsigned short )
{
    prog_kill_ret * pr = ( prog_kill_ret * ) pkt;
    
    printf( "Trap reply: REQ_PROG_KILL\n" );
    printf( "    Error   : 0x%.08x\n", pr->err );
    
    return 1;
}

int handle_REQ_SET_WATCH( unsigned char * pkt, unsigned short )
{
    set_watch_req * prq = ( set_watch_req * ) pkt;
    
    printf( "Debugger request: REQ_SET_WATCH\n" );
    printf( "    Address:    %.04x:%.08x\n", prq->watch_addr.segment, prq->watch_addr.offset );
    printf( "    Size:       %.04x\n", prq->size );
    
    return 1;
}

int handle_REQ_SET_WATCH_REPLY( unsigned char * pkt, unsigned short )
{
    set_watch_ret * pr = ( set_watch_ret * ) pkt;

    printf( "Trap reply: REQ_SET_WATCH\n" );
    printf( "    Error:  0x%.08x\n", pr->err );
    unsigned_32 mult = ( pr->multiplier & ~USING_DEBUG_REG );
    printf( "    Multiplier:  0x%.08x\n", mult );
    if( pr->multiplier & USING_DEBUG_REG )
        printf( "    Using:  hardware registers\n" ); 
    
    return 1;
}

int handle_REQ_CLEAR_WATCH( unsigned char * pkt, unsigned short )
{
    clear_watch_req * prq = ( clear_watch_req * ) pkt;
    
    printf( "Debugger request: REQ_CLEAR_WATCH\n" );
    printf( "    Address:    %.04x:%.08x\n", prq->watch_addr.segment, prq->watch_addr.offset );
    printf( "    Size:       %.04x\n", prq->size );
    
    return 1;
}

int handle_REQ_CLEAR_WATCH_REPLY( unsigned char * , unsigned short )
{
    return 0;
}

int handle_REQ_SET_BREAK( unsigned char * pkt, unsigned short )
{
    set_break_req * prq = ( set_break_req * ) pkt;
    
    printf( "Debugger request: REQ_SET_BREAK\n" );
    printf( "    Address:    %.04x:%.08x\n", prq->break_addr.segment, prq->break_addr.offset );
   
    return 1;
}

int handle_REQ_SET_BREAK_REPLY( unsigned char * pkt, unsigned short )
{
    set_break_ret * pr = ( set_break_ret * ) pkt;

    printf( "Trap reply: REQ_SET_BREAK\n" );
    printf( "    Old:        %.08x\n", pr->old );
    
    return 1;
}

int handle_REQ_CLEAR_BREAK( unsigned char * pkt, unsigned short )
{
    clear_break_req * prq = ( clear_break_req * ) pkt;
    
    printf( "Debugger request: REQ_CLEAR_BREAK\n" );
    printf( "    Address:    %.04x:%.08x\n", prq->break_addr.segment, prq->break_addr.offset );
    printf( "    Old:        %.08x\n", prq->old );
    
    return 1;
}

int handle_REQ_CLEAR_BREAK_REPLY( unsigned char * , unsigned short )
{
    return 0;
}

int handle_REQ_GET_NEXT_ALIAS( unsigned char * pkt, unsigned short )
{
    get_next_alias_req * prq = ( get_next_alias_req * ) pkt;
    
    printf( "Debugger request: REQ_GET_NEXT_ALIAS\n" );
    printf( "    Segment:    %.04x\n", prq->seg );
    
    return 1;
}

int handle_REQ_GET_NEXT_ALIAS_REPLY( unsigned char * pkt, unsigned short )
{
    get_next_alias_ret * pr = ( get_next_alias_ret * ) pkt;
    
    printf( "Trap reply: REQ_GET_NEXT_ALIAS\n" );
    if( pr->seg ) {
        printf( "    Segment:    %.04x\n", pr->seg );
        if( pr->alias ) {
            printf( "    Alias:      %.04x\n", pr->alias );
        } else {
            printf( "    *** DELETED ***\n" );
        }
    } else {
        printf( "    *** END OF LIST ***\n" );
    }

    return 1;
}

int handle_REQ_SET_USER_SCREEN( unsigned char * , unsigned short )
{
    printf( "Debugger request: REQ_SET_USER_SCREEN\n" );
    
    return 1;
}

int handle_REQ_SET_USER_SCREEN_REPLY( unsigned char * , unsigned short )
{
    return 0;
}

int handle_REQ_SET_DEBUG_SCREEN( unsigned char * , unsigned short )
{
    printf( "Debugger request: REQ_SET_DEBUG_SCREEN\n" );
    return 1;
}

int handle_REQ_SET_DEBUG_SCREEN_REPLY( unsigned char * , unsigned short )
{
    return 0;
}

int handle_REQ_READ_USER_KEYBOARD( unsigned char * pkt, unsigned short )
{
    read_user_keyboard_req * prq = ( read_user_keyboard_req * ) pkt;
    
    printf( "Debugger request: REQ_READ_USER_KEYBOARD\n" );
    printf( "    Wait:   %u\n", prq->wait );
    
    return 1;
}

int handle_REQ_READ_USER_KEYBOARD_REPLY( unsigned char * pkt, unsigned short )
{
    read_user_keyboard_ret * pr = ( read_user_keyboard_ret * ) pkt;
    
    printf( "Trap reply: REQ_READ_USER_KEYBOARD\n" );
    printf( "    Wait:   %.02x\n", pr->key );
    
    return 1;
}

int handle_REQ_GET_LIB_NAME( unsigned char * pkt, unsigned short )
{
    get_lib_name_req * prq = ( get_lib_name_req * ) pkt;
    
    printf( "Debugger request: REQ_GET_LIB_NAME\n" );
    printf( "    MAD Handle: %u\n", prq->handle );
    
    return 1;
}

int handle_REQ_GET_LIB_NAME_REPLY( unsigned char * pkt, unsigned short )
{
    get_lib_name_ret *pr = ( get_lib_name_ret * ) pkt;
    char * name = ( char * ) &pr[1];
    
    printf( "Trap reply: REQ_GET_LIB_NAME\n" );
    if( pr->handle ) {
        printf( "    MAD Handle: %u\n", pr->handle );
        if( *name ) {
            printf( "    Name:       %s\n", name );
        } else {
            printf( "    *** DELETED ***\n" );
        }
    } else {
        printf( "    *** END OF LIST ***\n" );
    }
    
    return 1;
}

int handle_REQ_GET_ERR_TEXT( unsigned char * pkt, unsigned short )
{
    get_err_text_req * prq = ( get_err_text_req * ) pkt;

    printf( "Debugger request: REQ_GET_ERR_TEXT\n" );
    printf( "    Error:  %.08x\n", prq->err );
    
    return 1;
}

int handle_REQ_GET_ERR_TEXT_REPLY( unsigned char * , unsigned short )
{
    return 0;
}

int handle_REQ_GET_MESSAGE_TEXT( unsigned char * , unsigned short )
{
    printf( "Debugger request: REQ_GET_MESSAGE_TEXT\n" );
    
    return 1;
}

int handle_REQ_GET_MESSAGE_TEXT_REPLY( unsigned char * pkt, unsigned short )
{
    get_message_text_ret * pr = ( get_message_text_ret * ) pkt;
    char * text = ( char * ) &pr[1];
    
    printf( "Trap reply: REQ_GET_MESSAGE_TEXT\n" );
    printf( "    Flags:      %.02x\n", pr->flags );
    printf( "    Message:    $s\n", text );
    
    return 1;
}

int handle_REQ_REDIRECT_STDIN( unsigned char * pkt, unsigned short )
{
    redirect_stdin_req * prq = ( redirect_stdin_req * ) pkt;
    char * name = ( char * ) &prq[1];

    printf( "Debugger request: REQ_REDIRECT_STDIN\n" );
    printf( "    Name:   %s\n", name );
    
    return 1;
}

int handle_REQ_REDIRECT_STDIN_REPLY( unsigned char * pkt, unsigned short )
{
    redirect_stdin_ret * pr = ( redirect_stdin_ret * ) pkt;
    
    printf( "Trap reply: REQ_REDIRECT_STDIN\n" );
    printf( "    Error:  %d\n", pr->err );
    
    return 1;
}

int handle_REQ_REDIRECT_STDOUT( unsigned char * pkt, unsigned short )
{
    redirect_stdout_req * prq = ( redirect_stdout_req * ) pkt;
    char * name = ( char * ) &prq[1];

    printf( "Debugger request: REQ_REDIRECT_STDOUT\n" );
    printf( "    Name:   %s\n", name );
    
    return 1;
}

int handle_REQ_REDIRECT_STDOUT_REPLY( unsigned char * pkt, unsigned short )
{
    redirect_stdout_ret * pr = ( redirect_stdout_ret * ) pkt;
    
    printf( "Trap reply: REQ_REDIRECT_STDOUT\n" );
    printf( "    Error:  %d\n", pr->err );
    
    return 1;
}

int handle_REQ_SPLIT_CMD( unsigned char * pkt, unsigned short )
{
    split_cmd_req * prq = ( split_cmd_req * ) pkt;
    char * cmd = ( char * ) &prq[1];
    
    printf( "Debugger request: REQ_SPLIT_CMD\n" );
    printf( "    Command:    %s\n", cmd );
    
    return 1;
}

int handle_REQ_SPLIT_CMD_REPLY( unsigned char * pkt, unsigned short )
{
    split_cmd_ret * pr = ( split_cmd_ret * ) pkt;

    printf( "Trap reply: REQ_SPLIT_CMD\n" );
    printf( "    Command end:        %.04x\n", pr->cmd_end );
    printf( "    Parameter start:    %.04x\n", pr->parm_start );
    
    return 1;
}

int handle_REQ_READ_REGS( unsigned char * , unsigned short )
{
    printf( "Debugger request: REQ_READ_REGS\n" );
    
    return 1;
}

void DumpRegistersMAX_X86( mad_registers * pregs )
{
    struct x86_mad_registers * px = &pregs->x86;
    //          XXX:
    printf( "    EAX:%.08x EBX:%.08x ECX:%.08x EDX:%.08x\n", px->cpu.eax, px->cpu.ebx, px->cpu.ecx, px->cpu.edx );
    printf( "    ESI:%.08x EDI:%.08x EBP:%.08x ESP:%.08x\n", px->cpu.esi, px->cpu.edi, px->cpu.ebp, px->cpu.esp );
    printf( "    EIP:%.08x EFL:%.08x\n", px->cpu.eip, px->cpu.efl );
    printf( "    CR0:%.08x CR2:%.08x CR3:%.08x\n", px->cpu.cr0, px->cpu.cr2, px->cpu.cr3 );
    printf( "    DS: %.04x     ES: %.04x     SS: %.04x     CS: %.04x\n", px->cpu.ds, px->cpu.es, px->cpu.ss, px->cpu.cs );
    printf( "    FS: %.04x     GS: %.04x\n", px->cpu.fs, px->cpu.gs );

    printf( "    *** to be done - FPU, MMX, XMM *** \n" );
}

void DumpRegisters( mad_registers * pregs )
{
    switch( read_mad_handle ){
    case MAD_X86:
        DumpRegistersMAX_X86( pregs );
        break;
    case MAD_AXP:
    case MAD_PPC:
    case MAD_MIPS:
    case MAD_MSJ:
    default:
        printf( "Cannot currently handle registers for MAD type %u\n", read_mad_handle );
        break;
    }
}

int handle_REQ_READ_REGS_REPLY( unsigned char * pkt, unsigned short )
{
    mad_registers * pregs = ( mad_registers * ) pkt;
    
    printf( "Trap reply: REQ_READ_REGS\n" );
    DumpRegisters( pregs );

    return 1;
}

int handle_REQ_WRITE_REGS( unsigned char * pkt, unsigned short )
{
    write_regs_req * prq = ( write_regs_req * ) pkt;
    mad_registers * pregs = ( mad_registers * ) &prq[1];
    
    printf( "Debugger request: REQ_WRITE_REGS\n" );
    DumpRegisters( pregs );
    
    return 1;
}

int handle_REQ_WRITE_REGS_REPLY( unsigned char * , unsigned short )
{
    return 0;
}

int handle_REQ_MACHINE_DATA( unsigned char * pkt, unsigned short len )
{
    machine_data_req *  prq = ( machine_data_req * ) pkt;
    unsigned char *     msd = ( unsigned char * ) &prq[1];
    unsigned short      msd_len = ( len - ( unsigned short ) sizeof( machine_data_req ) );
    
    printf( "Debugger request: REQ_MACHINE_DATA\n" );
    printf( "    Type:       %u\n", prq->info_type );
    printf( "    Address:    %.04x:%.08x\n", prq->addr.segment, prq->addr.offset );

    if( msd_len ) {
        printf( "Machine specific data - to be decoded:\n" );
        DumpPacket( msd, msd_len, 1 );
    }
    return 1;
}

int handle_REQ_MACHINE_DATA_REPLY( unsigned char * pkt, unsigned short len )
{
    machine_data_ret * pr = (machine_data_ret * ) pkt;
    unsigned char * msd = ( unsigned char * ) &pr[1];
    unsigned short  msd_len = ( len - ( unsigned short ) sizeof( machine_data_ret ) );

    printf( "Trap reply: REQ_MACHINE_DATA\n" );
    printf( "    Start:  %.08x\n", pr->cache_start );
    printf( "    End:    %.08x\n", pr->cache_end );
    
    if( msd_len ){
        printf( "Machine specific data - to be decoded:\n" );
        DumpPacket( msd, msd_len, 1 );
    }

    return 1;
}
