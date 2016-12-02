function req_desc = tango_group_read_attributes_asynch (group_id, attr_names, forward)
%TANGO_GROUP_READ_ATTRBIUTE_ASYNCH Read asynchronously several attribute on a TANGO group.
%
% Syntax:
% -------
%   req_desc = tango_group_read_attributes_asynch(group_id, attr_names, forward)
%
% Argin: group_id
% ---------------
%   |- type: 1-by-1 double array
%   |- desc: the group identifier
%
% Argin: attr_names
% -----------------
%   |- type: 1-by-n cell array of 1-by-p char arrays (strings)
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
%     Must be the name of a 'readable' attribute.
%
% See also TANGO_GROUP_READ_ATTRIBUTES_REPLY

% tango_group_read_attributes_asynch cmd-id: 71
req_desc = tango_binding(int16(71), group_id, attr_names, forward);
return;
