function replies = tango_group_read_attribute_reply (request_desc, timeout)
%TANGO_GROUP_RETURN_ATTRIBUTE_REPLY Returns the result of a pending attribute reading
%
% Syntax:
% -------
%     replies = tango_group_read_attribute_reply (request_desc, timeout)
%
% Argin: request_desc 
% -------------------
%   |- type: 1-by-1 struct array
%   |- desc: the asynch. request descriptor (previously returned by tango_group_write_attribute__asynch)
%   |- note: DO NOT MODIFY (OR ANSWER MAY BE LOST)
%
% Argin: timeout
% --------------
%   |- type: 1-by-1 numeric array (i.e. any numeric type)
%   |- desc: If after <timeout> milliseconds the reply is still not available then generate an error. 
%            If set to 0, wait indefinitly.
%
% Argout: replies
% ---------------
%   |- type: 1-by-n struct array
%   |- desc: the replies
%
% See also TANGO_GROUP_READ_ATTRIBUTE_ASYNCH
%

% TANGO_COMMAND_INOUT_REPLY cmd-id: 64
try
  replies = tango_binding(int16(64), request_desc, timeout);
catch
  %ignore <argout not set> warning if cmd does not return any value
  %just set it to 0 (i.e. no error)
  replies = 0
end
return;
