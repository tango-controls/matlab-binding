function group_id = tango_group_create (group_name)
%TANGO_GROUP_CREATE Creates a new TANGO group.
%
% Syntax:
% -------
%   group_id = tango_group_create (group_name)
%
% Argin: group_name
% -----------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the group to create
%
% Argout: group_id
% ----------------
%   |- type: 1-by-1 double array
%   |- desc: the group id
%
% Example:
% --------
%
% See also TANGO_GROUP_KILL

% TANGO_GROUP_CREATE cmd-id: 53
group_id = tango_binding(int16(53), group_name);
return;
