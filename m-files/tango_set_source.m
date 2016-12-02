function tango_set_source (dev_name, data_src)
%TANGO_SET_SOURCE Set the data source for the specified device.
%
% This function set the data source (device, polling buffer or
% polling buffer then device) for <tango_command_inout> and 
% <tango_read_attribute(s)>. See the Advanced Features chapter
% of the TANGO constrol system manual for all details regarding
% the polling.
%
% Syntax:
% -------
%   tango_set_source(dev_name, data_src)
%
% Argin: dev_name 
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the target device
%
% Argin: data_src
% ---------------
%   |- type: 1-by-1 double array OR 1-by-n char array (string) (1)
%   |- desc: the device data source. 
%
%  (1): <data_src> can be a double numeric value or a string. The
%  following values are allowed: 
%  
%     0 : 'Device'
%     1 : 'Cache'
%     2 : 'Cache-Device' (the default)
%
%  Note: these values maps the Tango::DevSource enumeration. 
%  See the TANGO C++ API manual. 
%
% Example:
% --------
%   %- set device data source
%   tango_set_source('tango/tangotest/1', 'Cache');
%   %- always check error
%   if (tango_error == -1)
%     %- handle error
%     tango_print_error_stack;
%     return;
%   end
%
%   which is equivalent to:
%
%   %- set device data source
%   tango_set_source('tango/tangotest/1', 1);
%   %- always check error
%   if (tango_error == -1)
%     %- handle error
%     tango_print_error_stack;
%     return;
%   end
%   
%   See also TANGO_GET_SOURCE

% TANGO_SET_SOURCE cmd-id: 22
tango_binding(int16(22), dev_name, data_src);
return;