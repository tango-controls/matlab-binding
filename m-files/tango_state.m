function state = tango_state (dev_name)
%TANGO_STATE Returns the current state of the specified device.
%
% Syntax:
% -------
%   state = tango_state (dev_name)
%
% Argin: dev_name 
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the target device
%
% Argout: state
% -------------
%    |- type: 1-by-1 struct array
%    |- desc: the device state 
%    |- fields: 
%        |
%        |- |- name: value
%        |  |- type: 1-by-1 double array
%        |  |- desc: the device state (as a numeric value)
%        |
%        |- |- name: name
%           |- type: 1-by-n char array (string)
%           |- desc: the device state (as a string)
%   
%   Note: this structure maps/extends the Tango::DevState enum. 
%   See the TANGO C++ API manual.
%
% Example:
% --------
%   %- get device state
%   state = tango_state('tango/tangotest/3');
%   %- always check error
%   if (tango_error == -1)
%     %- handle error
%     tango_print_error_stack;
%     return;
%   end
%   %- state is valid, print it
%   disp(state);
%
%   This code generates the following output:
%
%      value: 0
%       name: 'ON'
%
% See also TANGO_STATUS.

% TANGO_STATE cmd-id: 12
state = tango_binding(int16(12), dev_name);
return;