function desc = tango_description (dev_name)
%TANGO_DESCRIPTION Returns the description of the specified device.
%
% Syntax:
% -------
%   dev_desc = tango_description(dev_name)
%
% Argin: dev_name 
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the target device
%
% Argout: dev_desc
% ------------------
%   |- type: 1-by-n char array
%   |- desc: the device description
%         
% Example:
% --------
%   %- get device description
%   dev_desc = tango_description('tango/tangotest/3');
%   %- always check error
%   if (tango_error == -1)
%     %- handle error here
%     tango_print_error_stack;
%     return;
%   end
%   %- dev_desc is valid, print it
%   disp(dev_desc);
%
%   This code generates the following output:
%
%    A TANGO test device
%
% See also TANGO_INFO           

% TANGO_DESCRIPTION cmd-id: 13
desc = tango_binding(int16(13), dev_name);
return;