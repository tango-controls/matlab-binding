function tango_print_error_stack
%TANGO_PRINT_ERROR_STACK Prints the TANGO error stack in the command window.
%
% Syntax:
% -------
%   tango_print_error_stack
%
% Example:
% --------
%   %- get "unknown command" info
%   cmd_info = tango_command_query('tango/tangotest/3', 'dummy');
%   %- always check error
%   if (tango_error == -1)
%     %- handle error (cmd_info is not valid - DO NOT USE IT)
%     %- print error stack
%     tango_print_error_stack;
%     %- can't continue
%     return;
%   end
%   %- cmd_info is valid
%   disp(cmd_info);
%
%   This code generates the following output:
%
%   ************************************************************
%   *                    TANGO ERROR STACK                     *
%   ************************************************************
%   - ERROR 1
%   	|-reason.....API_CommandNotFound
%   	|-desc.......Command dummy not found
%   	|-origin.....Device_2Impl::command_query_2
%   	|-severity...Error (1)
%   - ERROR 2
%   	|-reason.....command_query failed
%   	|-desc.......failed to execute command_query on device tango/tangotest/3
%   	|-origin.....TangoBinding::command_query
%   	|-severity...Error (1)
%   ************************************************************
%   
%   See also TANGO_ERROR_STACK, TANGO_ERROR

% does matlab have <strfind>?
try
  pos = strfind('dummy', char(10)); 
  print_error_stack_using_strfind;
catch
  print_error_stack_as_it_is;
end
return;

%------------------------------------------------------------------------------------------------
% SUB FUNCTION : <strfind> implementation of print_error_stack
%------------------------------------------------------------------------------------------------
function print_error_stack_using_strfind
% get the error stack
err_stack = tango_error_stack;
% display each DevError in the array
[err_stack_m, err_stack_n] = size(err_stack); 
stars(1:60) = char('*');
disp(stars);
disp('*                    TANGO ERROR STACK                     *')
disp(stars);
for i = 1:err_stack_n
  disp(sprintf('- ERROR %d', i));
  %- print reason
  pos = strfind(err_stack(1,i).reason, char(10)); 
  if (isempty(pos)) 
    disp(sprintf('\t|-reason.....%s',err_stack(1,i).reason));
  else
    disp(sprintf('\t|-desc'));  
    [m,n] = size(pos);
    [m,reason_n] = size(err_stack(1,i).reason);
    from = 1;
    for j = 1:n + 1
      if (j == n + 1)
        to = desc_n;
      else 
        to = pos(j) - 1;
      end
      disp(sprintf('\t   `--> %s', err_stack(1,i).reason(from:to)));
      if (j < n + 1)
        from = pos(j) + 1;
      end
    end
  end 
  %- print desc
  pos = strfind(err_stack(1,i).desc, char(10)); 
  if (isempty(pos)) 
    disp(sprintf('\t|-desc.......%s',err_stack(1,i).desc));  
  else
    disp(sprintf('\t|-desc'));  
    [m,n] = size(pos);
    [m,desc_n] = size(err_stack(1,i).desc);
    from = 1;
    for j = 1:n + 1
      if (j == n + 1)
        to = desc_n;
      else 
        to = pos(j) - 1;
      end
      disp(sprintf('\t   `--> %s', err_stack(1,i).desc(from:to)));
      if (j < n + 1)
        from = pos(j) + 1;
      end
    end
  end
  %- print origin
  disp(sprintf('\t|-origin.....%s',err_stack(1,i).origin));
  %- print severity
  disp(sprintf('\t|-severity...%s (%d)',err_stack(1,i).severity_str, err_stack(1,i).severity));
end
disp(stars)
return;
%------------------------------------------------------------------------------------------------
% SUB FUNCTION : <simple> implementation of print_error_stack
%------------------------------------------------------------------------------------------------
function print_error_stack_as_it_is
% get the error stack
err_stack = tango_error_stack;
% display each DevError in the array
[err_stack_m, err_stack_n] = size(err_stack); 
stars(1:60) = char('*');
disp(stars);
disp('*                    TANGO ERROR STACK                     *')
disp(stars);
for i = 1:err_stack_n
  disp(sprintf('- ERROR %d\r', i));
  %- print reason
  disp(sprintf('\t|-reason.....%s',err_stack(1,i).reason));
  %- print desc
  disp(sprintf('\t|-desc.......%s',err_stack(1,i).desc));  
  %- print origin
  disp(sprintf('\t|-origin.....%s',err_stack(1,i).origin));
  %- print severity
  disp(sprintf('\t|-severity...%s (%d)',err_stack(1,i).severity_str, err_stack(1,i).severity));
end
disp(stars)
return;