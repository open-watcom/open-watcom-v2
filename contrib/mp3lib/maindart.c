#define  INCL_PM                    /* required to use PM APIs.            */
#define  INCL_OS2MM                 /* required for MCI and MMIO headers   */
#define  INCL_DOS

#include <os2.h>
#include <os2me.h>
#include <string.h>
#include "mp3dec.h"

#define  STRING_LENGTH  128
#define  MAX_BUFFERS    16

USHORT               usDeviceID;                /* Amp Mixer device id     */
HEVENT               hevStop;                   /* Stop event semaphore    */

ULONG                ulBufferCount;             /* Current file buffer     */
ULONG                ulNumBuffers;              /* Number of file buffers  */

MCI_MIX_BUFFER       MixBuffers[MAX_BUFFERS];   /* Device buffers          */
MCI_MIXSETUP_PARMS   MixSetupParms;             /* Mixer parameters        */
MCI_BUFFER_PARMS     BufferParms;               /* Device buffer parms     */


int FillBuffer( PMCI_MIX_BUFFER buffer )
{
    memset( buffer->pBuffer, 0, BufferParms.ulBufferSize );

    if( MPG_Read_Frame() != OK )
        return( 1 );

    MPG_Decode_L3( buffer->pBuffer );
    return( 0 );
}


LONG APIENTRY MyEvent( ULONG ulStatus, PMCI_MIX_BUFFER pBuffer, ULONG ulFlags )
{
    switch( ulFlags ) {
    case MIX_STREAM_ERROR | MIX_READ_COMPLETE :  /* error occur in device */
    case MIX_STREAM_ERROR | MIX_WRITE_COMPLETE:  /* error occur in device */

        if( ulStatus == ERROR_DEVICE_UNDERRUN ) {
            /* handle ERROR_DEVICE_UNDERRUN or OVERRUN here
            */
        }
        /* Fall through! */
    case MIX_WRITE_COMPLETE:           /* for playback  */
        if( FillBuffer( pBuffer ) ) {
            DosPostEventSem( hevStop );
        } else {
            MixSetupParms.pmixWrite( MixSetupParms.ulMixHandle, pBuffer, 1 );
        }
        break;
    } /* end switch */

    return( TRUE );
} /* end MyEvent */


VOID MciError( ULONG ulError )
{
   SZ       szBuffer[ STRING_LENGTH ];
   ULONG    rc;

   rc = mciGetErrorString( ulError, szBuffer, STRING_LENGTH );

   WinMessageBox( HWND_DESKTOP, HWND_DESKTOP,
                  szBuffer, "MCI ERROR",
                  0, MB_OK | MB_ERROR | MB_MOVEABLE);
}


VOID Close( VOID )
{
   MCI_GENERIC_PARMS    GenericParms;
   ULONG                rc;

   rc = mciSendCommand( usDeviceID, MCI_BUFFER,
                        MCI_WAIT | MCI_DEALLOCATE_MEMORY,
                        ( PVOID )&BufferParms, 0 );

   if( rc != MCIERR_SUCCESS ) {
      MciError( rc );
      return;
   }

   rc = mciSendCommand( usDeviceID, MCI_CLOSE, MCI_WAIT ,
                        ( PVOID )&GenericParms, 0 );

   if( rc != MCIERR_SUCCESS ) {
      MciError( rc );
      return;
   }
}


