function argout = tango_command_inout (dev_name, cmd_name, varargin)
%TANGO_COMMAND_INOUT Executes the TANGO command <cmd_name> on the specified device.
%
% Syntax:
% -------
%     [argout] = tango_command_inout(dev_name, cmd_name [, argin])
%
% Argin: dev_name 
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the target device
%
% Argin: cmd_name
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the command 
%
% Argin: argin (none if command argin is DevVoid)
% -----------------------------------------------
%   |- type: one of the TANGO types (1)
%   |- desc: the command input argument
%
% Argout: argout (none if command argout is DevVoid)
% --------------------------------------------------
%   |- type: one of the TANGO types (1)
%   |- desc: the command result
%   
%  (1) - TANGO types <-> Matlab types mapping
%
%       TANGO                       Matlab
%     ---------                  -------------
%     DEV_VOID...................none
%     DEV_STATE..................1-by-n char
%     DEV_STRING.................1-by-n char
%     DEV_BOOLEAN................1-by-1 uint8
%     DEV_SHORT..................1-by-1 int16
%     DEV_USHORT.................1-by-1 uint16
%     DEV_LONG...................1-by-1 int32
%     DEV_ULONG..................1-by-1 uint32
%     DEV_FLOAT..................1-by-1 single
%     DEV_DOUBLE.................1-by-1 double
%     DEVVAR_CHARARRAY...........1-by-n char
%     DEVVAR_STRINGARRAY.........1-by-n cell{1-by-n char}
%     DEVVAR_SHORTARRAY..........1-by-n int16
%     DEVVAR_USHORTARRAY.........1-by-n uint16
%     DEVVAR_LONGARRAY...........1-by-n int32
%     DEVVAR_ULONGARRAY..........1-by-n uint32
%     DEVVAR_FLOATARRAY..........1-by-n single
%     DEVVAR_DOUBLEARRAY.........1-by-n double
%     DEVVAR_LONGSTRINGARRAY.....1-by-1 struct[lvalue: 1-by-n int32,  svalue: 1-by-n cell{1-by-n char}]
%     DEVVAR_DOUBLESTRINGARRAY...1-by-1 struct[dvalue: 1-by-n double, svalue: 1-by-n cell{1-by-n char}]
%
% Example I - DEVVAR_STRINGARRAY<-cmd->DEV_STRING:
% ------------------------------------------------
%   %- exec command
%   info = tango_command_inout('sys/database/dbds', 'DbGetServerInfo', '*');
%   %- always check error
%   if (tango_error == -1)
%     %- handle error
%     tango_print_error_stack;
%     return;
%   end
%   %- info is a valid 1-by-n cell{1-by-n char} (i.e. DEVVAR_STRINGARRAY)
%
% Example II - DEV_STRING<-cmd->DEV_VOID:
% ------------------------------------------------
%   %- exec command
%   state = tango_command_inout('sys/database/dbds', 'State');
%   %- always check error
%   if (tango_error == -1)
%     %- handle error
%     tango_print_error_stack;
%     return;
%   end
%   %- state is a valid 1-by-n char array (i.e. DEV_STRING)
%
% See also TANGO_COMMAND_INOUT_ASYNCH.
%

% TANGO_COMMAND_INOUT cmd-id: 4
% check # of input argument
try
  argin = 0;
  if (nargin > 2)
    argin = varargin{1};
  end
  argout = tango_binding(int16(4), dev_name, cmd_name, argin);
catch
  argout = 0;
end
return;
