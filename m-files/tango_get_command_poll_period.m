function  polling_period = tango_get_command_poll_period (dev_name, cmd_name)
%TANGO_GET_COMMAND_POLL_PERIOD Returns a command polling period in ms.  
%
% See the TANGO control system manual for more information about 
% the polling mechanism.
%
% Syntax:
% -------
%   polling_period = tango_get_command_poll_period(dev_name, cmd_name)
%
% Argin: dev_name 
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the target device
%
% Argin: cmd_name 
% ----------------
%   |- type: 1-by-n char array (string)
%   |- desc: the command name
%
% Argout: polling_period
% ----------------------
%   |- type: 1-by-1 double array 
%   |- desc: the <cmd_name> polling period in ms
%
% Example:
% --------
%   dev_name = 'tango/tangotest/1';
%   cmd_name = 'DevVoid';
%   pp = tango_get_command_poll_period(dev_name, cmd_name);
%   if (tango_error == -1)
%     tango_print_error_stack;
%     return;
%   end
%   disp(sprintf('%s::%s polling period is %.2fms\n', dev_name, cmd_name, pp));
%   
% See also TANGO_POLL_COMMAND, TANGO_GET_ATTRIBUTE_POLL_PERIOD

% TANGO_GET_COMMAND_POLL_PERIOD cmd-id: 34
polling_period = tango_binding(int16(34), dev_name, cmd_name);
return;