function err = tango_error
%TANGO_ERROR Returns the current value of the TANGO error code.
%
% The TANGO error code is updated each time a TANGO binding 
% m-function is executed from Matlab. It is set to -1 in 
% case of error, 0 otherwise.
%
% Syntax:
% -------
%   err = tango_error
%
% Argout: err
% -----------
%   |- type: 1-by-n double array
%   |- desc: the TANGO error code 
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
%     |-reason.....API_CommandNotFound
%     |-desc.......Command dummy not found
%     |-origin.....Device_2Impl::command_query_2
%     |-severity...Error (1)
%   - ERROR 2
%     |-reason.....command_query failed
%     |-desc.......failed to execute command_query on device tango/tangotest/3
%     |-origin.....TangoBinding::command_query
%     |-severity...Error (1)
%   ************************************************************
%   
% See also TANGO_ERROR_STACK, TANGO_PRINT_ERROR_STACK     

% TANGO_ERROR cmd-id: 0
err = tango_binding(int16(0));
return;
