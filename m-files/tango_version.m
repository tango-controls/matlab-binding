function tango_ver = tango_version
%TANGO_VERSION Returns the version of the TANGO library used to comiple the TANGO Mex-file.
%
% Syntax:
% -------
%   tango_ver = tango_version
%
% Argout: tango_ver
% -----------------
%   |- type: 1-by-n char array (string)
%   |- desc: the TANGO library used to comiple the TANGO binding.
%
% Example:
% --------
%   %- get tango lib version
%   tango_ver = tango_version;
%   %- always check error
%   if (tango_error == -1)
%     %- handle error
%     tango_print_error_stack;
%     return;
%   end
%   %- tango_ver is valid, print it
%   disp(tango_ver);
%
%   This code generates the following output:
%
%      2.1.1
%
% See also TANGO_MEX_VERSION

% TANGO_STATUS cmd-id: -1
tango_ver = tango_binding(int16(-1));
return;