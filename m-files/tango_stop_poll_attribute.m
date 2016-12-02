function tango_stop_poll_attribute(dev_name, attr_name)
%TANGO_STOP_POLL_ATTRIBUTE Remove an attribute from the list of polled attributes.
%
% See the TANGO control system manual for more information about 
% the polling mechanism.
%
% Syntax:
% -------
%   tango_stop_poll_attribute(dev_name, attr_name)
%
% Argin: attr_name
% ----------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the target device
%
% Argin: cmd_name 
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the attribute name
%
% Example:
% --------
%   % stop polling attribute
%   tango_stop_poll_attribute('tango/tangotest/1', 'short_scalar');
%   % always check error
%   if (tango_error == -1)
%     tango_print_error_stack;
%     return;
%   end
%
% See also TANGO_POLL_COMMAND, TANGO_STOP_POLL_ATTRIBUTE

% TANGO_STOP_POLL_ATTRIBUTE cmd-id: 33
tango_binding(int16(33), dev_name, attr_name);
return;