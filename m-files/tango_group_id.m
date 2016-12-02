function id = tango_group_id (group_name)
%TANGO_GROUP_ID Returns the id of the group named <group_name>.
%
% Syntax:
% -------
%   id = tango_group_id (group_name)
%
% Argin: group_name
% -----------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the group
%
% Argout: group_id
% ----------------
%   |- type: 1-by-1 double array
%   |- desc: the group id
%
% Example:
% --------
%
% See also TANGO_GROUP_*

% TANGO_GROUP_ID cmd-id: 57
id = tango_binding(int16(57), group_name);
return;
