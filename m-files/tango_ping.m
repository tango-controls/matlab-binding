function result = tango_ping (dev_name)
%TANGO_PING Sends a ping to the specified device.
%
% This function returns the time elapsed in microseconds 
% (round trip time). Use <tango_ping> to verify that a 
% device is up and ready. 
%
% Syntax:
% -------
%   elapsed_usec = tango_ping(dev_name)
%
% Argin: dev_name 
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the target device
%
% Argout: elapsed_usec
% --------------------
%   |- type: 1-by-1 double array
%   |- desc: the round trip time in microseconds. 
%
% Example:
% --------
%   %- ping the device
%   elapsed_usec = tango_ping('tango/tangotest/1');
%   %- always check error
%   if (tango_error == -1)
%     %- handle error
%     tango_print_error_stack;
%     return;
%   end
%   %- elapsed_usec is valid, print it
%   disp(sprintf('%s is up and ready [RRT: %.2f usec]\r', dev_name, elapsed_usec));
%

% TANGO_PING cmd-id: 10
result = tango_binding(int16(10), dev_name);
return;