%module base
%{
    #include "rdb_interface.h"
    #include "scp_interface.h"
%}

%include "stdint.i"
%include "viRDBIcd.h"
%include "tcp_interface.h"
%include "rdb_interface.h"
%include "scp_interface.h"
