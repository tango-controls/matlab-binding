function data_src = tango_get_source(dev_name)
%TANGO_GET_SOURCE Returns the data source for the specified device.
%
% Syntax:
% -------
%   data_src = tango_get_source(dev_name)
%
% Argin: dev_name 
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the target device
%
% Argout: data_src
% ----------------
%   |- type: 1-by-1 struct array
%   |- desc: the device data source.
%   |- fields: 
%        |
%        |- |- name: value
%        |  |- type: 1-by-1 double array
%        |  |- desc: the device data source (as a numeric value)
%        |
%        |- |- name: name
%           |- type: 1-by-n char array (string)
%           |- desc: the device data source (as a string)
%   
%   Note: this structure maps/extends the Tango::DevState enum. 
%   See the TANGO C++ API manual. 
%
% Example:
% --------
%   %- get device data src
%   data_src = tango_get_timeout('tango/tangotest/1');
%   %- always check error
%   if (tango_error == -1)
%     %- handle error
%     tango_print_error_stack;
%     return;
%   end
%   %- data_src is valid, print it
%   disp(data_src); 
%   
%   This code generates the following output:
%
%      value: 2
%       name: 'Cache-Device'
%
%   See also TANGO_SET_SOURCE

% TANGO_GET_SOURCE cmd-id: 23
data_src = tango_binding(int16(23), dev_name);
return;