function status = tango_status (dev_name)
%TANGO_STATUS Returns the current status of the specified device.
%
% Syntax:
% -------
%   status = tango_status(dev_name)
%
% Argin: dev_name 
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the target device
%
% Argout: status
% --------------
%   |- type: 1-by-n char array (string)
%   |- desc: the device status 
%
%   Note: if <dev_name> is the name of an administration 
%   device (i.e. a device of the DServer class), <status> is 
%   a multi-line string containing both the device and the 
%   polling status.
%
% Example:
% --------
%   %- get device status
%   status = tango_status('dserver/tangotest/1');
%   %- always check error
%   if (tango_error == -1)
%     %- handle error
%     tango_print_error_stack;
%     return;
%   end
%   %- status is valid, print it
%   disp(status);
%
%   This code generates the following output:
%
%      The device is ON
%      The polling is ON 
%
% See also TANGO_STATE

% TANGO_STATUS cmd-id: 9
status = tango_binding(int16(9), dev_name);
return;