function prop_val = tango_get_property (dev_name, prop_name)
%TANGO_GET_PROPERTY Returns the value of a single device property stored into the TANGO database.  
%
% Syntax:
% -------
%   tango_get_property(dev_name, prop_name)
%
% Argin: dev_name 
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the target device
%
% Argin: prop_name
% ----------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the property to read
%
% Argout: prop_val
% ----------------
%   |- type: 1-by-1 struct array
%   |- desc: the property value
%   |- fields:
%        |  
%        |-|- name: name 
%        | |- type: 1-by-n char array (string)
%        | |- desc: the property name 
%        |
%        |-|- name: value
%          |- type: 1-by-n cell array {1-by-n char array}
%          |- desc: the property (string) values (1)
%
%	(1) Since there is no way for a generic TANGO client (such as 
%	this binding) to guess the actual type of the property value, 
%	it is returned as strings stored into a cell array. It is the 
%	user responsability to cast the cell content from string to 
%	the actual type (see example below).
%
% Example:
% --------
%   %- get property from database
%   prop_val = tango_get_property('tango/tangotest/3', 'polled_attr');
%   if (tango_error == -1)
%     tango_print_error_stack;
%     return;
%   end
%		%- display prop_val
%   disp(prop_val);
%	  %- convert polling period from string to double
%		attr = prop_val.value{1};
%		pp = str2num(prop_val.value{2});
%		disp(sprintf('Polling period for attribute %s is %.2f ms.\r', attr, pp));
%
%		This example generates the following output:
%
%		 name: 'polled_attr'
%		value: {'short_scalar', '250'}
%
%		Polling period for attribute short_scalar is 250 ms.
%
% See also TANGO_GET_PROPERTIES, TANGO_PUT_PROPERTY, TANGO_DEL_PROPERTY.

% TANGO_GET_PROPERTY cmd-id: 40
prop_val = tango_binding(int16(40), dev_name, prop_name);
return;
