function admin_name = tango_admin_name (dev_name)
%TANGO_ADMIN_NAME Returns the name of the corresponding administrator device.
%
% Syntax:
% -------
%   admin_name = tango_admin_name(dev_name)
%
% Argin: dev_name 
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the target device
%
% Argout: admin_name
% ------------------
%   |- type: 1-by-n char array
%   |- desc: the name of the corresponding administrator device (i.e. the 
%            name of the device representing the process in which the target 
%            device is running). You can use this name to execute an 
%            administration command on the target device (e.g. to restart it).
%         
% Example:
% --------
%   %- get the administrator device name
%   admin_name = tango_admin_name('tango/tangotest/3');
%   %- always check error
%   if (tango_error == -1)
%     %- handle error here
%     tango_print_error_stack;
%     return;
%   end
%   %- admin_name is valid, print it
%   disp(admin_name);
%
%   This code generates the following output:
%
%    dserver/TangoTest/3
%

% TANGO_ADMIN_NAME cmd-id: 14
admin_name = tango_binding(int16(14), dev_name);
return;