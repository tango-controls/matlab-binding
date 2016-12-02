function attr_val = tango_read_attribute (dev_name, attr_name)
%TANGO_READ_ATTRIBUTE Read a single attribute on the specified device.
%
% Syntax:
% -------
%   attr_value = tango_read_attribute(dev_name, attr_name)
%
% Argin: dev_name
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the target device
%
% Argin: attr_name
% ----------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the attribute to read (1)
%
% Argout: attr_value
% ------------------
%   |- type: 1-by-1 struct array
%   |- desc: the attribute value
%   |- fields:
%        |  
%        |-|- name: name
%        | |- type: 1-by-n char array (string)
%        | |- desc: the attribute name 
%        |
%        |-|- name: quality
%        | |- type: 1-by-1 double array
%        | |- desc: the quality of this attribute value (numeric form) (2)
%        |
%        |-|- name: quality_str
%        | |- type: 1-by-n char array (string)
%        | |- desc: the quality of this attribute value (text form) (2)
%        |
%        |-|- name: m
%        | |- type: 1-by-1 double array
%        | |- desc: the size of the y dimension (# of rows) (3)
%        |
%        |-|- name: n
%        | |- type: 1-by-1 double array
%        | |- desc: the size of the x dimension (# of columns) (3)
%        |
%        |-|- name: time
%        | |- type: 1-by-1 double array
%        | |- desc: time-stamp (4)
%        |
%        |-|- name: value
%          |- type: one of the TANGO supported types for attributes (5)
%          |- desc: the actual value
%
%  Note: this structure maps the Tango::DeviceAttribute structure.
%  See the TANGO control system manual. 
%
%  (1) - Attribute name
%
%     It must be the name of 'readable' attribute (i.e. the
%     name of a READ, READ-WRITE or READ-WITH-WRITE attribute)
%
%  (2) - Quality
%            
%     0: 'VALID'
%     1: 'INVALID'
%     2: 'ALARM'
%     3: 'CHANGING'
%     4: 'WARNING'
%
%  (3) - Dimensions
%        
%     m is always set to 0 for SCALAR and SPECTRUM attribute     
%
%  (4) - Time-stamp
%
%     The timestamp is a "Matlab serial date number". See the Matlab 
%     <datenum> and <datestr> functions for more information about 
%     the Matlab serial date/time format. 
%
%  (5) - Attribute data type
%     
%     for SCALAR attributes:
%       DEV_SHORT....1-by-1 int16 array 
%       DEV_LONG.....1-by-1 int32 array
%       DEV_DOUBLE...1-by-1 double array
%       DEV_STRING...1-by-n char array
%
%     for VECTOR attributes:
%       DEV_SHORT....1-by-n int16 array
%       DEV_LONG.....1-by-n int32 array
%       DEV_DOUBLE...1-by-n double array
%
%     for IMAGE attributes:
%       DEV_SHORT....m-by-n int16 array
%       DEV_LONG.....m-by-n int32 array
%       DEV_DOUBLE...m-by-n double array
%
% Example:
% --------
%   %- read attribute
%   attr_val = tango_read_attribute('tango/tangotest/1', 'short_scalar');
%   %- always check error
%   if (tango_error == -1)
%     %- handle error 
%     tango_print_error_stack;
%     return;
%   end
%   %- attr_val is a valid 1-by-1 int16 array
%   if (attr_val.quality ~= 1) 
%     %- print the value
%     sprintf('time: %s - short_scalar value: %.0f\r', datestr(attr_val.time), double(attr_val.value))  
%   end
%
%   See also TANGO_READ_ATTRIBUTES, TANGO_WRITE_ATTRIBUTE, TANGO_WRITE_ATTRIBUTES

% TANGO_READ_ATTRIBUTE cmd-id: 5
attr_val = tango_binding(int16(5), dev_name, attr_name);
return;
