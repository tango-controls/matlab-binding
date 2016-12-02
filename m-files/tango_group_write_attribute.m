function replies = tango_group_write_attribute (group_id, attr_name, attr_value, forward)
%TANGO_GROUP_WRITE_ATTRBIUTE Writes a single attribute on a TANGO group.
%
% Syntax:
% -------
%   replies = tango_group_write_attribute (group_id, attr_name, forward, attr_value)
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
% Argin: attr_value
% -----------------
%   |- type: one of the TANGO supported types for attributes (2) 
%   |- desc: the value to write
%
% Argout: replies
% ---------------
%   |- type: 1-by-n struct array
%   |- desc: the replies (i.e. acknowledges)
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
% See also TANGO_GROUP_WRITE_ATTRIBUTE_ASYNCH

% Forward to the asynch version of <tango_group_write_attribute>
if nargin < 3
   error('At least 3 arguments required');
end
if nargin < 4
   forward = 0
end

req_desc = tango_group_write_attribute_asynch(group_id, attr_name, forward, attr_value);
if (tango_error == -1 || ~isstruct(req_desc))
   replies = -1;
   return;
end
replies = tango_group_write_attribute_reply(req_desc, 0);
return;

