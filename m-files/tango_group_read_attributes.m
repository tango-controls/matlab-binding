function replies = tango_group_read_attributes (group_id, attr_names, forward)
%TANGO_GROUP_READ_ATTRBIUTE Read a several attributes on a TANGO group.
%
% Syntax:
% -------
%   replies = tango_group_read_attributes (group_id, attr_names, forward)
%
% Argin: group_id
% ---------------
%   |- type: 1-by-1 double array
%   |- desc: the group identifier
%
% Argin: attr_name s
% ----------------
%   |- type: type: 1-by-n cell array of 1-by-p char arrays (strings
%   |- desc: the name of the attributes to read (1)
%
% Argin: forward
% --------------
%   |- type: 1-by-1 numeric array (i.e. any numeric type)
%   |- desc: if set to non-null value (true) the command is forwarded to sub-groups.
%
% Argout: replies
% ---------------
%   |- type: 1-by-n struct array of 1-by-m struct array of 1-by-p struct array
%   |- desc: the replies (i.e. acknowledges)
%
%      replies
%      -------
%        |- has_failed: true (i.e 1) if at least one error occured, false otherwise (i.e 0).
%        |- dev_replies: per device replies - 1-by-m struct array with the following structure
%           -----------
%              |- dev_name: the device name
%              |- is_enabled: true (i.e 1) if enabled, false otherwise (i.e 0).
%              |- has_failed: true (i.e 1) if at least one error occured, false otherwise (i.e 0).
%              |- attr_values: attribute values - 1-by-p struct array with the following structure
%                 -----------
%                   |- is_enabled: true (i.e 1) if associated device is enabled, false otherwise (i.e 0).
%                   |- has_failed:  true (i.e 1) if an error occured, false otherwise (i.e 0).
%                   |- dev_name: associated device name.
%                   |- attr_name: attribute name.
%                   |- quality: attribute quality as a numeric value.
%                   |- quality_str: attribute quality as a string.
%                   |- m: the size of the y dimension (# of rows)
%                   |- n: the size of the x dimension (# of columns)
%                   |- time: time-stamp
%                   |- value: the actual value (set when has_failed == false, undefined otherwise)
%                   |- error: the error (set when has_failed == true, undefined otherwise)
%
% See also TANGO_GROUP_READ_ATTRIBUTES_ASYNCH, TANGO_GROUP_READ_ATTRIBUTES_REPLY

% Forward to the asynch version of <tango_group_read_attributes>
req_desc = tango_group_read_attributes_asynch(group_id, attr_names, forward);
if (tango_error == -1 || ~isstruct(req_desc))
   replies = -1;
   return;
end
replies = tango_group_read_attributes_reply(req_desc, 0);
return;

