/**
 *  @file   rdb_interface.h
 *  @brief  RDB implementation of TCP base class
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
#include "RDBHandler.hh"
#include "tcp_interface.h"

#include "config.h"


#include <cstring>


struct compmetest {
    bool operator () (char const *a, char const *b) const {
        return std::strcmp(a,b ) < 0;
}
};

class RDBInterface : public TCPInterface {

    public:
        RDBInterface(){}
        RDBInterface(char* host) :
            TCPInterface(host){
                fprintf(stderr, "Received host argument: %s", host);
                // Initialise for all desired message types
                is_new_message_received_[RDB_PKG_ID_OBJECT_STATE] = false;
                messages_[RDB_PKG_ID_OBJECT_STATE] = (char*) new RDB_MSG_ENTRY_HDR_t();
            }

        void ConnectDefaultInterface();
        void ConnectImageInterface();

        // RDB Data receiver
        void ReceiveDataOnce();
        void ReceiveData();
        // RDB Image receiver
        void ReceiveImageOnce();
        void ReceiveImage();

        RDB_OBJECT_STATE_t GetObjectState(char* object_name);
        RDB_IMAGE_t GetImage();

        bool HasNewMessage(int message_id);

    protected:
        void Receive(int socket_fd_interface);
        void SendTrigger(
                int & sendSocket,
                const double & simTime,
                const unsigned int & simFrame );

        void ParseRDBPackage(RDB_MSG_t* msg);

        void ParseRDBMessageEntry(
                const double &simTime,
                const unsigned int &simFrame,
                RDB_MSG_ENTRY_HDR_t* entryHdr);

        // TODO: initialise?
        std::map<int, bool> is_new_message_received_;
        std::map<int, char*> messages_;
        std::map<char*, RDB_OBJECT_STATE_t*, compmetest> object_states_;

        int socket_fd_default_;
        int socket_fd_image_;
};



extern RDBInterface rdb_interface;
extern void CreateRDBInterface(char * host);
