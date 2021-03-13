#!/usr/bin/env python2

"""


    Author: Philipp Rothenhaeusler, Stockholm 2020

"""

import time

import vtd.generated.base as bs


print("Start testing RDB interface")
bs.CreateRDBInterface("127.0.0.1")
print("Test default interface")
rdb_interface = bs.cvar.rdb_interface
rdb_interface.ConnectDefaultInterface() #GetMessageObj()

def print_actor(name):
    msg = rdb_interface.GetObjectState("Ego")
    print("PRINT IN PYTHON:\n\n")
    print("Received message:\n", msg)
    print("Received message elements:\n", dir(msg))
    print("Received message elements ext:\n", dir(msg.ext))
    print("Received message elements ext.speed:\n", msg.ext.speed)
    print("Received message elements ext.speed:\n", msg.ext.speed.x)
    print("Received message elements base.category:\n", msg.base.category)
    print("Received message elements base.id:\n", msg.base.id)
    print("Received message elements base.name:\n", msg.base.name)
    print("Received message elements base:\n", dir(msg.base))


if __name__ == "__main__":
    try:
        #rdb_interface.ConnectImageInterface() #GetMessageObj()
        while not rdb_interface.HasNewMessage(9):
            print('searching searching searching')
            rdb_interface.ReceiveDataOnce()
            time.sleep(0.1)
        print_actor("Ego")
        print_actor("Pino")
        # euaouoeu
        print("--> [x] Created RDB interface successfully")
        print("Show global interfaces", bs.cvar)
    except Exception as e:
        print("Execution of tests failed with:\n", e)

