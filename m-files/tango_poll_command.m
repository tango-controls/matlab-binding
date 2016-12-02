function tango_poll_command(dev_name, cmd_name, polling_period)
%TANGO_POLL_COMMAND Add a command to the list of polled commands.
%
% See the TANGO control system manual for more information about 
% the polling mechanism.
%
% Syntax:
% -------
%   tango_poll_command(dev_name, cmd_name, polling_period)
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
% Argin: polling_period
% ---------------------
%   |- type: 1-by-1 double array
%   |- desc: the command polling period in ms
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
% See also TANGO_POLL_ATTRIBUTE

% TANGO_POLL_COMMAND cmd-id: 30
tango_binding(int16(30), dev_name, cmd_name, polling_period);
return;