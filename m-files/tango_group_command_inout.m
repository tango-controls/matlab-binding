function argout = tango_group_command_inout (grp_id, cmd_name, forward, varargin)
%tango_group_command_inout Executes asynchronously the TANGO command <cmd_name> on the specified group.
%
% Syntax:
% -------
%     [argout] = tango_group_command_inout(grp_id, cmd_name , forward, [, argin])
%
% Argin: grp_id
% -------------
%   |- type: 1-by-1 double array
%   |- desc: the group identifier
%
% Argin: cmd_name
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the command name
%
% Argin: forward
% --------------
%   |- type: 1-by-1 numeric array (i.e. any numeric type)
%   |- desc: if set to 1 (true) the command is forwarded to sub-groups, 0 otherwise.
%
% Argin: argin (none if command argin is DevVoid)
% -----------------------------------------------
%   |- type: one of the TANGO types (1)
%   |- desc: the command input argument
%
% Argout: argout
% --------------
%   |- type: 1-by-n struct array
%   |- desc: the command replies
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
% See also TANGO_GROUP_COMMAND_INOUT_ASYNCH, TANGO_GROUP_COMMAND_REPLY.
%
try
  argin = 0;
  if (nargin >= 4)
    argin = varargin{1};
  end
  req_id = tango_group_command_inout_asynch(grp_id, cmd_name, 0, forward, argin);
  if (tango_error == -1 || ~isstruct(req_id))
    argout = -1;
    return;
  end
  argout = tango_group_command_inout_reply(req_id, 0);
catch
  argout = 0;
end
return;
