function err_stack = tango_error_stack
%TANGO_ERROR_STACK Returns the TANGO error stack.
%
% Syntax:
% -------
%   err_stack = tango_error_stack
%
% Argout: err_stack
% -----------------
%   |- type: 1-by-n struct array (one structure per error)
%   |- desc: the TANGO error stack
%   |- fields: 
%       |
%       |-|- name: reason
%       | |- type: 1-by-n char array (string)
%       | |- desc: the error type 
%       |
%       |-|- name: desc
%       | |- type: 1-by-n char array (string)
%       | |- desc: the detailed error description
%       |
%       |-|- name: origin
%       | |- type: 1-by-n char array (string)
%       | |- desc: the error source
%       |
%       |-|- name: severity
%       | |- type: 1-by-1 double array
%       | |- desc: the error severity as numeric value (1)
%       |
%       |-|- name: severity_str
%         |- type: 1-by-n char array (string)
%         |- desc: the error severity as string value (1)     
%
% Note: this structure maps/extends the Tango::DevFailed exception. 
% See the TANGO C++ API manual.
%  
% (1) - Error severity (maps the Tango::ErrSeverity enumeration)
% 
%     0 - 'Warning'
%     1 - 'Error'
%     2 - 'Panic'
%
% Example:
% --------
%   %- get "unknown command" info
%   cmd_info = tango_command_query('tango/tangotest/3', 'dummy');
%   %- always check error
%   if (tango_error == -1)
%     %- handle error (cmd_info is not valid - DO NOT USE IT)
%     %- print error stack
%     tango_print_error_stack;
%     %- can't continue
%     return;
%   end
%   %- cmd_info is valid
%   disp(cmd_info);
%
%   This code generates the following output:
%
%   ************************************************************
%   *                    TANGO ERROR STACK                     *
%   ************************************************************
%   - ERROR 1
%   	|-reason.....API_CommandNotFound
%   	|-desc.......Command dummy not found
%   	|-origin.....Device_2Impl::command_query_2
%   	|-severity...Error (1)
%   - ERROR 2
%   	|-reason.....command_query failed
%   	|-desc.......failed to execute command_query on device tango/tangotest/3
%   	|-origin.....TangoBinding::command_query
%   	|-severity...Error (1)
%   ************************************************************
%   
%   See also TANGO_PRINT_ERROR_STACK, TANGO_ERROR

% TANGO_ERROR_STACK cmd-id: 1
err_stack = tango_binding(int16(1));
return;