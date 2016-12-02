function info = tango_info (dev_name)
%TANGO_INFO Returns information on the specified device.  
%
% Syntax:
% -------
%   dev_info = tango_info(dev_name)
%
% Argin: dev_name 
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the target device
%
% Argout: dev_info
% ----------------
%   |- type: 1-by-1 struct array
%   |- desc: misc. device information 
%   |- fields: 
%       |
%       |-|- name: class
%       | |- type: 1-by-n char array (string)
%       | |- desc: the device class name
%       |
%       |-|- name: server
%       | |- type: 1-by-n char array (string)
%       | |- desc: the device server name (host process)
%       |
%       |-|- name: host
%       | |- type: 1-by-n char array (string)
%       | |- desc: the device server name (host process)
%       |
%       |-|- name: version
%       | |- type: 1-by-1 double array
%       | |- desc: the TANGO library "major version number" used by the device  
%       |
%       |-|- name: doc_url
%       | |- type: 1-by-n char array (string)
%       | |- desc: a WEB pointer to the device documentation
%       |
%       |-|- name: type
%         |- type: 1-by-1 double array
%         |- desc: the device type id
%
%   Note: this structure maps the Tango::DeviceInfo structure. 
%   See the TANGO C++ API manual.
%
% Example:
% --------
%   %- get device information
%   info = tango_info('tango/tangotest/3');
%   %- always check error
%   if (tango_error == -1)
%     %- handle error
%     tango_print_error_stack;
%     return;
%   end
%   %- info is valid, print it
%   disp(info);
%
%   This code generates the following output:
%
%     class:    'TangoTest'
%     server:   'TangoTest/3'
%     host:     'pc103inf.soleil.u-psud.fr'
%     version:  2
%     doc_url:  'http://www.esrf.fr/tango'
%     type:     0 
%
% See also TANGO_DESCRIPTION, TANGO_ADMIN_NAME, TANGO_IDL_VERSION.

% TANGO_INFO cmd-id: 11
info = tango_binding(int16(11), dev_name);
return;