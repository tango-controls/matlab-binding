function cmd_polled = tango_is_command_polled(dev_name, cmd_name)
%TANGO_IS_COMMAND_POLLED Returns true if the specified command is polled, false otherwise.
%
%   See the TANGO control system manual for more information about 
%   the polling mechanism.
%
% Syntax:
% -------
%   cmd_polled = tango_is_command_polled(dev_name, cmd_name)
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
% Argout: cmd_polled
% ------------------
%   |- type: 1-by-1 double array (with logical flag set)
%   |- desc: the command polling status
%
% Example:
% --------
%   % poll command
%   tango_poll_command('tango/tangotest/1', 'DevVoid', 500);
%   % always check error
%   if (tango_error == -1)
%     tango_print_error_stack;
%     return;
%   end
%   % is command polled?
%   polled = tango_is_command_polled('tango/tangotest/1', 'DevVoid');
%   % always check error
%   if (tango_error == -1)
%     tango_print_error_stack;
%     return;
%   end
%   if (polled ~= 1)
%     disp('ERROR. Cmd DevVoid should be polled');
%     return;
%   end
%
% See also TANGO_POLL_COMMAND, TANGO_IS_ATTRIBUTE_POLLED

% TANGO_IS_COMMAND_POLLED cmd-id: 28
cmd_polled = tango_binding(int16(28), dev_name, cmd_name);
return;