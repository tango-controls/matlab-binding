function tango_group_remove (parent_group_id, group_id_or_name_pattern, forward)
%TANGO_GROUP_REMOVE Remove object(s) (group or devices) from an existing TANGO group.
%
% Syntax:
% -------
%   tango_group_remove (parent_group_id, group_id_or_name_pattern)
%
% Argin: parent_group_id
% ----------------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the group parent
%
% Argin: group_id_or_name_pattern
% -------------------------------
%   |- type: 1-by-1 double array
%   |    or  1-by-n char array (string)
%   |    or  1-by-n cell array of 1-by-n char array (string)
%   |- desc: group id or device name patterns
%
% Argin: forward
% --------------
%   |- type: 1-by-1 double array (used as boolean)
%   |- desc: forward request to sub-groups
%
% Example:
% --------
%
% See also TANGO_GROUP_ADD

% TANGO_GROUP_REMOVE cmd-id: 56
if nargin < 2
   forward = 0;
end
tango_binding(int16(56), parent_group_id, group_id_or_name_pattern, forward);
return;
