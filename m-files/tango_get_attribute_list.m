function attr_list = tango_get_attribute_list (dev_name)
%TANGO_GET_ATTRIBUTE_LIST Returns the name of all the attributes implemented by the specified device. 
%
% Syntax:
% -------
%   attr_list = tango_get_attribute_list(dev_name)
%
% Argin: dev_name 
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the target device
%
% Argout: attr_list
% -----------------
%   |- type: 1-by-n cell array {1-by-n char array}
%   |- desc: the attribute names
%
% Example:
% --------
%   %- get attributes names
%   attr_list = tango_get_attribute_list('dev');
%   %- always check error
%   if (tango_error == -1)
%     %- handle error
%     tango_print_error_stack;
%     return;
%   end
%   %- attr_list is valid
%   [m, n] = size(attr_list)
%   for i = 1:n
%     disp(attr_list(i));
%   end
% 
%   See also TANGO_ATTRIBUTE_LIST_QUERY, TANGO_COMMAND_LIST_QUERY

% TANGO_GET_ATTRIBUTE_LIST cmd-id: 25
attr_list = tango_binding(int16(25), dev_name);
return;