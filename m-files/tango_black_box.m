function bb = tango_black_box (dev_name, black_box_len)
%TANGO_BLACK_BOX Returns the last <n> commands executed on the device
%
% Syntax:
% -------
%   black_box = tango_black_box(dev_name, n)
%
% Argin: dev_name 
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the target device
%
% Argin: n
% --------
%   |- type: 1-by-1 double array
%   |- desc: the last <n> commands executed will be returned
%
% Argout: black_box
% -----------------
%   |- type: 1-by-n cell array - each cell contains a 1-by-n char array (string)
%   |- desc: the last n commands executed on the device (date, time, source and 
%            command name are provided)
%
% Example:
% --------
%   %- get last 5 commands executed on device
%   bbox = tango_black_box('tango/tangotest/3', 5);
%   %- always check error
%   if (tango_error == -1)
%     %- handle error
%     tango_print_error_stack;
%     return;
%   end
%   %- bbox is valid, print its content
%   [m, n] = size(bbox);
%   for i = 1:n
%     disp(sprintf('\t%s\r', char(bbox(i))));
%   end
%
%   This code generates the following output:
%
%    13/08/2002 16:23:46:52 : Operation info requested from pc103inf.soleil.u-psud.fr
%    13/08/2002 16:23:46:50 : Attribute state requested from pc103inf.soleil.u-psud.fr
%    13/08/2002 16:23:46:50 : Attribute status requested from pc103inf.soleil.u-psud.fr
%    13/08/2002 16:23:46:49 : Attribute description requested from pc103inf.soleil.u-psud.fr
%    13/08/2002 16:23:46:45 : Attribute adm_name requested from pc103inf.soleil.u-psud.fr
%

% TANGO_BLACK_BOX cmd-id: 17
bb = tango_binding(int16(17), dev_name,  black_box_len);
return;