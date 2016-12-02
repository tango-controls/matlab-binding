function tango_write_attribute_reply (request_desc, timeout)
%TANGO_WRITE_ATTRIBUTE_REPLY Checks acknowledge of previous call to TANGO_WRITE_ATTRIBUTE_ASYNCH.
%
% Syntax:
% -------
%   tango_write_attribute_reply(request_desc, timeout)
%
% Argin: request_desc 
% -------------------
%   |- type: 1-by-1 struct array
%   |- desc: the asynch. request descriptor (previously returned by tango_read_attribute_asynch)
%   |- note: DO NOT MODIFY (OR ANSWER MAY BE LOST)
%
% Argin: timeout 
% --------------
%   |- type: 1-by-1 numeric array (i.e. any numeric type)
%   |- desc: If after <timeout> milliseconds the reply is still not available then generate an error. 
%            If set to 0, wait indefinitly. 
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
% See also TANGO_WRITE_ATTRIBUTE_ASYNCH, TANGO_WRITE_ATTRIBUTES_ASYNCH, TANGO_WRITE_ATTRIBUTES_REPLY

% TANGO_WRITE_ATTRIBUTE_ASYNCH cmd-id: 50
tango_binding(int16(50), request_desc, timeout);
return;