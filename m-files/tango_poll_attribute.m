function tango_poll_attribute(dev_name, attr_name, polling_period)
%TANGO_POLL_ATTRIBUTE Add an attribute to the list of polled attributes.
%
% See the TANGO control system manual for more information about 
% the polling mechanism.
%
% Syntax:
% -------
%   tango_poll_attribute(dev_name, attr_name, polling_period)
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
% Argin: polling_period
% ---------------------
%   |- type: 1-by-1 double array
%   |- desc: the attribute polling period in ms
%
% Example:
% --------
%   % poll attribute
%   tango_poll_attribute('tango/tangotest/1', 'short_scalar', 250);
%   % always check error
%   if (tango_error == -1)
%     tango_print_error_stack;
%     return;
%   end
%   % is attribute polled?
%   polled = tango_is_attribute_polled('tango/tangotest/1', 'short_scalar');
%   % always check error
%   if (tango_error == -1)
%     tango_print_error_stack;
%     return;
%   end
%   if (polled ~= 1)
%     disp('ERROR. Attribute short_scalar should be polled');
%     return;
%   end
%
% See also TANGO_POLL_COMMAND

% TANGO_POLL_ATTRIBUTE cmd-id: 31
tango_binding(int16(31), dev_name, attr_name, polling_period);
return;
