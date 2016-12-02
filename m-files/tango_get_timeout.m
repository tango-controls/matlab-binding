function tmo = tango_get_timeout (dev_name)
%TANGO_GET_TIMEOUT Returns the client side timeout for the specified device.
%
% Syntax:
% -------
%   tmo = tango_get_timeout(dev_name)
%
% Argin: dev_name 
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the target device
%
% Argout: tmo
% -----------
%   |- type: 1-by-1 double array
%   |- desc: the client side timeout in milliseconds. 
%
% Example:
% --------
%   %- get device timeout
%   tmo = tango_get_timeout('tango/tangotest/1');
%   %- always check error
%   if (tango_error == -1)
%     %- handle error
%     tango_print_error_stack;
%     return;
%   end
%   %- tmo is valid, print it
%   disp(sprintf('timeout for %s is %.0f ms\r', dev_name, tmo)); 
%   
%   See also TANGO_SET_TIMEOUT

% TANGO_GET_TIMEOUT cmd-id: 21
tmo = tango_binding(int16(21), dev_name);
return;