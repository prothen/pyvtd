#include "rdb_interface.h"
//#include "swig_wrapper.cpp"

RDBInterface     rdb_interface;


/*
 *
 * Getters and Setters for RDB Data and RDB Image type packages
 *
 *
 */

RDB_OBJECT_STATE_t RDBInterface::GetObjectState(char* object_name){
    if (!is_new_message_received_[RDB_PKG_ID_OBJECT_STATE]){
        fprintf(stderr, "No message of this type has been received yet.\n");
        if (object_states_.find(object_name) == object_states_.end()){
            fprintf(stderr, "OBJECT_STATE_t does not exist for %s\n", object_name);
            return * (new RDB_OBJECT_STATE_t());
        }
        return *((RDB_OBJECT_STATE_t*) object_states_[object_name]);
    }
    is_new_message_received_[RDB_PKG_ID_OBJECT_STATE] = false;

    RDB_MSG_ENTRY_HDR_t* message_header = (RDB_MSG_ENTRY_HDR_t*) messages_[RDB_PKG_ID_OBJECT_STATE];
    int n_data_bytes = message_header->dataSize;
    int n_element_bytes = message_header->elementSize;
    int n_elements = n_data_bytes / n_element_bytes;
    char * data_pointer = ((char*) message_header) + message_header->headerSize;

#if DEBUG_ENABLED
    fprintf(stderr, "GetObjectState: Stored object has id %i (request was: %i)\n", message_header->pkgId, RDB_PKG_ID_OBJECT_STATE);
    fprintf(stderr, "GetObjectState: Return object name %s \n", object_name);
    fprintf(stderr, "GetObjectState: Parsing now ...");
#endif

    for (int i = 0; i < n_elements; i++){
        fprintf(stderr, "Start looping over elemnt:  %i \n", i);

        char*  data_entry  = new char[n_element_bytes];
        RDB_OBJECT_STATE_t * tmp_object = ((RDB_OBJECT_STATE_t*) data_pointer);
        // parse bytes to message
        memcpy( data_entry,  data_pointer, n_element_bytes);
        object_states_[tmp_object->base.name] = (RDB_OBJECT_STATE_t*) data_entry;
        //RDB_OBJECT_STATE_t tmpMessage = *((RDB_OBJECT_STATE_t*) data_pointe);
        // object_states_[((RDB_OBJECT_STATE_t*) data_pointer)->base.name] = (RDB_OBJECT_STATE_t*) data_pointer;
        data_pointer += message_header->elementSize;
#if DEBUG_ENABLED
        fprintf(stderr, "C++ Parsing element %i \n", i);
        fprintf(stderr, "Parsing object entry element %i\n", i);
        fprintf(stderr, "Parsed object name: %s\n", tmp_object->base.name);
        fprintf(stderr, "Parsed object pos: %.3lf\n", tmp_object->base.pos.x);
        fprintf(stderr, "Parsed object pos: %.3lf\n", tmp_object->base.pos.y);
        fprintf(stderr, "Parsed object speed x: %.3lf\n", tmp_object->ext.speed.x);
        fprintf(stderr, "Parsed object speed y: %.3lf\n", tmp_object->ext.speed.y);
#endif
    }
#if DEBUG_ENABLED
    fprintf(stderr, "GetObjectState:    Fetch name %s \n", object_name);
    fprintf(stderr, "GetObjectState:    Stored name %s \n", object_states_[object_name]->base.name);
#endif
    return *object_states_[object_name];
}

RDB_IMAGE_t RDBInterface::GetImage(){
    if (!is_new_message_received_[RDB_PKG_ID_IMAGE]){
        fprintf(stderr, "No message of this type has been received yet.");
        return *(new RDB_IMAGE_t());
    }
    is_new_message_received_[RDB_PKG_ID_IMAGE] = false;

    RDB_MSG_ENTRY_HDR_t* message_header = (RDB_MSG_ENTRY_HDR_t*) messages_[RDB_PKG_ID_IMAGE];
    int n_data_bytes = message_header->dataSize;
    int n_element_bytes = message_header->elementSize;
    int n_elements = n_data_bytes / n_element_bytes;

    char * data_pointer = ( (char*) message_header ) + message_header->headerSize;

#if DEBUG_ENABLED
    fprintf(stderr, "Expect id %i and got id %i", RDB_PKG_ID_OBJECT_STATE, message_header->pkgId);
    RDB_IMAGE_t tmp_message = (*(RDB_IMAGE_t*) data_pointer);
    // fprintf(stderr, "Received name %s", tmp_message.base.name);
    // fprintf(stderr, "Received id %d", tmp_message.base.id);
#endif

    return *((RDB_IMAGE_t*) data_pointer);
}

