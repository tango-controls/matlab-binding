function polling_status = tango_polling_status (dev_name)
%TANGO_POLLING_STATUS Returns a device polling status.
%
% See the TANGO control system manual for more information about 
% the polling mechanism.
%
% Syntax:
% -------
%   polling_status = tango_polling_status(dev_name)
%
% Argin: dev_name 
% ---------------
%   |- type: 1-by-n char array (string)
%   |- desc: the name of the target device
%
% Argout: polling_status
% ----------------------
%   |- type: 1-by-n struct array (one struct by polled object)
%   |- desc: the device polling status
%   |- fields:
%        |  
%        |-|- name: ob
%        | |- type: 1-by-n char array (string)
%        | |- desc: Polled [command or attribute] name
%        |
%        |-|- name: pp
%        | |- type: 1-by-n char array (string)
%        | |- desc: The [command or attribute] polling period in ms
%        |
%        |-|- name: bd
%        | |- type: 1-by-n char array (string)
%        | |- desc: The [command or attribute] polling buffer depth 
%        |
%        |-|- name: le
%        | |- type: 1-by-n char array (string)
%        | |- desc: Time needed for the last command execution or attribute reading in ms
%        |
%        |-|- name: lu
%        | |- type: 1-by-n char array (string)
%        | |- desc: 'Age' of the last record in the polling buffer (in ms)
%        |
%        |-|- name: dt
%        | |- type: 1-by-n char array (string)
%        | |- desc: Delta between last records (in ms)
%        |
%        |-|- name: ex
%          |- type: 1-by-n char array (string)
%          |- desc: Exception 
%
% Example:
% --------
%   %- get polling status
%   pstatus = tango_polling_status('dserver/tangotest/1');
%   %- always check error
%   if (tango_error == -1)
%     %- handle error
%     tango_print_error_stack;
%     return;
%   end
%   %- pstatus is valid, print it
%   disp(pstatus);
%
%   This code generates the following output:
%
%    ob: 'Polled command name = DevVoid'
%    pp: 'Polling period (mS) = 500'
%    bd: 'Polling ring buffer depth = 10'
%    le: 'Time needed for the last command execution (mS) = 0.000'
%    lu: 'Data not updated since 219 mS'
%    dt: 'Delta between last records (in mS) = 499, 500, 500, 500'
%    ex: 'no error'
%
%    ob: 'Polled attribute name = short_scalar'
%    pp: 'Polling period (mS) = 250'
%    bd: 'Polling ring buffer depth = 10'
%    le: 'Time needed for the last attribute reading (mS) = 0.000'
%    lu: 'Data not updated since 125 mS'
%    dt: 'Delta between last records (in mS) = 265, 250, 234, 250'
%    ex: 'no error'
%
% See also TANGO_STATUS

% TANGO_POLLING_STATUS cmd-id: 27
polling_status = tango_binding(int16(27), dev_name);
return;