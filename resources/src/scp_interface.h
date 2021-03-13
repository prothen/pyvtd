/**
 *  @file   scp_interface.h
 *  @brief  SCP implementation of TCP base class
 *  @author Philipp Rothenhäusler
 *  @date   2020-10-31
 ***********************************************/
#pragma once

#include <map>
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
#include "scpIcd.h"
#include "tcp_interface.h"

#include "config.h"


class SCPInterface : public TCPInterface {

    public:
        SCPInterface() :
            TCPInterface(){}

        SCPInterface(char* host) :
            TCPInterface(host){
                fprintf(stderr, "Received host argument: %s", host);
            }

        void ConnectSCPInterface();
        void SendCommand(const char* text);

    protected:
        int socket_fd_;

};


extern SCPInterface  scp_interface;
extern void CreateSCPInterface(char* host);