/*
 *
 * Interface specific functions for connections
 *
 *
 */

void CreateRDBInterface(char* host){
    rdb_interface = RDBInterface(host);
}

void RDBInterface::ConnectDefaultInterface(){
    Initialise(RDB_DEFAULT_PORT, socket_fd_default_);
}

void RDBInterface::ConnectImageInterface(){
    Initialise(RDB_IMAGE_PORT, socket_fd_default_);
}

void RDBInterface::ReceiveDataOnce(){
    Receive(socket_fd_default_);
}
void RDBInterface::ReceiveData(){
    // Todo: add while loop
    Receive(socket_fd_default_);
}

void RDBInterface::ReceiveImageOnce(){
    Receive(socket_fd_image_);
}
void RDBInterface::ReceiveImage(){
    // Todo: add while loop
    Receive(socket_fd_image_);
}

void RDBInterface::Receive(int socket_fd_interface){
    int socket_fd = socket_fd_interface;
    // TODO: Decide whether to make this a blocking in thread
    // or keep user interaction for reception

    unsigned int        bytesInBuffer   = 0;
    size_t              bufferSize      = sizeof( RDB_MSG_HDR_t );
    unsigned int        count           = 0;
    unsigned char       *pData          = ( unsigned char* ) calloc( 1, bufferSize );
    char*               szBuffer        = new char[DEFAULT_BUFFER];  // allocate on heap
    int                 ret;

    bool found_message_header = false;

    while ( !found_message_header ){
        if ( !( count++ % 1000 ) )
          SendTrigger( socket_fd, 0, 0 );

        ret = recv( socket_fd, szBuffer, DEFAULT_BUFFER, 0 );

        if ( ret == -1 ){
            printf( "recv() failed: %s\n", strerror( errno ) );
            break;
        }

        if ( ret != 0 ){
            // Allow dynamic expansion of buffer if more bytes than buffer size received
            if ( ( bytesInBuffer + ret ) > bufferSize ){
                pData      = ( unsigned char* ) realloc( pData, bytesInBuffer + ret );
                bufferSize = bytesInBuffer + ret;
            }

            memcpy( pData + bytesInBuffer, szBuffer, ret );
            bytesInBuffer += ret;

            // Search for package header (RDB_MSG_HDR_t)
            // Contains multiple RDB_MSG_ENTRY_HDR_t which optionally
            // contain multiple data messages (e.g. RDB_OBJECT_STATE_t)
            if ( bytesInBuffer >= sizeof( RDB_MSG_HDR_t ) ){
                RDB_MSG_HDR_t* hdr = ( RDB_MSG_HDR_t* ) pData;

                // Test that received data is valid with the
                // constant field named magicNo
                if ( hdr->magicNo != RDB_MAGIC_NO ){
                    printf( "message receiving is out of sync; discarding data" );
                    bytesInBuffer = 0;
                } else{
                    found_message_header = true;
                }
            }
        }
    }
    // TODO: Subsequent if only necessary due to loop break residuals from example code segments
    if (found_message_header) {
        RDB_MSG_HDR_t* hdr = ( RDB_MSG_HDR_t* ) pData;

        // loop through all RDB packages
        while ( bytesInBuffer >= ( hdr->headerSize + hdr->dataSize ) ){
            unsigned int msgSize = hdr->headerSize + hdr->dataSize;

            ParseRDBPackage( ( RDB_MSG_t* ) pData);
            memmove( pData, pData + msgSize, bytesInBuffer - msgSize );
            bytesInBuffer -= msgSize;
        }

    }
    //TODO: debug whether we ever close
    // subsequent is from example code
    // ::close(socket_fd_);
}

