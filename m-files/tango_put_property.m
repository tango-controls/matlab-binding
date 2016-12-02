function tango_put_property (dev_name, prop_name, prop_value)
%TANGO_PUT_PROPERTY Add (or update) a single device property into the TANGO database.  
%
% Syntax:
% -------
%   tango_put_property(dev_name, prop_name, prop_val)
%
% Argin: dev_name 
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the target device
%
% Argin: prop_name
% ----------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the property to read
%
% Argin: prop_value
% -----------------
%   |- type: 1-by-n cell array {1-by-n char array}
%   |- desc: the property (string) values
%
% Example:
% --------
%   %- put property into the database
%   tango_put_property('tango/tangotest/3', 'my_prop', {'str_val1', '125', 'str_val2', '3.14159'});
%   if (tango_error == -1)
%     tango_print_error_stack;
%     return;
%   end
%
% See also TANGO_PUT_PROPERTIES, TANGO_GET_PROPERTY, TANGO_DEL_PROPERTY.

% TANGO_PUT_PROPERTY cmd-id: 41
tango_binding(int16(41), dev_name, prop_name, prop_value);
return;