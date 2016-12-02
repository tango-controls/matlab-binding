function tango_group_kill (group_id)
%TANGO_GROUP_KILL Kill/Release a TANGO group and its sub-groups.
%
% Syntax:
% -------
%   tango_group_kill (group_id)
%
% Argin: group_id
% -----------------
%   |- type: 1-by-1 double array
%   |- desc: the group identifier
%
% Example:
% --------
%
% See also TANGO_GROUP_CREATE

% TANGO_GROUP_KILL cmd-id: 54
tango_binding(int16(54), group_id);
return;
