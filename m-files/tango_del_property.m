function tango_del_property (dev_name, prop_name)
%TANGO_DEL_PROPERTY Delete a single device property from the TANGO database.  
%
% Syntax:
% -------
%   tango_del_property(dev_name, prop_name)
%
% Argin: dev_name 
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the target device
%
% Argin: prop_name 
% ----------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the property to delete
%
% Example:
% --------
%   %- delete property from database
%   tango_del_property('tango/tangotest/3', 'dummy_1');
%   if (tango_error == -1)
%     tango_print_error_stack;
%     return;
%   end
%
% See also TANGO_DEL_PROPERTIES, TANGO_GET_PROPERTY, TANGO_PUT_PROPERTY.
 
% TANGO_DEL_PROPERTIES cmd-id: 42
tango_binding(int16(42), dev_name, prop_name);
return;