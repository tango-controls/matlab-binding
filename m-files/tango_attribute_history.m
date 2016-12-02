function attr_hist = tango_attribute_history (dev_name, attr_name, hist_depth)
%TANGO_ATTRIBUTE_HISTORY Retrieve an attribute history from the polling buffer.
%
% See the TANGO control system manual for more information about 
% the polling mechanism.
%
% Syntax:
% -------
%   attr_hist = tango_command_history(dev_name, attr_name, hist_depth)
%
% Argin: dev_name 
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the target device
%
% Argin: attr_name 
% ----------------
%   |- type: 1-by-n char array (string)
%   |- desc: the attribute name
%
% Argin: hist_depth 
% -----------------
%   |- type: 1-by-1 double array
%   |- desc: the number of "historical values" to return.
%
% Argout: attr_hist
% -----------------
%   |- type: 1-by-n struct array (with n <= hist_depth)
%   |- desc: the <attr_name> history - an array of <AttributeValue>
%   |- fields: 
%        |
%        |- |- name: time
%        |  |- type: 1-by-1 double array
%        |  |- desc: the timestamp (1)
%        |
%        |- |- name: value
%        |  |- type: 1-by-1 struct array (maps the Tango::DeviceAttribute structure)
%        |  |- desc: the command argout (i.e. command result)
%        |  |- fields:
%        |      |  
%        |      |-|- name: name 
%        |      | |- type: 1-by-n char array (string)
%        |      | |- desc: the attribute name 
%        |      |
%        |      |-|- name: quality
%        |      | |- type: 1-by-1 double array
%        |      | |- desc: the quality of this attribute value (numeric form) (2)
%        |      |
%        |      |-|- name: quality_str
%        |      | |- type: 1-by-n char array (string)
%        |      | |- desc: the quality of this attribute value (text form) (2)
%        |      |
%        |      |-|- name: m
%        |      | |- type: 1-by-1 double array
%        |      | |- desc: the size of the y dimension (# of rows) (3)
%        |      |
%        |      |-|- name: n
%        |      | |- type: 1-by-1 double array
%        |      | |- desc: the size of the x dimension (# of columns) (3)
%        |      |
%        |      |-|- name: time
%        |      | |- type: 1-by-1 double array 
%        |      | |- desc: time-stamp (4)
%        |      |
%        |      |-|- name: value
%        |        |- type: one of the TANGO supported types for attributes (4) 
%        |        |- desc: the actual value
%        |
%        |- |- name: has_failed
%        |  |- type: 1-by-1 double array (with logical flag set)
%        |  |- desc: true if the command gave error, false otherwise
%        |
%        |- |- name: errors
%           |- type: 1-by-n struct array (maps the Tango::DevFailed exception) 
%           |- desc: error details (5)
%           |- fields: 
%                |
%                |-|- name: reason
%                | |- type: 1-by-n char array (string)
%                | |- desc: the error type 
%                |
%                |-|- name: desc
%                | |- type: 1-by-n char array (string)
%                | |- desc: the detailed error description
%                |
%                |-|- name: origin
%                | |- type: 1-by-n char array (string)
%                | |- desc: the error source
%                |
%                |-|- name: severity
%                | |- type: 1-by-1 double array
%                | |- desc: the error severity as numeric value (6)
%                |
%                |-|- name: severity_str
%                  |- type: 1-by-n char array (string)
%                  |- desc: the error severity as string value (6)     
%
%  Note: this structure maps the Tango::DeviceAttributeHistoryList structure. 
%  See the TANGO C++ API manual. 
%
%  (1) - The command timestamp is a "Matlab serial date number". See 
%  the Matlab <datenum> function for more information about the serial 
%  date format. 
%
%  (2) - Quality
%            
%     0: 'VALID'
%     1: 'INVALID'
%     2: 'ALARM'
%     3: 'CHANGING'
%    
%  (3) - Dimensions
%        
%     n is always set to 0 for SCALAR and SPECTRUM attribute     
%
%  (4) - Attribute data type
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
%  (5) - Error details
%
%     The <errors> is undefined (i.e. NULL) if the <has_failed> is set to <false>.
%     Always check <has_failed> value before trying to use the <errors> field (see example below).
%
%  (6) - Error severity (maps the Tango::ErrSeverity enumeration)
% 
%     0 - 'Warning'
%     1 - 'Error'
%     2 - 'Panic'
%
% Example:
% --------
%   hist = tango_attribute_history('tango/tangotest/1', 'short_scalar', 5);
%   if (tango_error == -1)
%     tango_print_error_stack;
%     return;
%   end
%   [m, n] = size(hist);
%   for i = 1:n
%     disp(sprintf('\t-Attr.History(%d) ----------------\r', i));
%     disp(hist(i));
%     disp(sprintf('\t-Attr.History(%d).value ----------\r', i));
%     disp(hist(i).value);  
%   end
%   
% See also TANGO_READ_ATTRIBUTE, TANGO_READ_ATTRIBUTES

% TANGO_ATTRIBUTE_HISTORY cmd-id: 26
attr_hist = tango_binding(int16(26), dev_name, attr_name, hist_depth);
return;
