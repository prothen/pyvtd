#include "tcp_interface.h"


void TCPInterface::Initialise(int port, int &socket_fd){
    socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if ( socket_fd == -1 )
    {
        fprintf( stderr, "socket() failed: %s\n", strerror( errno ) );
        return;
    }

    int opt = 1;
    setsockopt ( socket_fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof( opt ) );

    struct sockaddr_in server_;
    server_.sin_family      = AF_INET;
    server_.sin_port        = htons(port);
    server_.sin_addr.s_addr = inet_addr(host_);
    struct hostent    *tmphost = NULL;
    if ( server_.sin_addr.s_addr == INADDR_NONE )
    {
        tmphost = gethostbyname(host_);
        if ( tmphost == NULL )
        {
            fprintf( stderr, "Unable to resolve server_: %s\n", host_);
            return;
        }
        memcpy( &server_.sin_addr, tmphost->h_addr_list[0], tmphost->h_length );
    }

    bool bConnected = false;
    while ( !bConnected )
    {
        if (connect(
                    socket_fd,
                    (struct sockaddr *)&server_,
                    sizeof( server_ ) ) == -1 )
        {
            fprintf( stderr, "connect() failed: %s\n", strerror( errno ) );
            sleep( 1 );
        }
        else
            bConnected = true;
    }
#if DEBUG_ENABLED
    fprintf( stderr, "TCP socket connection successful!\n" );
#endif
    return;



}

