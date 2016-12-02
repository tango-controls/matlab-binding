function dev_list = tango_group_device_list (group_id, forward)
%TANGO_GROUP_DEVICE_LIST Returns of the list of devices belonging to the specified group
%
% Syntax:
% -------
%   dev_list = tango_group_device_list (group_id, forward)
%
% Argin: group_id
% ---------------
%   |- type: 1-by-1 double array
%   |- desc: the group identifier
%
% Argin: forward
% --------------
%   |- type: 1-by-1 numeric array (i.e. any numeric type)
%   |- desc: if set to non-null value (true), the function propagate 
%            the request to <sub groups>
%
% Argout: dev_list
% ----------------
%   |- type: 1-by-n cell array
%   |- desc: the device name list
%
% Example:
% --------
%
% See also TANGO_GROUP_*

% TANGO_GROUP_SET_TIMEOUT_MSECS cmd-id: 78
if nargin < 1
   error('At least 1 argument required');
end
if nargin < 2
   forward = 0;
end
dev_list = tango_binding(int16(78), group_id, forward);
return;
