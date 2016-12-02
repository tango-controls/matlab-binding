function group_size = tango_group_size (group_id, forward)
%TANGO_GROUP_ID Returns the id of the group named <group_name>.
%
% Syntax:
% -------
%   grp_size = tango_group_id (group_id)
%
% Argin: group_id
% ---------------
%   |- type: 1-by-1 double array
%   |- desc: the group identifier
%
% Argin: forward
% --------------
%   |- type: 1-by-1 numeric array (i.e. any numeric type)
%   |- desc: if set to non-null value (true), the function returns the
%            number of devices in the whole hierarchy (specified group and
%            its sub-groups). if set to 0, the function only returns the 
%            number of devices in the specified group.  
%
% Argout: group_size
% ------------------
%   |- type: 1-by-1 double array
%   |- desc: the group id
%
% Example:
% --------
%
% See also TANGO_GROUP_*

% TANGO_GROUP_SIZE cmd-id: 68
if nargin < 2
   forward = 0;
end
group_size = tango_binding(int16(68), group_id, forward);
return;
