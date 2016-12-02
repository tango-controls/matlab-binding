function attr_polled = tango_is_attribute_polled(dev_name, attr_name)
%TANGO_IS_ATTRIBUTE_POLLED Returns true if the specified attribute is polled, false otherwise.
%
%   See the TANGO control system manual for more information about 
%   the polling mechanism.
%
% Syntax:
% -------
%   attr_polled = tango_is_attribute_polled(dev_name, attr_name)
%
% Argin: dev_name 
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the target device
%
% Argin: attr_name
% ----------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the attribute 
%
% Argout: attr_polled
% -------------------
%   |- type: 1-by-1 double array (with logical flag set)
%   |- desc: the attribute polling status
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
% See also TANGO_POLL_ATTRIBUTE, TANGO_IS_COMMAND_POLLED

% TANGO_IS_ATTRIBUTE_POLLED cmd-id: 29
attr_polled = tango_binding(int16(29), dev_name, attr_name);
return;