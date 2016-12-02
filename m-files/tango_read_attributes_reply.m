function attr_val_list = tango_read_attributes_reply (request_desc, timeout)
%TANGO_READ_ATTRIBUTES_REPLY Returns the result of a previous call to TANGO_READ_ATTRIBUTES_ASYNCH.
%
% Syntax:
% -------
%   attr_val_list = tango_read_attribute(request_desc, timeout)
%
% Argin: request_desc 
% -------------------
%   |- type: 1-by-1 struct array
%   |- desc: the asynch. request descriptor (previously returned by tango_read_attribute_asynch)
%   |- note: DO NOT MODIFY (OR ANSWER MAY BE LOST)
%
% Argin: timeout 
% --------------
%   |- type: 1-by-1 numeric array (i.e. any numeric type)
%   |- desc: If after <timeout> milliseconds the reply is still not available then generate an error. 
%            If set to 0, wait indefinitly. 
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
%   req_desc = tango_read_attributes_asynch('tango/tangotest/1', attr_name_list);
%   %- always check error
%   if (tango_error == -1)
%     %- handle error 
%     tango_print_error_stack;
%     return;
%   end
%   %- do some work...
%   do_some_work();
%   %- ...then get response (with a timeout of 1000 ms)
%   attr_val_list = tango_read_attributes_reply(req_desc, 1000);
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
%   See also TANGO_READ_ATTRIBUTES_ASYNCH, TANGO_READ_ATTRIBUTE_REPLY

% TANGO_READ_ATTRIBUTES cmd-id: 48
attr_val_list = tango_binding(int16(48), request_desc, timeout);
return;
