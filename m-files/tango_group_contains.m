function result = tango_group_contains (group_id, pattern, forward)
%TANGO_GROUP_ID Returns the id of the group named <group_name>.
%
% Syntax:
% -------
%   result = tango_group_contains (group_id, dev_name_pattern, forward)
%
% Argin: group_id
% ---------------
%   |- type: 1-by-1 double array
%   |- desc: the group identifier
%
% Argin: dev_name_pattern
% -----------------------
%   |- type: 1-by-n char array (i.e. String)
%   |- desc: the pattern to match (e.g. 'tango/sys/*'). can (obviously) be 
%            the full name of a single device.  
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
%   |- desc: 1 (i.e true) means "at least one device which name matches 
%            <pattern> could be found in the group". 0 (i.e false) means 
%            "no device by that name".
%
% Example:
% --------
%
% See also TANGO_GROUP_*

% TANGO_GROUP_CONTAINS cmd-id: 70
if nargin < 3
   forward = 0;
end
result = tango_binding(int16(70), group_id, pattern, forward);
return;