void RDBInterface::ParseRDBPackage( RDB_MSG_t* msg){
    if ( !msg || !msg->hdr.dataSize )
      return;

    RDB_MSG_ENTRY_HDR_t* entry = ( RDB_MSG_ENTRY_HDR_t* ) ( (
                ( char* ) msg ) + msg->hdr.headerSize );

    // remainingBytes is how large the RDB message is
    // --> how many bytes of pairs of (entry, data)
    uint32_t remainingBytes    = msg->hdr.dataSize;

    // Loop through message entry size
    while ( remainingBytes ){
        // parse the entry and data
        ParseRDBMessageEntry( msg->hdr.simTime, msg->hdr.frameNo, entry );
        // remove the parse (entry,data) pair
        remainingBytes -= ( entry->headerSize + entry->dataSize );

        // prepare the next entry header for parsing
        if ( remainingBytes )
          entry = ( RDB_MSG_ENTRY_HDR_t* ) (
                      ( ( char* ) entry ) + entry->headerSize + entry->dataSize );
    }
#if DEBUG_ENABLED
    fprintf( stderr, "All received packages have been parsed successfully.\n" );
#endif
}

void RDBInterface::ParseRDBMessageEntry(
        const double & simTime,
        const unsigned int & simFrame, RDB_MSG_ENTRY_HDR_t* entryHdr ){
    if ( !entryHdr )
        return;

    int noElements = entryHdr->elementSize ? ( entryHdr->dataSize / entryHdr->elementSize ) : 0;

    // TODO: Example residual: some messages require special treatment, but all return atm.
    // can be removed?
    if ( !noElements ) {
        switch ( entryHdr->pkgId )
        {
            case RDB_PKG_ID_START_OF_FRAME:
                // fprintf( stderr, "void ParseRDBMessageEntry: got start of frame\n" );
                break;

            case RDB_PKG_ID_END_OF_FRAME:
                // fprintf( stderr, "void ParseRDBMessageEntry: got end of frame\n" );
                break;

            default:
                return;
                break;
        }
        return;
    }

    char*         dataPtr = ( char* ) entryHdr;

    dataPtr += entryHdr->headerSize;

    int message_size = entryHdr->headerSize + entryHdr->dataSize;
    // allocate buffer
    char*  message  = new char[message_size];
    // parse bytes to message
    memcpy( message, (char*) entryHdr, message_size);
    messages_[entryHdr->pkgId] = message;
    is_new_message_received_[entryHdr->pkgId] = true;

#if DEBUG_ENABLED
    switch(entryHdr->pkgId){
        case RDB_PKG_ID_OBJECT_STATE:{
            RDB_OBJECT_STATE_t tmp_message = (
                    *(RDB_OBJECT_STATE_t*) ((char*)entryHdr + entryHdr->headerSize));

            RDB_OBJECT_STATE_t tmp2_message = (
                    *(RDB_OBJECT_STATE_t*) ((char*)message + entryHdr->headerSize));

            fprintf(stderr, "Received name %s", tmp_message.base.name);
            fprintf(stderr, "Received id %d", tmp_message.base.id);
            fprintf(stderr, "Received speed x %.31f", tmp_message.ext.speed.x);
            fprintf(stderr, "Received speed x %.31f", tmp_message.ext.speed.y);

            fprintf(stderr, "Received name %s", tmp2_message.base.name);
            fprintf(stderr, "Received id %d", tmp2_message.base.id);
            fprintf(stderr, "Received speed x %.31f", tmp2_message.ext.speed.x);
            break;
            }
        default:
            break;
    }
    fprintf( stderr, "Received pkg id %i with elements %i \n",
            entryHdr->pkgId, (int)entryHdr->dataSize/entryHdr->elementSize );
#endif

}

bool RDBInterface::HasNewMessage(int message_id){
    return is_new_message_received_[message_id];
}


void RDBInterface::SendTrigger(
        int & sendSocket,
        const double & simTime,
        const unsigned int & simFrame ){
    // TODO: Decide whether this trigger should be sent in this way
  Framework::RDBHandler myHandler;

  myHandler.initMsg();

  RDB_TRIGGER_t *myTrigger = ( RDB_TRIGGER_t* ) myHandler.addPackage( simTime, simFrame, RDB_PKG_ID_TRIGGER );

  if ( !myTrigger )
    return;

  myTrigger->frameNo = simFrame + 1;
  myTrigger->deltaT  = 0.043;

  int retVal = send( sendSocket, ( const char* ) ( myHandler.getMsg() ), myHandler.getMsgTotalSize(), 0 );

  if ( !retVal )
    fprintf( stderr, "SendTrigger: could not send trigger\n" );

}

