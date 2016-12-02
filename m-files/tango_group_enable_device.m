function result = tango_group_enable_device (group_id, dev_name, forward)
%TANGO_GROUP_ID Enables a device in the group.
%
% Syntax:
% -------
%   result = tango_group_enable_device (group_id, dev_name, forward)
%
% Argin: group_id
% ---------------
%   |- type: 1-by-1 double array
%   |- desc: the group identifier
%
% Argin: dev_name
% ---------------
%   |- type: 1-by-n char array (i.e. String)
%   |- desc: the name of the device to be enabled.
%
% Argin: forward
% --------------
%   |- type: 1-by-1 numeric array (i.e. any numeric type)
%   |- desc: if set to non-null value (true), the function searches 
%            the whole hierarchy (specified group and its sub-groups). if 
%            set to 0, the search will be limited to the specified group.  
%
% Argout: result
% --------------
%   |- type: 1-by-1 double array
%   |- desc: always returns 0.
%
% Example:
% --------
%
% See also TANGO_GROUP_*

% TANGO_GROUP_CONTAINS cmd-id: 73
if nargin < 3
   forward = 0;
end
result = tango_binding(int16(73), group_id, dev_name, forward);
return;
