function tango_group_dump (group_id)
%TANGO_GROUP_DUMP Dump a TANGO group into the command window.
%
% Syntax:
% -------
%   tango_group_dump (group_id)
%
% Argin: group_id
% -----------------
%   |- type: 1-by-1 double array
%   |- desc: the group identifier

% TANGO_GROUP_DUMP cmd-id: 60
tango_binding(int16(60), group_id);
return;
