function tango_set_attributes_config (dev_name, attr_config_list)
%TANGO_SET_ATTRIBUTES_CONFIG Change several attribute configurations for the specified device.
%
% Syntax:
% -------
%   tango_set_attributes_config(dev_name, attr_config_list)
%
% Argin: dev_name 
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the target device
%
% Argin: attr_config_list
% -----------------------
%   |- type: 1-by-n struct array (one structure per attribute)
%   |- desc: attribute configuration list
%   |- fields:
%        |   
%        |- Note: some of the following fields are NOT modifiable 
%        |  at runtime. These fields are marked [HC]. Modifiable
%        |  fields are marked [M] and have a default value (dval).
%        |  
%        |-|- name: name [HC]
%        | |- type: 1-by-n char array (string)
%        | |- desc: the attribute name 
%        |
%        |-|- name: writable [HC]
%        | |- type: 1-by-1 double array
%        | |- desc: the attribute access mode as a numeric value (1)
%        |
%        |-|- name: writable_str [HC]
%        | |- type: 1-by-n char array (string)
%        | |- desc: the attribute access mode as a string (1)
%        |
%        |-|- name: data_format [HC]
%        | |- type: 1-by-1 double array
%        | |- desc: the attribute data format as a numeric value (2)
%        |
%        |-|- name: data_format_str [HC]
%        | |- type: 1-by-n char array (string)
%        | |- desc: the attribute data format as a string (2)
%        |
%        |-|- name: data_type [HC]
%        | |- type: 1-by-1 double array
%        | |- desc: the attribute data type as a numeric value (3)
%        |
%        |-|- name: data_type_str [HC]
%        | |- type: 1-by-n char array (string)
%        | |- desc: the attribute data type as a string (3)
%        |
%        |-|- name: max_m [HC]
%        | |- type: 1-by-1 double array
%        | |- desc: the maximum y dim (# of rows: 0 for SCALAR and VECTOR attribute)
%        |
%        |-|- name: max_n [HC]
%        | |- type: 1-by-1 double array
%        | |- desc: the maximum x dim (# of columns)
%        |
%        |-|- name: description [M]
%        | |- type: 1-by-n char array (string)
%        | |- desc: the attribute description
%        | |- dval: 'No description'
%        |
%        |-|- name: label [M]
%        | |- type: 1-by-n char array (string)
%        | |- desc: the attribute label
%        | |- dval: 'No label'
%        |
%        |-|- name: unit [M]
%        | |- type: 1-by-n char array (string)
%        | |- desc: the attribute unit
%        | |- dval: 'No unit'
%        |
%        |-|- name: standard_unit [M]
%        | |- type: 1-by-n char array (string)
%        | |- desc: conversion factor to MKSA unit
%        | |- dval: 'No standard unit'
%        |
%        |-|- name: display_unit [M]
%        | |- type: 1-by-n char array (string)
%        | |- desc: the attribute unit in a printable form
%        | |- dval: 'No display unit'
%        |
%        |-|- name: format [M]
%        | |- type: 1-by-n char array (string)
%        | |- desc: how to print attribute value (4)
%        | |- dval: 'No format'
%        |
%        |-|- name: min_value [M]
%        | |- type: 1-by-n char array (string)
%        | |- desc: the attribute minimum 'writable' value
%        | |- dval: 'Not specified'
%        |
%        |-|- name: max_value [M]
%        | |- type: 1-by-n char array (string)
%        | |- desc: the attribute maximum 'writable' value
%        | |- dval: 'Not specified'
%        |  
%        |-|- name: min_alarm [M]
%        | |- type: 1-by-n char array (string)
%        | |- desc: the attribute low alarm threshold
%        | |- dval: 'Not specified'
%        |
%        |-|- name: max_alarm [M]
%        | |- type: 1-by-n char array (string)
%        | |- desc: the attribute high alarm threshold
%        | |- dval: 'Not specified'
%        |
%        |-|- name: writable_attr_name [HC]
%        | |- type: 1-by-n char array (string)
%        | |- desc: the associated writable attribute (for READ-WITH-WRITE attribute only)
%        | |- dval: 'Not specified'
%        |
%        |-|- name: extensions [M]
%        | |- type: 1-by-n cell array {1-by-n char array} 
%        | |- desc: user extensions              
%        |
%        |-|- name: disp_level [HC]
%          |- type: 1-by-1 double array 
%          |- desc: the attribute display level (5)
%
%  Note: this structure maps the Tango::AttributeInfo structure.
%  See Appendix A - Reference part - Device Attribute of the 
%  TANGO control system manual. 
%
%  (1) - Attribute access mode:
%
%     0: 'RO'   (read-only)
%     1: 'RWW'  (read-with-write)
%     2: 'WO'   (write-only)
%     3: 'RW'   (read-write)
%
%  (2) - Attribute data format:
%
%     0: 'Scalar'  (1-by-1 array)
%     1: 'Vector'  (1-by-max_n array)
%     2: 'Image'   (max_m-by-max_n array)
%
%  (3) - Attribute data type
%     
%     for SCALAR attributes:
%       DEV_SHORT....2 '1-by-1 int16'
%       DEV_LONG.....3 '1-by-1 int32'
%       DEV_DOUBLE...5 '1-by-1 double'
%       DEV_STRING...8 '1-by-n char'
%
%     for VECTOR attributes:
%       DEV_SHORT....2 '1-by-n int16'
%       DEV_LONG.....3 '1-by-n int32'
%       DEV_DOUBLE...5 '1-by-n double'
%
%     for IMAGE attributes:
%       DEV_SHORT....2 'm-by-n int16'
%       DEV_LONG.....3 'm-by-n int32'
%       DEV_DOUBLE...5 'm-by-n double'
%
%  (4) - Attribute format
%
%     The attribute format is a string containing keywords
%     separated by the ';' character. The supported keywords
%     are: fixed, scientific, uppercase, showpoint, showpos, 
%     setw() and setprecision().
%
%     Example: 'scientific;uppercase;setprecision(3)'  
%
%  (5) - Display level
% 
%     0 - 'Operator'
%     1 - 'Expert'
%
% Example:
% --------
%   %- change configuration for a single attribute 
%   %- first, get current attribute configuration
%   attr_config = tango_get_attribute_config('tango/tangotest/1', 'short_scalar');
%   %- always check error
%   if (tango_error == -1)
%     %- handle error 
%     tango_print_error_stack;
%     return;
%   end
%   %- change some values
%   attr_config.description = 'a simple short scalar attribute';
%   attr_config.label = 'ssc';
%   attr_config.unit = 'A.U.';
%   attr_config.standard_unit = attr_config.unit;
%   attr_config.display_unit = attr_config.unit;
%   attr_config.min_value = '-16384';
%   attr_config.max_value = '16384';
%   %- now, change attribute configuration
%   tango_set_attributes_config(dev_name, attr_config);
%   %- always check error
%   if (tango_error == -1)
%     %- handle error 
%     tango_print_error_stack;
%     return;
%   end
%   
%   See also TANGO_ATTRIBUTE_LIST_QUERY, TANGO_ATTRIBUTE_QUERY

% TANGO_SET_ATTRIBUTES_CONFIG cmd-id: 16
tango_binding(int16(16), dev_name, attr_config_list);
return;