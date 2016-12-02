function tango_write_attributes (dev_name, attr_name_val_list)
%TANGO_WRITE_ATTRIBUTES Writes several attributes on the specified device.
%
% Syntax:
% -------
%   tango_write_attributes(dev_name, attr_name_val_list)
%
% Argin: dev_name 
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the target device
%
% Argin: attr_name_val_list
% -------------------------
%   |- type: 1-by-n struct array
%   |- desc: an array containing [attribute name / value] pairs 
%   |- fields:
%        |  
%        |-|- name: name 
%        | |- type: 1-by-n char array (string)
%        | |- desc: the attribute name (1)
%        |
%        |-|- name: value
%          |- type: one of the TANGO supported types for attributes (2) 
%          |- desc: the value to write
%
%  (1) - Attribute name
%
%     Must be the name of 'writable' attributes.
%
%  (2) - Attribute data type
%     
%     for SCALAR attributes:
%       DEV_SHORT....1-by-1 int16 array 
%       DEV_LONG.....1-by-1 int32 array
%       DEV_DOUBLE...1-by-1 double array
%       DEV_STRING...1-by-n char array
%
%     for VECTOR attributes:
%       DEV_SHORT....1-by-n int16 array
%       DEV_LONG.....1-by-n int32 array
%       DEV_DOUBLE...1-by-n double array
%
%     for IMAGE attributes:
%       DEV_SHORT....m-by-n int16 array
%       DEV_LONG.....m-by-n int32 array
%       DEV_DOUBLE...m-by-n double array
%
% Example:
% --------
%   %- build attribute name/value list
%   attr_name_val_list(1).name  = 'short_scalar';
%   attr_name_val_list(1).value = int16(4096);
%   attr_name_val_list(2).name  = 'long_scalar';
%   attr_name_val_list(2).value = int32(654321);
%   attr_name_val_list(3).name  = 'double_scalar';
%   attr_name_val_list(3).value = -pi;
%   % write values
%   tango_write_attributes('tango/tangotest/1', attr_name_val_list);
%   if (tango_error == -1)
%     tango_print_error_stack;
%     return;
%   end
%
%   See also TANGO_WRITE_ATTRIBUTE, TANGO_READ_ATTRIBUTES, TANGO_READ_ATTRIBUTE

% TANGO_WRITE_ATTRIBUTES cmd-id :8
tango_binding(int16(8), dev_name, attr_name_val_list);
return;