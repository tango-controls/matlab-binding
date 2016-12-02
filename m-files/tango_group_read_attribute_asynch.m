function req_desc = tango_group_read_attribute_asynch (group_id, attr_name, forward)
%TANGO_GROUP_READ_ATTRBIUTE_ASYNCH Read asynchronously a single attribute on a TANGO group.
%
% Syntax:
% -------
%   req_desc = tango_group_read_attribute_asynch(group_id, attr_name, forward)
%
% Argin: group_id
% ---------------
%   |- type: 1-by-1 double array
%   |- desc: the group identifier
%
% Argin: attr_name
% ----------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the attribute to write (1)
%
% Argin: forward
% --------------
%   |- type: 1-by-1 numeric array (i.e. any numeric type)
%   |- desc: if set to non-null value (true) the command is forwarded to sub-groups.
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
% See also TANGO_GROUP_READ_ATTRIBUTE_REPLY

% tango_group_read_attribute_asynch cmd-id: 63
if nargin < 2
   forward = 0
end
req_desc = tango_binding(int16(63), group_id, attr_name, forward);
return;
