function argout = tango_read_attribute_asynch (dev_name, attr_name)
%TANGO_READ_ATTRIBUTE_ASYNCH Reads asynchronously a single attribute on the specified device.
%
% Syntax:
% -------
%   req_desc = tango_read_attribute_asynch(dev_name, attr_name)
%
% Argin: dev_name 
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the target device
%
% Argin: attr_name
% ----------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the attribute to read (1)
%
% Argout: req_desc
% ----------------
%   |- type: 1-by-1 struct array
%   |- desc: the asynch. request descriptor (used to retrieve the answer) 
%   |- note: DO NOT MODIFY (OR ANSWER MAY BE LOST)
%
% Example:
% --------
%   %- read attribute
%   request_desc = tango_read_attribute_asynch('tango/tangotest/1', 'short_scalar');
%   %- always check error
%   if (tango_error == -1)
%     %- handle error 
%     tango_print_error_stack;
%     return;
%   end
%   %- do some work...
%   do_some_work();
%   %- ...then get response (with a timeout of 1000 ms)
%   info = tango_read_attribute_reply(request_desc, 1000);
%   %- always check error
%   if (tango_error == -1)
%     %- handle error
%     tango_print_error_stack;
%     return;
%   end
%   %- attr_val is a valid 1-by-1 int16 array
%   if (attr_val.quality ~= 1) 
%     %- print the value
%     sprintf('time: %s - short_scalar value: %.0f\r', datestr(attr_val.time), double(attr_val.value))  
%   end
%
%   See also TANGO_READ_ATTRIBUTE_REPLY, TANGO_READ_ATTRIBUTE

% TANGO_READ_ATTRIBUTE_ASYNCH cmd-id: 45
argout = tango_binding(int16(45), dev_name, attr_name);
return;
