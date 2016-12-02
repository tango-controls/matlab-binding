function tango_del_properties (dev_name, prop_name_list)
%TANGO_DEL_PROPERTIES Delete several device properties from the TANGO database.  
%
% Syntax:
% -------
%   tango_del_properties(dev_name, prop_name_list)
%
% Argin: dev_name 
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the target device
%
% Argin: prop_name_list
% ---------------------
%   |- type: 1-by-n cell array {1-by-n char array}
%   |- desc: the name of the properties to delete
%
% Example:
% --------
%   %- delete 3 properties from database
%   tango_del_properties('tango/tangotest/3', {'dummy_1', 'dummy_2', 'dummy_3'});
%   if (tango_error == -1)
%     tango_print_error_stack;
%     return;
%   end
%
% See also TANGO_DEL_PROPERTY, TANGO_GET_PROPERTIES, TANGO_PUT_PROPERTIES.

% TANGO_DEL_PROPERTIES cmd-id: 39
tango_binding(int16(39), dev_name, prop_name_list);
return;