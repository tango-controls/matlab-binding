function err = tango_close_device (dev_name)
%TANGO_OPEN_DEVICE Closes a previously opened connection to the specified device.
%   Use the wild card '*' to close all opened connections. Returns -1 on error, 
%   0 otherwise. In case of error use one the TANGO error stack function to get 
%   error details.
%
%   See also TANGO_OPEN_DEVICE.

% TANGO_CLOSE_DEVICE cmd-id : 3
tango_binding(int16(3), dev_name);
err = tango_error;
return;