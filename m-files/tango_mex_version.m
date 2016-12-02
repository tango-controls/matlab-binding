function tango_mex_ver = tango_mex_version
%TANGO_MEX_VERSION Returns the version number the TANGO Mex-file.
%
% Syntax:
% -------
%   tango_mex_ver = tango_mex_version
%
% Argout: tango_mex_ver
% ---------------------
%   |- type: 1-by-n char array (string)
%   |- desc: the TANGO binding version number.
%
% Example:
% --------
%   %- get tango MEX-file version
%   tango_mex_ver = tango_mex_version;
%   %- always check error
%   if (tango_error == -1)
%     %- handle error
%     tango_print_error_stack;
%     return;
%   end
%   %- tango_mex_ver is valid, print it
%   disp(tango_mex_ver);
%
%   This code generates the following output:
%
%      1.1.0
%
% See also TANGO_VERSION

% TANGO_STATUS cmd-id: -2
tango_mex_ver = tango_binding(int16(-2));
return;