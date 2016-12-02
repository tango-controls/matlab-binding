function tango_put_properties (dev_name, prop_name_value_list)
%TANGO_PUT_PROPERTIES Add (or update) several device properties into the TANGO database.  
%
% Syntax:
% -------
%   tango_put_properties(dev_name, prop_name_value_list)
%
% Argin: dev_name 
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the target device
%
% Argin: prop_name_value_list
% ---------------------------
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
%          |- desc: the property (string) values
%
% Example:
% --------
%   prop.name = 'dummy_1';
%   prop.value = {'dummy_1_value_1', 'dummy_1_value_2', 'dummy_1_value_3'};
%   prop(2).name = 'dummy_2';
%   prop(2).value = {'dummy_2_value_1', 'dummy_2_value_2'};
%   prop(3).name = 'dummy_3';
%   prop(3).value = {'dummy_3_value_1'};
%   %- put properties into the database
%   tango_put_property('tango/tangotest/3', prop);
%   if (tango_error == -1)
%     tango_print_error_stack;
%     return;
%   end
%
% See also TANGO_PUT_PROPERTY, TANGO_GET_PROPERTIES, TANGO_DEL_PROPERTIES.

% TANGO_PUT_PROPERTIES cmd-id: 38
tango_binding(int16(38), dev_name, prop_name_value_list);
return;