function  polling_period = tango_get_attribute_poll_period (device_name, attr_name)
%TANGO_GET_ATTRIBUTE_POLL_PERIOD Returns an attribute polling period in ms.  
%
% See the TANGO control system manual for more information about 
% the polling mechanism.
%
% Syntax:
% -------
%   polling_period = tango_get_attribute_poll_period(dev_name, attr_name)
%
% Argin: dev_name 
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the target device
%
% Argin: attr_name 
% ----------------
%   |- type: 1-by-n char array (string)
%   |- desc: the attribute name
%
% Argout: polling_period
% ----------------------
%   |- type: 1-by-1 double array 
%   |- desc: the <attr_name> polling period in ms
%
% Example:
% --------
%   dev_name = 'tango/tangotest/1';
%   attr_name = 'short_scalar';
%   pp = tango_get_attribute_poll_period(dev_name, attr_name);
%   if (tango_error == -1)
%     tango_print_error_stack;
%     return;
%   end
%   disp(sprintf('%s->%s polling period is %.2fms\n', dev_name, attr_name, pp));
%   
% See also TANGO_POLL_ATTRIBUTE, TANGO_GET_COMMAND_POLL_PERIOD

% TANGO_GET_ATTRIBUTE_POLL_PERIOD cmd-id: 35
polling_period = tango_binding(int16(35), device_name, attr_name);
return;