BOOL LoadFile( CHAR szFileName[] )
{
   MCI_AMP_OPEN_PARMS   AmpOpenParms;
   ULONG                rc, ulIndex;

   /* open the mixer device
    */
   memset( &AmpOpenParms, 0, sizeof ( MCI_AMP_OPEN_PARMS ) );
   AmpOpenParms.usDeviceID = ( USHORT ) 0;
   AmpOpenParms.pszDeviceType = ( PSZ ) MCI_DEVTYPE_AUDIO_AMPMIX;

   rc = mciSendCommand( 0, MCI_OPEN,
                       MCI_WAIT | MCI_OPEN_TYPE_ID | MCI_OPEN_SHAREABLE,
                       ( PVOID ) &AmpOpenParms, 0 );

   if( rc != MCIERR_SUCCESS ) {
      MciError( rc );
      return( TRUE );
   }

   usDeviceID = AmpOpenParms.usDeviceID;

   /* Open the audio file.
    */
   if( MPG_Stream_Open( szFileName ) ) {
      return( TRUE );
   }

   /* Set the MixSetupParms data structure to match the loaded file.
    * This is a global that is used to setup the mixer.
    */
   memset( &MixSetupParms, 0, sizeof( MCI_MIXSETUP_PARMS ) );

   MixSetupParms.ulBitsPerSample = 16;
   MixSetupParms.ulFormatTag = DATATYPE_WAVEFORM;
   MixSetupParms.ulSamplesPerSec = 44100; // TODO should be variable
   MixSetupParms.ulChannels = 2;

   /* Setup the mixer for playback of wave data
    */
   MixSetupParms.ulFormatMode = MCI_PLAY;
   MixSetupParms.ulDeviceType = MCI_DEVTYPE_WAVEFORM_AUDIO;
   MixSetupParms.pmixEvent    = MyEvent;

   rc = mciSendCommand( usDeviceID, MCI_MIXSETUP,
                        MCI_WAIT | MCI_MIXSETUP_INIT,
                        ( PVOID ) &MixSetupParms, 0 );

   if( rc != MCIERR_SUCCESS ) {
      MciError( rc );
      return( TRUE );
   }

   ulNumBuffers = MAX_BUFFERS;

   /* Set up the BufferParms data structure and allocate
    * device buffers from the Amp-Mixer
    */
   BufferParms.ulNumBuffers = ulNumBuffers;
   BufferParms.ulBufferSize = 576*2*2*2;// MixSetupParms.ulBufferSize;
   BufferParms.pBufList = MixBuffers;

   rc = mciSendCommand( usDeviceID, MCI_BUFFER,
                        MCI_WAIT | MCI_ALLOCATE_MEMORY,
                        ( PVOID ) &BufferParms, 0 );

   if( ULONG_LOWD( rc ) != MCIERR_SUCCESS ) {
      MciError( rc );
      return( TRUE );
   }

   /* Fill all device buffers with data from the audio file.
    */
   for( ulIndex = 0; ulIndex < ulNumBuffers; ulIndex++) {
      MixBuffers[ ulIndex ].ulBufferLength = BufferParms.ulBufferSize;
      if( FillBuffer( &MixBuffers[ ulIndex ] ) )
         return( TRUE );
   }

   /* Set the "end-of-stream" flag
    */
   MixBuffers[ulNumBuffers - 1].ulFlags = MIX_BUFFER_EOS;

   return( FALSE );
}


BOOL ResetPlayBack( VOID )
{
   ULONG                rc;

   /* Deinitialize the Amp-Mixer to reset for playback mode
    */
   rc = mciSendCommand( usDeviceID, MCI_MIXSETUP,
                        MCI_WAIT | MCI_MIXSETUP_DEINIT,
                        ( PVOID )&MixSetupParms, 0 );

   if( rc != MCIERR_SUCCESS ) {
      MciError( rc );
      return( TRUE );
   }


   /* Setup the mixer for play-back
    * Note: MixSetupParms is a global data structure. Most of this
    *       structure must be set before this point. The LoadFile
    *       procedure is the first place that this is done.
    */
   MixSetupParms.ulFormatMode = MCI_PLAY;

   rc = mciSendCommand( usDeviceID, MCI_MIXSETUP,
                        MCI_WAIT | MCI_MIXSETUP_INIT,
                        ( PVOID ) &MixSetupParms, 0 );

   if( rc != MCIERR_SUCCESS ) {
      MciError( rc );
      return( TRUE );
   }

   return( FALSE );
}


VOID StartPlayBack( VOID )
{
   ulBufferCount = 8;

   /* Write buffers to kick off the amp mixer.
    */
   MixSetupParms.pmixWrite( MixSetupParms.ulMixHandle,
                            MixBuffers, ulNumBuffers );
}

int main( int argc, char **argv )
{
   ULONG    rc;

   if( argc != 2 ) {
      return( 1 );
   }

   rc = DosCreateEventSem( NULL, &hevStop, 0, 0 );
   if( rc ) {
      return( 1 );
   }
   /* Load the audio file and setup for playback.
    */
   rc = LoadFile( argv[1] );
   if( rc ) {
      MciError( rc );
   }

   /* Start playing
    */
   StartPlayBack();

   DosWaitEventSem( hevStop, SEM_INDEFINITE_WAIT );
   DosSleep( 1000 );

   /* Shut down
    */
   Close();

   return( FALSE );
} /* end main */
