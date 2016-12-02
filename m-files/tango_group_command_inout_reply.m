function argout = tango_group_command_inout_reply (request_desc, timeout)
%TANGO_GROUP_COMMAND_INOUT_REPLY Returns the result of a pending asynch. command
%
% Syntax:
% -------
%     argout = tango_group_command_inout_reply(request_desc, timeout)
%
% Argin: request_desc 
% -------------------
%   |- type: 1-by-1 struct array
%   |- desc: the asynch. request descriptor (previously returned by tango_command_inout_asynch)
%   |- note: DO NOT MODIFY (OR ANSWER MAY BE LOST)
%
% Argin: timeout 
% --------------
%   |- type: 1-by-1 numeric array (i.e. any numeric type)
%   |- desc: If after <timeout> milliseconds the reply is still not available then generate an error. 
%            If set to 0, wait indefinitly. 
%
% Argout: argout
% --------------
%   |- type: 1-by-n struct array
%   |- desc: the command replies
%
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
% See also TANGO_GROUP_COMMAND_INOUT_ASYNCH, TANGO_GROUP_COMMAND_INOUT.
%

% TANGO_COMMAND_INOUT_REPLY cmd-id: 59
try
  argout = tango_binding(int16(59), request_desc, timeout);
catch
 %ignore <argout not set> warning if cmd does not return any value
 %just set it to 0 (i.e. no error)
 argout = 0
end
return;
