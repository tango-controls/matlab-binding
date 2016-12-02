function cmd_hist = tango_command_history (dev_name, cmd_name, hist_depth)
%TANGO_COMMAND_HISTORY Retrieve a command history from the polling buffer.
%
% See the TANGO control system manual for more information about 
% the polling mechanism.
%
% Syntax:
% -------
%   cmd_hist = tango_command_history(dev_name, cmd_name, hist_depth)
%
% Argin: dev_name 
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the target device
%
% Argin: cmd_name 
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the command name
%
% Argin: hist_depth 
% -----------------
%   |- type: 1-by-1 double array
%   |- desc: the number of "historical values" to return.
%
% Argout: cmd_hist
% ----------------
%   |- type: 1-by-n struct array (with n <= hist_depth)
%   |- desc: the <cmd_name> history.
%   |- fields: 
%        |
%        |- |- name: time
%        |  |- type: 1-by-1 double array
%        |  |- desc: the command timestamp (1)
%        |
%        |- |- name: value
%        |  |- type: one of the TANGO types (2)
%        |  |- desc: the command argout (i.e. command result)
%        |
%        |- |- name: has_failed
%        |  |- type: 1-by-1 double array (with logical flag set)
%        |  |- desc: true if the command gave error, false otherwise
%        |
%        |- |- name: errors
%           |- type: 1-by-n struct array (maps the Tango::DevFailed exception) 
%           |- desc: error details (3)
%           |- fields: 
%                |
%                |-|- name: reason
%                | |- type: 1-by-n char array (string)
%                | |- desc: the error type 
%                |
%                |-|- name: desc
%                | |- type: 1-by-n char array (string)
%                | |- desc: the detailed error description
%                |
%                |-|- name: origin
%                | |- type: 1-by-n char array (string)
%                | |- desc: the error source
%                |
%                |-|- name: severity
%                | |- type: 1-by-1 double array
%                | |- desc: the error severity as numeric value (4)
%                |
%                |-|- name: severity_str
%                  |- type: 1-by-n char array (string)
%                  |- desc: the error severity as string value (4)     
%
%  Note: this structure maps the Tango::DeviceDataHistoryList structure. 
%  See the TANGO C++ API manual. 
%
%  (1) - The command timestamp is a "Matlab serial date number". See 
%  the Matlab <datenum> function for more information about the serial 
%  date format. 
%
%  (2) - TANGO types <-> Matlab types mapping
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
%     DEVVAR_DOUBLESTRINGARRAY...1-by-1 struct[lvalue: 1-by-n double, svalue: 1-by-n cell{1-by-n char}]
%
%  (3) - Error details
%
%     The <errors> is undefined (i.e. NULL) if the <failed> is set to <false>. Always check <failed> 
%     value before trying to use the <errors> field (see example below).
%
%  (4) - Error severity (maps the Tango::ErrSeverity enumeration)
% 
%     0 - 'Warning'
%     1 - 'Error'
%     2 - 'Panic'
%
% Example:
% --------
%   %- get command history
%   cmd_hist = tango_command_history('tango/tangotest/1', 'Status', 5);
%   %- always check error
%   if (tango_error == -1)
%     %- handle error
%     tango_print_error_stack;
%     return;
%   end
%   %- cmd_hist is valid, print it
%   [m, n] = size(cmd_hist);
%   for i = 1:n
%     %- any error during command polling?
%     if (cmd_hist(i).has_failed)
%       %- an error occured, <errors> is defined
%       [m_err, n_err] = size(cmd_hist(i).errors);
%       for j = 1:n_err
%         disp(cmd_hist(i).errors(j));
%       end
%     else
%       %- no error (<errors> is undefined - DO NOT USE IT)
%       disp(cmd_hist(i).value);
%     end
%   end
%   
%  See also TANGO_COMMAND_INOUT, TANGO_ATTRIBUTE_HISTORY

% TANGO_COMMAND_HISTORY cmd-id: 24
cmd_hist = tango_binding(int16(24), dev_name, cmd_name, hist_depth);
return;
