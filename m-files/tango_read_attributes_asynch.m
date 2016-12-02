function attr_val_list = tango_read_attributes_asynch (dev_name, attr_name_list)
%TANGO_READ_ATTRIBUTES_ASYNCH Reads asynchronously several attributes on the specified device.
%
% Syntax:
% -------
%   req_desc = tango_read_attributes_asynch(dev_name, attr_name)
%
% Argin: dev_name 
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the target device
%
% Argin: attr_name_list
% ---------------------
%   |- type: 1-by-n cell array {1-by-n char array}
%   |- desc: the name of the attributes to read (1)
%
% Argout: req_desc
% ----------------
%   |- type: 1-by-1 struct array
%   |- desc: the asynch. request descriptor (used to retrieve the answer) 
%   |- note: DO NOT MODIFY (OR ANSWER MAY BE LOST)
%
% Example:
% --------
%   %- build attribute list
%   attr_name_list = {'short_scalar', 'double_image'};
%   %- read attributes 
%   req_desc = tango_read_attributes_asynch('tango/tangotest/1', attr_name_list);
%   %- always check error
%   if (tango_error == -1)
%     %- handle error 
%     tango_print_error_stack;
%     return;
%   end
%   %- do some work...
%   do_some_work();
%   %- ...then get response (with a timeout of 1000 ms)
%   attr_val_list = tango_read_attributes_reply(req_desc, 1000);
%   %- always check error
%   if (tango_error == -1)
%     %- handle error 
%     tango_print_error_stack;
%     return;
%   end
%   %- store 'short_scalar' value in local variable
%   my_int16 = attr_val_list(1).value;
%   %- store 'double_image' value in local variable
%   my_image = attr_val_list(2).value;
%
%   See also TANGO_READ_ATTRIBUTES_REPLY, TANGO_READ_ATTRIBUTE_ASYNCH

% TANGO_READ_ATTRIBUTES_ASYNCH cmd-id: 47
attr_val_list = tango_binding(int16(47), dev_name, attr_name_list);
return;