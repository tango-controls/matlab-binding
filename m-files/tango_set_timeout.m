function tango_set_timeout (dev_name, tmo)
%TANGO_SET_TIMEOUT Set the client side timeout for the specified device.
%
% Any function (performed on this device) which takes longer than this 
% time to execute will generate an error.
%
% Syntax:
% -------
%   tango_set_timeout(dev_name, tmo)
%
% Argin: dev_name 
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the target device
%
% Argin: tmo
% ----------
%   |- type: 1-by-1 double array
%   |- desc: the client side timeout in milliseconds. 
%
% Example:
% --------
%   %- set device timeout
%   tango_set_timeout('tango/tangotest/1', 1500);
%   %- always check error
%   if (tango_error == -1)
%     %- handle error
%     tango_print_error_stack;
%     return;
%   end
%   
%   See also TANGO_GET_TIMEOUT

% TANGO_SET_TIMEOUT cmd-id: 20
tango_binding(int16(20), dev_name, tmo);
return