function  idl_ver = tango_idl_version (dev_name)
%TANGO_IDL_VERSION Returns the TANGO IDL version implemented by the specified device.
%
% Syntax:
% -------
%   idl_ver = tango_idl_version(dev_name)
%
% Argin: dev_name 
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the target device
%
% Argout: idl_ver
% ---------------
%   |- type: 1-by-1 double array
%   |- desc: the TANGO IDL version implemented by the target device
%
% Example:
% --------
%   %- get TANGO IDL version
%   idl_ver = tango_idl_version('tango/tangotest/1');
%   %- always check error
%   if (tango_error == -1)
%     %- handle error
%     tango_print_error_stack;
%     return;
%   end
%   %- idl_ver is valid, print it
%   disp(sprintf('Device %s implements TANGO IDL version %.0f\r', dev_name, idl_ver)); 
%   
%   See also TANGO_INFO

% TANGO_IDL_VERSION cmd-id: 36
idl_ver = tango_binding(int16(36), dev_name);
return;