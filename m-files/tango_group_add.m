function tango_group_add (parent_group_id, group_id_or_name_pattern)
%TANGO_GROUP_ADD Add objects (group or devices) to an existing TANGO group.
%
% Syntax:
% -------
%   tango_group_add (parent_group_id, group_id_or_name_pattern)
%
% Argin: parent_group_id
% ----------------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the parent group
%
% Argin: group_id_or_name_pattern
% -------------------------------
%   |- type: 1-by-1 double array
%   |    or  1-by-n char array (string)
%   |    or  1-by-n cell array of 1-by-n char array
%   |- desc: group id or device name patterns
%
% Example:
% --------
%
% See also TANGO_GROUP_REMOVE

% TANGO_GROUP_ADD cmd-id: 55
tango_binding(int16(55), parent_group_id, group_id_or_name_pattern);
return;
