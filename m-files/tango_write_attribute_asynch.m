function req_desc = tango_write_attribute_asynch (dev_name, attr_name, attr_value)
%TANGO_WRITE_ATTRIBUTE_ASYNCH Writes asynchronously a single attribute on the specified device.
%
% Syntax:
% -------
%   req_desc = tango_write_attribute_asynch(dev_name, attr_name, attr_value)
%
% Argin: dev_name 
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the target device
%
% Argin: attr_name
% ----------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the attribute to write (1)
%
% Argin: attr_value
% -----------------
%   |- type: one of the TANGO supported types for attributes (2) 
%   |- desc: the value to write
%
% Argout: req_desc
% ----------------
%   |- type: 1-by-1 struct array
%   |- desc: the asynch. request descriptor (used to retrieve the answer) 
%   |- note: DO NOT MODIFY (OR ANSWER MAY BE LOST)
%
%  (1) - Attribute name
%
%     Must be the name of a 'writable' attribute.
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
% %- write the attribute
% req_desc = tango_write_attribute_asynch('tango/tangotest/3', 'long_scalar', int32(123456));
% if (tango_error == -1)
%   tango_print_error_stack;
%   return;
% end
% %- do some work ...
% do_some_work();
% %- ... then get request acknowledge (timeout = 100 ms)
% tango_write_attribute_reply(req_desc, 100);
% if (tango_error == -1)
%   tango_print_error_stack;
%   return;
% end
%
% See also TANGO_WRITE_ATTRIBUTE_REPLY, TANGO_WRITE_ATTRIBUTES_ASYNCH, TANGO_WRITE_ATTRIBUTE

% TANGO_WRITE_ATTRIBUTE_ASYNCH cmd-id: 49
req_desc = tango_binding(int16(49), dev_name, attr_name, attr_value);
return;