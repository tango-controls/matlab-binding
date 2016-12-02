function tango_print_cmd_list (dev_name)

  %- get command information
  cmds_info = tango_command_list_query(dev_name);
  %- always check error
  if (tango_error == -1)
    %- handle error
    tango_print_error_stack;
    return;
  end
  %- cmds_info is valid
  [m, n] = size(cmds_info);
  disp(sprintf('Device <%s> has %d command(s)\n', dev_name, n));
  for i = 1:n
    %- print each cmd info
    disp(cmds_info(i));
  end
  


