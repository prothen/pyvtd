#include "scp_interface.h"


SCPInterface scp_interface;


void CreateSCPInterface(char* host){
    scp_interface = SCPInterface(host);
}

void SCPInterface::ConnectSCPInterface(){
    Initialise(SCP_PORT, socket_fd_);
}

void SCPInterface::SendCommand(const char* text){
    if ( !text )
        return;

    if ( !strlen( text ) )
        return;

    size_t         totalSize = sizeof( SCP_MSG_HDR_t ) + strlen( text );
    SCP_MSG_HDR_t* msg       = ( SCP_MSG_HDR_t* ) new char[ totalSize ];

    // target pointer for actual SCP message data
    char* pData = ( char* ) msg;
    pData += sizeof( SCP_MSG_HDR_t );

    // fill the header information
    msg->magicNo  = SCP_MAGIC_NO;
    msg->version  = SCP_VERSION;
    msg->dataSize = strlen( text );
    sprintf( msg->sender, "ExampleConsole" );
    sprintf( msg->receiver, "any" );

    // fill the actual data section (no trailing \0 required)

    memcpy( pData, text, strlen( text ) );

    int retVal = ::send( socket_fd_, msg, totalSize, 0 );

    if ( !retVal )
        fprintf( stderr, "sendSCPMessage: could not send message.\n" );

#if DEBUG_ENABLED
    fprintf( stderr, "sendSCPMessage: sent %d characters in a message of %i bytes: ***%s***\n",
                     msg->dataSize, totalSize, text);
#endif

    // free allocated buffer space
    delete msg;
}

