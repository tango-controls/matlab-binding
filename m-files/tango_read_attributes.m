function attr_val_list = tango_read_attributes (dev_name, attr_name_list)
%TANGO_READ_ATTRIBUTES Read several attributes on the specified device.
%
% Syntax:
% -------
%   attr_val_list = tango_read_attribute(dev_name, attr_name)
%
% Argin: dev_name 
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the target device
%
% Argin: attr_name_list
% ---------------------
%   |- type: 1-by-n cell array {1-by-n char array}
%   |- desc: the name of the attributes to read (1)
%
% Argout: attr_val_list
% ---------------------
%   |- type: 1-by-n struct array (one structure per attribute value)
%   |- desc: the list of attribute value
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
%  (1) - Attribute names
%
%     The list must contains the name of 'readable' attributes (i.e. 
%     the name of some READ, READ-WRITE or READ-WITH-WRITE attributes)
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
%   %- build attribute list
%   attr_name_list = {'short_scalar', 'double_image'};
%   %- read attributes
%   attr_val_list = tango_read_attributes('tango/tangotest/1', attr_name_list);
%   %- always check error
%   if (tango_error == -1)
%     %- handle error 
%     tango_print_error_stack;
%     return;
%   end
%   %- store 'short_scalar' value in local variable
%   my_int16 = attr_val_list(1).value;
%   %- store 'double_image' value in local variable
%   my_image = attr_val_list(2).value;
%
%   See also TANGO_READ_ATTRIBUTE, TANGO_WRITE_ATTRIBUTES, TANGO_WRITE_ATTRIBUTE

% TANGO_READ_ATTRIBUTES cmd-id: 6
attr_val_list = tango_binding(int16(6), dev_name, attr_name_list);
return;
