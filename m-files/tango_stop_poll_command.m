function tango_stop_poll_command (dev_name, cmd_name)
%TANGO_STOP_POLL_COMMAND Remove a command from the list of polled commands.
%
% See the TANGO control system manual for more information about 
% the polling mechanism.
%
% Syntax:
% -------
%   tango_stop_poll_command(dev_name, cmd_name)
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
% Example:
% --------
%   % stop polling command
%   tango_stop_poll_command('tango/tangotest/1', 'DevVoid');
%   % always check error
%   if (tango_error == -1)
%     tango_print_error_stack;
%     return;
%   end
%
% See also TANGO_POLL_COMMAND, TANGO_STOP_POLL_ATTRIBUTE

% TANGO_STOP_POLL_COMMAND cmd-id: 32
tango_binding(int16(32), dev_name, cmd_name);
return;