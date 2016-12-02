function err = tango_open_device (dev_name)
%TANGO_OPEN_DEVICE Opens a connection to the specified device.
%   Returns -1 on error, 0 otherwise. In case of error use one
%   the TANGO error stack function to get error details.
%
%   See also TANGO_CLOSE_DEVICE.

% TANGO_OPEN_DEVICE cmd-id : 2
tango_binding(int16(2), dev_name);
err = tango_error;
return;