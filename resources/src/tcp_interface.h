/**
 *  @file   tcp_interfac.h
 *  @brief  Base class for TCP interface
 *  @author Philipp Rothenhäusler
 *  @date   2020-10-31
 ***********************************************/
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "config.h"

class TCPInterface {

    public:
        TCPInterface(){}

        TCPInterface(char* host) {
            // TODO: Debug host changes
            strcpy(host_, host);
        }

        void Initialise(int port, int& socket_fd);

    protected:
        char host_[128];
        int port_;
};
