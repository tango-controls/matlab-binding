function prop_val_list = tango_get_properties (dev_name, prop_name_list)
%TANGO_GET_PROPERTIES Returns the value of several device properties stored into the TANGO database.  
%
% Syntax:
% -------
%   tango_get_properties(dev_name, prop_name_list)
%
% Argin: dev_name 
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the target device
%
% Argin: prop_name_list
% ---------------------
%   |- type: 1-by-n cell array {1-by-n char array}
%   |- desc: the name of the properties to read
%
% Argout: prop_val_list
% ---------------------
%   |- type: 1-by-n struct array (one structure per property)
%   |- desc: the list of property value
%   |- fields:
%        |  
%        |-|- name: name 
%        | |- type: 1-by-n char array (string)
%        | |- desc: the property name 
%        |
%        |-|- name: value
%          |- type: 1-by-n cell array {1-by-n char array}
%          |- desc: the property (string) values (1)
%
%	(1) Since there is no way for a generic TANGO client (such as
%	this binding) to guess the actual type of the property value,
%	it is returned as strings stored into a cell array. It is the
%	user responsability to cast the cell content from string to
%	the actual type (see TANGO_GET_PROPERTY example).
%
% Example:
% --------
%   %- get 3 properties from database
%   prop_list = {'mthreaded_impl', 'sleep_period', 'polled_attr'};
%   prop_val_list = tango_get_properties('tango/tangotest/3', prop_list);
%   if (tango_error == -1)
%     tango_print_error_stack;
%     return;
%   end
%   [m, n] = size(prop_val_list);
%   for i = 1:n
%     disp(prop_val_list(i));
%   end
%
% See also TANGO_GET_PROPERTY, TANGO_PUT_PROPERTIES, TANGO_DEL_PROPERTIES.

% TANGO_GET_PROPERTIES cmd-id: 37
prop_val_list = tango_binding(int16(37), dev_name, prop_name_list);
return;
