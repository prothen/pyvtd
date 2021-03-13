#!/usr/bin/env python2


"""


    Author: Philipp Rothenhaeusler, Stockholm 2020

"""

import attr
import time
import enum
import numpy
import typing


import vtd.generated.base as bs


class SCPMessageType(enum.IntEnum):
    Road = 0
    Precipitation = 1


@attr.s
class Controller(object):

    actor_name = attr.ib()
    vtd_ip = attr.ib(default="127.0.0.1")
    debug_enabled = attr.ib(default=False)

    # TODO: add types for rdb and scp swig classes
    rdb_interface = attr.ib(default=None)
    scp_interface = attr.ib(default=None)

    def __attrs_post_init__(self):
        print('call connect to initialise')
        print("Test default interface")

    def connect(self):
        bs.CreateRDBInterface("127.0.0.1")
        self.rdb_interface = bs.cvar.rdb_interface
        self.rdb_interface.ConnectDefaultInterface()
        print('Created connection to rdb interface')

        bs.CreateSCPInterface("127.0.0.1")
        self.scp_interface = bs.cvar.scp_interface
        self.scp_interface.ConnectSCPInterface()
        print('Created connection to scp interface')

    def ensure_message_received(self, pkg_id):
        while not self.rdb_interface.HasNewMessage(pkg_id):
            print('searching searching searching')
            self.rdb_interface.ReceiveDataOnce()
            time.sleep(0.1)
        print('received a message of id', pkg_id)

    def get_state(self):
        msg = self.rdb_interface.GetObjectState(self.actor_name)
        if self.debug_enabled:
            print("Received message:\n", msg)
            print("Received message elements base.id:\n", msg.base.id)
            print("Received message elements base.name:\n", msg.base.name)
            print("Received message elements base.pos.x:\n", msg.base.pos.x)
            print("Received message elements base.pos.y:\n", msg.base.pos.y)
        return numpy.sqrt(msg.ext.speed.x**2 + msg.ext.speed.y**2)

    def actuate(self, velocity, steering=None):
        """  """
        self.scp_interface.SendCommand('<Traffic><ActionSpeedChange actor="Ego" target="{}"/> </Traffic>'.format(velocity))
        # self.scp_interface.SendCommand('<Traffic><ActionSpeedChange actor="Ego" target="{}"/> </Traffic>'.format(speed))

    def change_road(self, *args, **kwargs):
        default_args = dict()
        default_args.update(kwargs)
        # TODO hardcode default values
        # apply environment and command
        # command = " ".join(["=".[attribute, value] for default_args.items()])
        # scp_interface.SendCommand('<Environment><Precipitation {} /> </Environment>'.format(command))
        pass

    def change_precipitation(self, command_type, *args, **kwargs):
        # see road template
        # apply environment and command
        # scp_interface.SendCommand('<Environment><Precipitation type = "snow" intensity = "1"/> </Environment>')
        pass


if __name__ == "__main__":
    try:
        controller = Controller("Ego", debug_enabled=False)
        controller.connect()
        controller.ensure_message_received(9)
        vehicle_state = controller.get_state()
        controller.actuate(300)
        t0 = time.time() + 20
        while (time.time() < t0):
            controller.ensure_message_received(9)
            state = controller.get_state()
            print(state)

    except Exception as e:
        print("Execution of tests failed with:\n", e)
