function tango_enable_v1_compatibility
%TANGO_ENABLE_V1_COMPATIBILITY
%
% Syntax:
% -------
%   tango_enable_v1_compatibility;
%   
%   This influences the <tango_read_attributes> behaviour in case (at least) one attribute gave error:
%   When <v1_compatibility> is enabled, <tango_read_attributes> simply returns an error (no value for any attribute).
%   When <v1_compatibility> is disabled, <tango_read_attributes> returns an array of structures containing both valid data and error(s).
%
% See also TANGO_DISABLE_V1_COMPATIBILITY

% TANGO_ENABLE_V1_COMPATIBILITY cmd-id: 75
tango_binding(int16(75));
return;