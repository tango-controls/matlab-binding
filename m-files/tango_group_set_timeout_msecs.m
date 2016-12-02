function all_devices_alive = tango_group_set_timeout_msecs (group_id, tmo_msecs)
%TANGO_GROUP_SET_TIMEOUT_MSECS Sets timeout (in msecs) on devices belonging to the specified group
%
% Syntax:
% -------
%   tango_group_set_timeout_msecs (group_id, tmo_msecs)
%
% Argin: group_id
% ---------------
%   |- type: 1-by-1 double array
%   |- desc: the group identifier
%
% Argin: tmo_msecs
% ----------------
%   |- type: 1-by-1 numeric array (i.e. any numeric type)
%   |- desc: the timeout in msecs  
%
% Argout: none
% ------------
%
% Example:
% --------
%
% See also TANGO_GROUP_*

% TANGO_GROUP_SET_TIMEOUT_MSECS cmd-id: 77
tango_binding(int16(77), group_id, int32(tmo_msecs));
return;
