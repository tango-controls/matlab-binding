function cmd_info = tango_command_query (dev_name, cmd_name)
%TANGO_COMMAND_QUERY Query the specified device for information about a single command.  
%
% Syntax:
% -------
%   cmd_info = tango_command_query(dev_name, cmd_name)
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
% Argout: cmd_info
% ----------------
%   |- type: 1-by-1 struct array
%   |- desc: information about the <cmd_name> TANGO command
%   |- fields: 
%       |
%       |-|- name: cmd_name
%       | |- type: 1-by-n char array (string)
%       | |- desc: the command name
%       |
%       |-|- name: cmd_tag
%       | |- type: 1-by-1 double array
%       | |- desc: the command tag (for TACO)
%       |
%       |-|- name: in_type
%       | |- type: 1-by-n char array (string)
%       | |- desc: argin Matlab type (1)
%       |
%       |-|- name: in_type_desc
%       | |- type: 1-by-n char array (string)
%       | |- desc: argin description 
%       |
%       |-|- name: out_type
%       | |- type: 1-by-n char array (string)
%       | |- desc: argout Matlab type (1)
%       |
%       |-|- name: out_type_desc
%       | |- type: 1-by-n char array (string)
%       | |- desc: argout description 
%       |
%       |-|- name: disp_level
%       | |- type: 1-by-1 double array
%       | |- desc: the command display level as binary value (2)
%       |
%       |-|- name: disp_level_str
%         |- type: 1-by-n char array (string)
%         |- desc: the command display level as string (2)
%
%   Note: this structure maps/extends the Tango::CommandInfo structure. 
%   See the TANGO C++ API manual.
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
% (2) - Display level
% 
%     0 - Operator
%     1 - Expert
%
% Example:
% --------
%   %- get command information
%   cmd_info = tango_command_query('sys/database/dbds', 'DbGetServerInfo');
%   %- always check error
%   if (tango_error == -1)
%     %- handle error
%     tango_print_error_stack;
%     return;
%   end
%   %- cmd_info is valid - print only operator cmd
%   if (cmd_info.disp_level == 0) 
%     disp(cmd_info);
%   end
%
%   This code generates the following output:
%
%          cmd_name: 'DbGetServerInfo'
%           cmd_tag: 0
%           in_type: '1-by-n char'
%      in_type_desc: 'server name'
%          out_type: '1-by-n cell{1-by-n char}'
%     out_type_desc: 'server info'
%        disp_level: 0
%    disp_level_str: 'Operator'
%
% See also TANGO_COMMAND_LIST_QUERY.

% TANGO_COMMAND_QUERY cmd-id: 19
cmd_info = tango_binding(int16(19), dev_name, cmd_name);
return;
