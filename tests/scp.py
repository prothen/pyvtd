#!/usr/bin/env python2
"""

    Author: Philipp Rothenhaeusler, Stockholm 2020

"""


import vtd.generated.base as bs


if __name__ == "__main__":
    try:
        print("Start testing SCP interface")
        bs.CreateSCPInterface("127.0.0.1")
        print("--> [x] Created SCP interface successfully")
        print("Show global interfaces", bs.cvar)
        scp_interface = bs.cvar.scp_interface
        print("--> [x] Assigned interface successfully")
        scp_interface.ConnectSCPInterface()
        # print("Show scp_default_interface:\n", scp_interface)
        # print("Show scp_default_interface elements:\n", dir(scp_interface))
        # scp_interface.SendCommand('XML-Syntax`)
    except Exception as e:
        print("Execution of tests failed with:\n", e)
