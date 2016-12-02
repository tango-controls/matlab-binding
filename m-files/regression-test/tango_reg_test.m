function result = tango_reg_test (dev_name, num_it)
%TANGO_REG_TEST Executes a TANGO binding regression test using the
%TangoTest device.
%
% Syntax:
% -------
%   success = tango_reg_test (dev_name, it)
%
% Argin: inst_name
% ----------------
%   |- type: 1-by-n char array (string)
%   |- desc: the instance name of TangoTest device
%
% Argin: it
% ---------
%   |- type: 1-by-1 double array 
%   |- desc: the # of times you want to execute the test
%
% Argout: result
% --------------
%   |- type: 1-by-1 double array 
%   |- desc: -1 on error, 0 otherwise
%
print_title('* TANGO BINDING FOR MATLAB *');
for counter = 1:num_it 
 if (do_it(dev_name) == -1)
    print_title('* REG-TEST FAILED *  REG-TEST FAILED *');
    print_title('* WARNING: THIS TEST MIGHT HAVE CHANGED THE <V1 BACKWARD COMPATIBILITY FLAG> *'); 
    result = -1;
    return;
 end
end
% if (test_groups(num_it) == -1)
%  print_title('* REG-TEST FAILED *  REG-TEST FAILED *');
%  result = -1;
%  return;
% end;
print_title('* REG-TEST PASSED *  REG-TEST PASSED *'); 
print_title('* WARNING: THIS TEST MIGHT HAVE CHANGED THE <V1 BACKWARD COMPATIBILITY FLAG> *'); 
result = 0;
return;

%==================================================================================
% FUNCTION do_it: THE ACTUAL REGRESSION TEST 
%==================================================================================
function result = do_it (dev_name)
%==================================================================================
% DEVICE NAME (OUR OWN TANGOTEST INSTANCE NAME GOES HERE) 
%==================================================================================
%dev_name = strcat('tango/tangotest/',inst_name);
%==================================================================================
disp(sprintf('- regression test on %s', dev_name));
%**********************************************************************************
%==================================================================================
%                                 MISC. FUNCTIONS
%==================================================================================
%**********************************************************************************
print_sub_title('TESTING MISC. FUNCTIONS');
%----------------------------------------------------------------------------------
disp('- testing tango_ping');
elapsed_usec = tango_ping(dev_name);
if (tango_error == -1) 
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp(sprintf('%s is up and ready [RTT: %f usec]\r', dev_name, elapsed_usec));
%----------------------------------------------------------------------------------
disp('- testing tango_admin_name');
adm_name = tango_admin_name(dev_name);
if (tango_error == -1) 
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp(sprintf('  `--> %s\r', adm_name));
%----------------------------------------------------------------------------------
disp('- testing tango_description');
desc = tango_description(dev_name);
if (tango_error == -1) 
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp(sprintf('  `--> %s\r', desc));
%----------------------------------------------------------------------------------
disp('- testing tango_status');
status = tango_status(dev_name);
if (tango_error == -1) 
  disp('  `--> ERROR');
  tango_print_error_stack;
  return;
end
disp(sprintf('  `--> %s\r', status));
%----------------------------------------------------------------------------------
disp('- testing tango_state');
state = tango_state(dev_name);
if (tango_error == -1) 
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp(sprintf('  `--> %s [%.0f]\r', state.name, state.value));
%----------------------------------------------------------------------------------
disp('- testing tango_info');
info = tango_info(dev_name);
if (tango_error == -1) 
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp(info);
%----------------------------------------------------------------------------------
disp('- testing tango_idl_version');
idl_ver = tango_idl_version(dev_name);
if (tango_error == -1) 
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp(sprintf('  `--> passed [%s supports TANGO IDL version %d]\r', dev_name, idl_ver));
%----------------------------------------------------------------------------------
disp('- testing tango_get_source');
src = tango_get_source(dev_name);
if (tango_error == -1) 
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp(sprintf('  `--> %s [%.0f]\r', src.name, src.value));
%----------------------------------------------------------------------------------
disp('- testing tango_set_source (as numeric value)');
tango_set_source(dev_name, mod(src.value + 1, 2));
if (tango_error == -1) 
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
%----------------------------------------------------------------------------------
disp('- testing tango_set_source (as string value)');
tango_set_source(dev_name, 'Device');
if (tango_error == -1) 
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
%----------------------------------------------------------------------------------
disp('- testing tango_set_source (as string value - error expected)');
tango_set_source(dev_name, 'DUMMY-SOURCE');
if (tango_error == 0) 
  disp('  `--> ERROR EXPECTED (tango_error should be -1)');
  result = -1; return;
end
%----------------------------------------------------------------------------------
disp('- testing tango_get_timeout');
tmo = tango_get_timeout(dev_name);
if (tango_error == -1) 
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp(sprintf('  `--> %.0f ms\r', tmo));
%----------------------------------------------------------------------------------
disp('- testing tango_set_timeout');
tango_set_timeout(dev_name, tmo + 1);
if (tango_error == -1) 
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
%----------------------------------------------------------------------------------
disp('-testing tango_black_box');
bb = tango_black_box(dev_name, 5);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
[m, n] = size(bb);
for i = 1:n
  disp(sprintf('\t%s\r', char(bb(i))));
end

%**********************************************************************************
%==================================================================================
%                                     COMMANDS
%==================================================================================
%**********************************************************************************
print_sub_title('TESTING COMMANDS AND COMMAND ORIENTED FUNCTIONS');
%==================================================================================
% DEVICE CMD QUERY
%==================================================================================
disp('-testing tango_command_query');
cmd_info = tango_command_query(dev_name, 'DevVarDoubleStringArray');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp(cmd_info);
%==================================================================================
% DEVICE CMD LIST QUERY
%==================================================================================
disp('-testing tango_command_list_query');
cmd_list = tango_command_list_query(dev_name);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
[m, n] = size(cmd_list);
for i = 1:n
  disp(cmd_list(i));
end
%==================================================================================
% TEST CMD: DevVoid
%==================================================================================
disp('-testing DevVoid command');
tango_command_inout(dev_name, 'DevVoid');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% TEST CMD: DevVarLongStringArray
%==================================================================================
disp('-testing DevVarLongStringArray command');
clear argin
argin.lvalue = int32(0:1023);
argin.svalue = cell(1,128);
argin.svalue(:) = {'dev-var-long-string-array-test'};
argout = tango_command_inout(dev_name, 'DevVarLongStringArray', argin);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
[argin_m, argin_n] = size(argin);
[argout_m, argout_n] = size(argout);
if (argin_m ~= argout_m || argin_n ~= argout_n)
  disp('  `--> ERROR::size(argin) ~= size(argout)');
end
[argin_m, argin_n] = size(argin.lvalue);
[argout_m, argout_n] = size(argout.lvalue);
if (argin_m ~= argout_m || argin_n ~= argout_n)
  disp('  `--> ERROR::size(argin.lvalue) ~= size(argout.lvalue)');
  result = -1; return;
end
if (argin.lvalue ~= argout.lvalue)
  disp('  `--> ERROR::argin.lvalue ~= argout.lvalue');
  result = -1; return;
end
[argin_m, argin_n] = size(argin.svalue);
[argout_m, argout_n] = size(argin.svalue);
if (argin_m ~= argout_m || argin_n ~= argout_n)
  disp('  `--> ERROR::size(argin.svalue) ~= size(argout.svalue)');
  result = -1; return;
end
diff = 0;
for i = 1:argin_n
  argin_str = argin.svalue{1,i};
  argout_str = argout.svalue{1,i};
  if (strcmp(argin_str, argout_str) ~= 1)
    diff = diff + 1;
  end
end
if (diff ~= 0)
  disp('  `--> ERROR::argin.svalue ~= argout.svalue');
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% TEST CMD: DevVarDoubleStringArray
%==================================================================================
disp('-testing DevVarDoubleStringArray command');
clear argin
argin.dvalue = 1024:2047;
argin.svalue = cell(1,128);
argin.svalue(:) = {'dev-var-double-string-array-test'};
argout = tango_command_inout(dev_name, 'DevVarDoubleStringArray', argin);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
[argin_m, argin_n] = size(argin);
[argout_m, argout_n] = size(argout);
if (argin_m ~= argout_m || argin_n ~= argout_n)
  disp('  `--> ERROR::size(argin) ~= size(argout)');
end
[argin_m, argin_n] = size(argin.dvalue);
[argout_m, argout_n] = size(argin.dvalue);
if (argin_m ~= argout_m || argin_n ~= argout_n)
  disp('  `--> ERROR::size(argin.dvalue) ~= size(argout.dvalue)');
  result = -1; return;
end
if (argin.dvalue ~= argout.dvalue)
  disp('  `--> ERROR::argin.dvalue ~= argout.dvalue');
  result = -1; return;
end
[argin_m, argin_n] = size(argin.svalue);
[argout_m, argout_n] = size(argin.svalue);
if (argin_m ~= argout_m || argin_n ~= argout_n)
  disp('  `--> ERROR::size(argin.svalue) ~= size(argout.svalue)');
  result = -1; return;
end
diff = 0;
for i = 1:argin_n
  argin_str = argin.svalue{1,i};
  argout_str = argout.svalue{1,i};
  if (strcmp(argin_str, argout_str) ~= 1)
    diff = diff + 1;
  end
end
if (diff ~= 0)
  disp('  `--> ERROR::argin.svalue ~= argout.svalue');
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% TEST CMD: DevVarStringArray
%==================================================================================
disp('-testing DevVarStringArray command');
clear argin
argin = cell(1,128);
argin(:) = {'dev-var-string-array-test'};
argout = tango_command_inout(dev_name, 'DevVarStringArray', argin);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
[argin_m, argin_n] = size(argin);
[argout_m, argout_n] = size(argout);
if (argin_m ~= argout_m || argin_n ~= argout_n)
  disp('  `--> ERROR::size(argin) ~= size(argout)');
end
diff = 0;
for i = 1:argin_n
  if (strcmp(argin{i}, argout{i}) ~= 1)
    diff = diff + 1;
  end
end
if (diff ~= 0)
  disp('  `--> ERROR::argin ~= argout');
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% TEST CMD: DevVarDoubleArray
%==================================================================================
disp('-testing DevVarDoubleArray command');
clear argin
argin = 0:0.1:128;
argout = tango_command_inout(dev_name, 'DevVarDoubleArray', argin);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
[argin_m, argin_n] = size(argin);
[argout_m, argout_n] = size(argout);
if (argin_m ~= argout_m || argin_n ~= argout_n)
  disp('  `--> ERROR::size(argin) ~= size(argout)');
end
if (argin ~= argout)
  disp('  `--> ERROR::argin ~= argout');
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% TEST CMD: DevVarFloatArray
%==================================================================================
disp('-testing DevVarFloatArray command');
clear argin
argin = single(-128:0.1:0);
argout = tango_command_inout(dev_name, 'DevVarFloatArray', argin);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
[argin_m, argin_n] = size(argin);
[argout_m, argout_n] = size(argout);
if (argin_m ~= argout_m || argin_n ~= argout_n)
  disp('  `--> ERROR::size(argin) ~= size(argout)');
end
if (argin ~= argout)
  disp('  `--> ERROR::argin ~= argout');
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% TEST CMD: DevVarULongArray
%==================================================================================
disp('-testing DevVarULongArray command');
clear argin
argin = uint32(0:256);
argout = tango_command_inout(dev_name, 'DevVarULongArray', argin);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
[argin_m, argin_n] = size(argin);
[argout_m, argout_n] = size(argout);
if (argin_m ~= argout_m || argin_n ~= argout_n)
  disp('  `--> ERROR::size(argin) ~= size(argout)');
end
if (argin ~= argout)
  disp('  `--> ERROR::argin ~= argout');
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% TEST CMD: DevVarLongArray
%==================================================================================
disp('-testing DevVarLongArray command');
clear argin
argin = int32(-1024:0);
argout = tango_command_inout(dev_name, 'DevVarLongArray', argin);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
[argin_m, argin_n] = size(argin);
[argout_m, argout_n] = size(argout);
if (argin_m ~= argout_m || argin_n ~= argout_n)
  disp('  `--> ERROR::size(argin) ~= size(argout)');
end
if (argin ~= argout)
  disp('  `--> ERROR::argin ~= argout');
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% TEST CMD: DevVarULong64Array
%==================================================================================
disp('-testing DevVarULong64Array command');
clear argin
argin = uint64(0:256);
argout = tango_command_inout(dev_name, 'DevVarULong64Array', argin);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  disp('  `--> this last error can be safely ignored if you are not running a recent version of the TangoTest device');
  %result = -1; return;
end
if (tango_error == 0)
    [argin_m, argin_n] = size(argin);
    [argout_m, argout_n] = size(argout);
    if (argin_m ~= argout_m || argin_n ~= argout_n)
      disp('  `--> ERROR::size(argin) ~= size(argout)');
    end
    if (argin ~= argout)
      disp('  `--> ERROR::argin ~= argout');
      result = -1; return;
    end
    disp('  `--> passed');
end
%==================================================================================
% TEST CMD: DevVarLong64Array
%==================================================================================
disp('-testing DevVarLong64Array command');
clear argin
argin = int64(-1024:0);
argout = tango_command_inout(dev_name, 'DevVarLong64Array', argin);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  tango_print_error_stack;
  disp('  `--> this last error can be safely ignored if you are not running a recent version of the TangoTest device');
  %result = -1; return;
end
if (tango_error == 0)
    [argin_m, argin_n] = size(argin);
    [argout_m, argout_n] = size(argout);
    if (argin_m ~= argout_m || argin_n ~= argout_n)
      disp('  `--> ERROR::size(argin) ~= size(argout)');
    end
    if (argin ~= argout)
      disp('  `--> ERROR::argin ~= argout');
      result = -1; return;
    end
    disp('  `--> passed');
end
%==================================================================================
% TEST CMD: DevVarUShortArray
%==================================================================================
disp('-testing DevVarUShortArray command');
clear argin
argin = uint16(0:1024);
argout = tango_command_inout(dev_name, 'DevVarUShortArray', argin);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
[argin_m, argin_n] = size(argin);
[argout_m, argout_n] = size(argout);
if (argin_m ~= argout_m || argin_n ~= argout_n)
  disp('  `--> ERROR::size(argin) ~= size(argout)');
end
if (argin ~= argout)
  disp('  `--> ERROR::argin ~= argout'); 
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% TEST CMD: DevVarShortArray
%==================================================================================
disp('-testing DevVarShortArray command');
clear argin
argin = int16(-1024:0);
argout = tango_command_inout(dev_name, 'DevVarShortArray', argin);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
[argin_m, argin_n] = size(argin);
[argout_m, argout_n] = size(argout);
if (argin_m ~= argout_m || argin_n ~= argout_n)
  disp('  `--> ERROR::size(argin) ~= size(argout)');
end
if (argin ~= argout)
  disp('  `--> ERROR::argin ~= argout');
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% TEST CMD: DevVarCharArray
%==================================================================================
disp('-testing DevVarCharArray command');
clear argin
argin = uint8(0:256);
argout = tango_command_inout(dev_name, 'DevVarCharArray', argin);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
[argin_m, argin_n] = size(argin);
[argout_m, argout_n] = size(argout);
if (argin_m ~= argout_m || argin_n ~= argout_n)
  disp('  `--> ERROR::size(argin) ~= size(argout)');
end
if (argin ~= argout)
  disp('  `--> ERROR::argin ~= argout');
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% TEST CMD: DevString
%==================================================================================
disp('-testing DevString command');
clear argin
argin = 'dev-string';
argout = tango_command_inout(dev_name, 'DevString', argin);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
[argin_m, argin_n] = size(argin);
[argout_m, argout_n] = size(argout);
if (argin_m ~= argout_m || argin_n ~= argout_n)
  disp('  `--> ERROR::size(argin) ~= size(argout)');
end
if (strcmp(argin, argout) ~= 1)
  disp('  `--> ERROR::argin ~= argout');
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% TEST CMD: DevBoolean
%==================================================================================
disp('-testing DevBoolean command');
clear argin
%argin = uint8(1);
argin = true;
argout = tango_command_inout(dev_name, 'DevBoolean', argin);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
[argin_m, argin_n] = size(argin);
[argout_m, argout_n] = size(argout);
if (argin_m ~= argout_m || argin_n ~= argout_n)
  disp('  `--> ERROR::size(argin) ~= size(argout)');
end
if (argin ~= argout)
  disp('  `--> ERROR::argin ~= argout');
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% TEST CMD: DevUShort
%==================================================================================
disp('-testing DevUShort command');
clear argin
argin = uint16(2^16 - 1);
argout = tango_command_inout(dev_name, 'DevUShort', argin);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
[argin_m, argin_n] = size(argin);
[argout_m, argout_n] = size(argout);
if (argin_m ~= argout_m || argin_n ~= argout_n)
  disp('  `--> ERROR::size(argin) ~= size(argout)');
end
if (argin ~= argout)
  disp('  `--> ERROR::argin ~= argout');
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% TEST CMD: DevShort
%==================================================================================
disp('-testing DevShort command');
clear argin
argin = int16(-2^16);
argout = tango_command_inout(dev_name, 'DevShort', argin);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
[argin_m, argin_n] = size(argin);
[argout_m, argout_n] = size(argout);
if (argin_m ~= argout_m || argin_n ~= argout_n)
  disp('  `--> ERROR::size(argin) ~= size(argout)');
end
if (argin ~= argout)
  disp('  `--> ERROR::argin ~= argout');
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% TEST CMD: DevULong
%==================================================================================
disp('-testing DevULong command');
clear argin
argin = uint32(2^32 -1);
argout = tango_command_inout(dev_name, 'DevULong', argin);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
[argin_m, argin_n] = size(argin);
[argout_m, argout_n] = size(argout);
if (argin_m ~= argout_m || argin_n ~= argout_n)
  disp('  `--> ERROR::size(argin) ~= size(argout)');
end
if (argin ~= argout)
  disp('  `--> ERROR::argin ~= argout');
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% TEST CMD: DevLong
%==================================================================================
disp('-testing DevLong command');
clear argin
argin = int32(-2^32);
argout = tango_command_inout(dev_name, 'DevLong', argin);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
[argin_m, argin_n] = size(argin);
[argout_m, argout_n] = size(argout);
if (argin_m ~= argout_m || argin_n ~= argout_n)
  disp('  `--> ERROR::size(argin) ~= size(argout)');
end
if (argin ~= argout)
  disp('  `--> ERROR::argin ~= argout');
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% TEST CMD: DevFloat
%==================================================================================
disp('-testing DevFloat command');
clear argin
argin = single(-pi);
argout = tango_command_inout(dev_name, 'DevFloat', argin);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
[argin_m, argin_n] = size(argin);
[argout_m, argout_n] = size(argout);
if (argin_m ~= argout_m || argin_n ~= argout_n)
  disp('  `--> ERROR::size(argin) ~= size(argout)');
end
if (argin ~= argout)
  disp('  `--> ERROR::argin ~= argout');
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% TEST CMD: DevDouble
%==================================================================================
disp('-testing DevDouble command');
clear argin
argin = pi;
argout = tango_command_inout(dev_name, 'DevDouble', argin);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
[argin_m, argin_n] = size(argin);
[argout_m, argout_n] = size(argout);
if (argin_m ~= argout_m || argin_n ~= argout_n)
  disp('  `--> ERROR::size(argin) ~= size(argout)');
end
if (argin ~= argout)
  disp('  `--> ERROR::argin ~= argout');
  result = -1; return;
end
disp('  `--> passed');

%**********************************************************************************
%==================================================================================
%                                    ASYNCH. COMMANDS
%==================================================================================
%**********************************************************************************
print_sub_title('TESTING ASYNCHRONOUS COMMANDS');
%==================================================================================
% TEST CMD: DevVarLongStringArray
%==================================================================================
disp('-testing asynch DevVarLongStringArray command');
dvla_argin.lvalue = int32(0:1023);
dvla_argin.svalue(1:256) = {'dev-var-long-string-array-test'};
req_desc = tango_command_inout_asynch(dev_name, 'DevVarLongStringArray', 0, dvla_argin);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
pause(0.1);
dvla_argout = tango_command_inout_reply(req_desc, 100);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
[argin_m, argin_n] = size(dvla_argin);
[argout_m, argout_n] = size(dvla_argout);
if (argin_m ~= argout_m || argin_n ~= argout_n)
  disp('  `--> ERROR::size(argin) ~= size(argout)');
end
[argin_m, argin_n] = size(dvla_argin.lvalue);
[argout_m, argout_n] = size(dvla_argin.lvalue);
if (argin_m ~= argout_m || argin_n ~= argout_n)
  disp('  `--> ERROR::size(argin.lvalue) ~= size(argout.lvalue)');
  result = -1; return;
end
if (dvla_argin.lvalue ~= dvla_argout.lvalue)
  disp('  `--> ERROR::argin.lvalue ~= argout.lvalue');
  result = -1; return;
end
[argin_m, argin_n] = size(dvla_argin.svalue);
[argout_m, argout_n] = size(dvla_argin.svalue);
if (argin_m ~= argout_m || argin_n ~= argout_n)
  disp('  `--> ERROR::size(argin.svalue) ~= size(argout.svalue)');
  result = -1; return;
end
diff = 0;
for i = 1:argin_n
  argin_str = dvla_argin.svalue{1,i};
  argout_str = dvla_argout.svalue{1,i};
  if (strcmp(argin_str, argout_str) ~= 1)
    diff = diff + 1;
  end
end
if (diff ~= 0)
  disp('  `--> ERROR::argin.svalue ~= argout.svalue');
  result = -1; return;
end
disp('  `--> passed');
%**********************************************************************************
%==================================================================================
%                                 ATTRIBUTES
%==================================================================================
%**********************************************************************************
print_sub_title('TESTING ATTRIBUTE ORIENTED FUNCTIONS');
%**********************************************************************************
% DEVICE ATTRIBUTES - GET ATTRIBUTE LIST
%**********************************************************************************
disp('-testing tango_get_attribute_list');
attr_list = tango_get_attribute_list(dev_name);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
[m, n] = size(attr_list);
for i = 1:n
  disp(attr_list(i));
end
%**********************************************************************************
% DEVICE ATTRIBUTES - READ ATTRIBUTE CONFIG
%**********************************************************************************
disp('-testing tango_get_attribute_config (get config. for one attribute)');
attr_config = tango_get_attribute_config(dev_name, 'short_scalar');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp(attr_config);
%**********************************************************************************
% DEVICE ATTRIBUTES - READ ATTRIBUTES CONFIG
%**********************************************************************************
disp('-testing tango_get_attributes_config (get config. for several attributes)');
attr_list = {'short_scalar', 'long_scalar', 'double_scalar', ...
             'short_spectrum', 'long_spectrum', 'double_spectrum', ...
             'short_image', 'long_image', 'double_image'};
attr_config = tango_get_attributes_config(dev_name, attr_list);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
[m, n] = size(attr_config);
for i = 1:n
  disp(attr_config(i));
end
%**********************************************************************************
% DEVICE ATTRIBUTES - WRITE ATTRIBUTE CONFIG
%**********************************************************************************
% disp('-testing tango_set_attribute_config');
% attr_config = tango_get_attribute_config(dev_name, 'short_scalar');
% if (tango_error == -1)
%   disp('  `--> ERROR');
%   tango_print_error_stack;
%   result = -1; return;
% end
% disp('  `--> original <short_scalar> config');
% disp(attr_config);
%change some values
%attr_config.description = 'a simple short scalar attribute';
%attr_config.label = 'sc';
%attr_config.unit = 'A.U.';
%attr_config.standard_unit = attr_config.unit;
%attr_config.display_unit = attr_config.unit;
%attr_config.format = '%d';
%attr_config.min_value = 'Not specified';
%attr_config.max_value = 'Not specified';
%attr_config.min_value = '-16384';
%attr_config.max_value = '+16384';
% tango_set_attributes_config(dev_name, attr_config);
% if (tango_error == -1)
%   disp('  `--> ERROR');
%   tango_print_error_stack;
%   result = -1; return; 
% end
% disp('  `--> passed');
% disp('  `--> modified <short_scalar> config');
% attr_config = tango_get_attribute_config(dev_name, 'short_scalar');
% if (tango_error == -1)
%   disp('  `--> ERROR');
%   tango_print_error_stack;
%   result = -1; return;
% end
% disp(attr_config);
%**********************************************************************************
% DEVICE ATTRIBUTES - READ/WRITE INDIVIDUALLY EACH ATTRIBUTE
%**********************************************************************************
print_sub_title('READING/WRITTING ATTRIBUTES (INDIVIDUALLY)');
%==================================================================================
% READ/WRITE ATTRIBUTE: string_scalar (read/write)
%==================================================================================
disp('-writing string_scalar');
str_in = 'a1:b2:c3:d4';
tango_write_attribute(dev_name, 'string_scalar', str_in );
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp(sprintf('  `--> passed (wrote %s)', str_in));
disp('-reading string_scalar');
str_out = tango_read_attribute(dev_name, 'string_scalar');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp(str_out);
if (strcmp(str_in, str_out.value) == 0) 
  disp('  `--> ERROR');
  result = -1; return;
end
%==================================================================================
% READ/WRITE ATTRIBUTE: boolean_scalar (read/write)
%==================================================================================
disp('-writing boolean_scalar');
val_in = true;
tango_write_attribute(dev_name, 'boolean_scalar', val_in);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp(sprintf('  `--> passed (wrote %d)', double(val_in)));
disp('-reading boolean_scalar');
val_out = tango_read_attribute(dev_name, 'boolean_scalar');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp(val_out);
if (val_in(1) ~= val_out.setpoint(1)) 
  disp('  `--> ERROR');
  result = -1; return;
end
%==================================================================================
% READ/WRITE ATTRIBUTE: uchar_scalar (read/write)
%==================================================================================
disp('-writing uchar_scalar');
val_in = uint8(16);
tango_write_attribute(dev_name, 'uchar_scalar', val_in);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp(sprintf('  `--> passed (wrote %d)', double(val_in)));
disp('-reading uchar_scalar');
val_out = tango_read_attribute(dev_name, 'uchar_scalar');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp(val_out);
if (val_in(1) ~= val_out.setpoint(1)) 
  disp('  `--> ERROR');
  result = -1; return;
end
%==================================================================================
% READ/WRITE ATTRIBUTE: short_scalar (read/write)
%==================================================================================
disp('-writing short_scalar');
val_in = int16(-4096);
tango_write_attribute(dev_name, 'short_scalar', val_in);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp(sprintf('  `--> passed (wrote %d)', double(val_in)));
disp('-reading short_scalar');
val_out = tango_read_attribute(dev_name, 'short_scalar');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp(val_out);
if (val_in(1) ~= val_out.setpoint(1)) 
  disp('  `--> ERROR');
  result = -1; return;
end
%==================================================================================
% READ/WRITE ATTRIBUTE: ushort_scalar (read/write)
%==================================================================================
disp('-writing ushort_scalar');
val_in = uint16(-4096);
tango_write_attribute(dev_name, 'ushort_scalar', val_in);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp(sprintf('  `--> passed (wrote %d)', double(val_in)));
disp('-reading ushort_scalar');
val_out = tango_read_attribute(dev_name, 'ushort_scalar');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp(val_out);
if (val_in(1) ~= val_out.setpoint(1)) 
  disp('  `--> ERROR');
  result = -1; return;
end
%==================================================================================
% READ/WRITE ATTRIBUTE: long_scalar (read/write)
%==================================================================================
disp('-writing long_scalar');
val_in = int32(8192);
tango_write_attribute(dev_name, 'long_scalar', val_in);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp(sprintf('  `--> passed (wrote %d)', double(val_in)));
disp('-reading long_scalar');
val_out = tango_read_attribute(dev_name, 'long_scalar');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp(val_out);
if (val_in(1) ~= val_out.setpoint(1)) 
  disp('  `--> ERROR');
  result = -1; return;
end
%==================================================================================
% READ/WRITE ATTRIBUTE: ulong_scalar (read/write)
%==================================================================================
disp('-writing ulong_scalar');
val_in = uint32(8192);
tango_write_attribute(dev_name, 'ulong_scalar', val_in);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  disp('  `--> this last error can be safely ignored if you are not running a recent version of the TangoTest device');
  %result = -1; return;
end
if (tango_error == 0)
    disp(sprintf('  `--> passed (wrote %d)', double(val_in)));
    disp('-reading ulong_scalar');
    val_out = tango_read_attribute(dev_name, 'ulong_scalar');
    if (tango_error == -1)
      disp('  `--> ERROR');
      tango_print_error_stack;
      result = -1; return;
    end
    disp('  `--> passed');
    disp(val_out);
    if (val_in(1) ~= val_out.setpoint(1)) 
      disp('  `--> ERROR');
      result = -1; return;
    end
end
%==================================================================================
% READ/WRITE ATTRIBUTE: long64_scalar (read/write)
%==================================================================================
disp('-writing long64_scalar');
val_in = int64(-1000000000 * 8192);
tango_write_attribute(dev_name, 'long64_scalar', val_in);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  disp('  `--> this last error can be safely ignored if you are not running a recent version of the TangoTest device');
  %result = -1; return;
end
if (tango_error == 0)
    disp(sprintf('  `--> passed (wrote %d)', double(val_in)));
    disp('-reading long64_scalar');
    val_out = tango_read_attribute(dev_name, 'long64_scalar');
    if (tango_error == -1)
      disp('  `--> ERROR');
      tango_print_error_stack;
      result = -1; return;
    end
    disp('  `--> passed');
    disp(val_out);
    if (val_in(1) ~= val_out.setpoint(1)) 
      disp('  `--> ERROR');
      result = -1; return;
    end
end
%==================================================================================
% READ/WRITE ATTRIBUTE: ulong64_scalar (read/write)
%==================================================================================
disp('-writing ulong64_scalar');
val_in = uint64(1000000000 * 8192);
tango_write_attribute(dev_name, 'ulong64_scalar', val_in);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  disp('  `--> this last error can be safely ignored if you are not running a recent version of the TangoTest device');
  %result = -1; return;
end
if (tango_error == 0)
    disp(sprintf('  `--> passed (wrote %d)', double(val_in)));
    disp('-reading ulong64_scalar');
    val_out = tango_read_attribute(dev_name, 'ulong64_scalar');
    if (tango_error == -1)
      disp('  `--> ERROR');
      tango_print_error_stack;
      result = -1; return;
    end
    disp('  `--> passed');
    disp(val_out);
    if (val_in(1) ~= val_out.setpoint(1)) 
      disp('  `--> ERROR');
      result = -1; return;
    end
end
%==================================================================================
% READ/WRITE ATTRIBUTE: float_scalar (read/write)
%==================================================================================
disp('-writing float_scalar');
val_in = single(pi);
tango_write_attribute(dev_name, 'float_scalar', val_in);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp(sprintf('  `--> passed (wrote %d)', val_in));
disp('-reading float_scalar');
val_out = tango_read_attribute(dev_name, 'float_scalar');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp(val_out);
if (val_in(1) ~= val_out.setpoint(1)) 
  disp('  `--> ERROR');
  result = -1; return;
end
%==================================================================================
% READ/WRITE ATTRIBUTE: double_scalar (read/write)
%==================================================================================
disp('-writing double_scalar');
val_in = pi;
tango_write_attribute(dev_name, 'double_scalar', pi);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp(sprintf('  `--> passed (wrote %d)', val_in));
disp('-reading double_scalar');
val_out = tango_read_attribute(dev_name, 'double_scalar');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp(val_out);
if (val_in(1) ~= val_out.setpoint(1)) 
  disp('  `--> ERROR');
  result = -1; return;
end
%==================================================================================
% READ ATTRIBUTE: boolean_spectrum (read/write)
%==================================================================================
disp('-reading boolean_spectrum_ro');
val_in = tango_read_attribute(dev_name, 'boolean_spectrum_ro');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-writing boolean_spectrum');
tango_write_attribute(dev_name, 'boolean_spectrum', val_in.value);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-reading boolean_spectrum');
val_out = tango_read_attribute(dev_name, 'boolean_spectrum');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-comparing read/write');
if (val_in.value ~= val_out.value) 
  disp('  `--> ERROR');
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% READ ATTRIBUTE: uchar_spectrum (read/write)
%==================================================================================
% disp('-reading uchar_spectrum_ro');
% val_in = tango_read_attribute(dev_name, 'uchar_spectrum_ro');
% if (tango_error == -1)
%   disp('  `--> ERROR');
%   tango_print_error_stack;
%   result = -1; return;
% end
% disp('  `--> passed');
% disp('-writing uchar_spectrum');
% tango_write_attribute(dev_name, 'uchar_spectrum', val_in.value);
% if (tango_error == -1)
%   disp('  `--> ERROR');
%   tango_print_error_stack;
%   result = -1; return;
% end
% disp('  `--> passed');
% disp('-reading uchar_spectrum');
% val_out = tango_read_attribute(dev_name, 'uchar_spectrum');
% if (tango_error == -1)
%   disp('  `--> ERROR');
%   tango_print_error_stack;
%   result = -1; return;
% end
% disp('  `--> passed');
% disp('-comparing read/write');
% if (val_in.value ~= val_out.value) 
%   disp('  `--> ERROR');
%  result = -1; return;
% end
% disp('  `--> passed');
%==================================================================================
% READ ATTRIBUTE: short_spectrum (read/write)
%==================================================================================
disp('-reading short_spectrum_ro');
val_in = tango_read_attribute(dev_name, 'short_spectrum_ro');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-writing short_spectrum');
tango_write_attribute(dev_name, 'short_spectrum', val_in.value);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-reading short_spectrum');
val_out = tango_read_attribute(dev_name, 'short_spectrum');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-comparing read/write');
if (val_in.value ~= val_out.value) 
  disp('  `--> ERROR');
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% READ ATTRIBUTE: ushort_spectrum (read/write)
%==================================================================================
disp('-reading ushort_spectrum_ro');
val_in = tango_read_attribute(dev_name, 'ushort_spectrum_ro');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-writing ushort_spectrum');
tango_write_attribute(dev_name, 'ushort_spectrum', val_in.value);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-reading ushort_spectrum');
val_out = tango_read_attribute(dev_name, 'ushort_spectrum');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-comparing read/write');
if (val_in.value ~= val_out.value) 
  disp('  `--> ERROR');
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% READ ATTRIBUTE: long_spectrum (read/write)
%==================================================================================
disp('-reading long_spectrum_ro');
val_in = tango_read_attribute(dev_name, 'long_spectrum_ro');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-writing long_spectrum');
tango_write_attribute(dev_name, 'long_spectrum', val_in.value);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-reading long_spectrum');
val_out = tango_read_attribute(dev_name, 'long_spectrum');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-comparing read/write');
if (val_in.value ~= val_out.value) 
  disp('  `--> ERROR');
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% READ ATTRIBUTE: ulong_spectrum (read)
%==================================================================================
disp('-reading ulong_spectrum_ro');
val_in = tango_read_attribute(dev_name, 'ulong_spectrum_ro');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  tango_print_error_stack;
  disp('  `--> this last error can be safely ignored if you are not running a recent version of the TangoTest device');
  %result = -1; return;
end
if (tango_error == 0)
    disp('  `--> passed');
end
%==================================================================================
% READ ATTRIBUTE: long64_spectrum (read)
%==================================================================================
disp('-reading long64_spectrum_ro');
val_in = tango_read_attribute(dev_name, 'long64_spectrum_ro');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  disp('  `--> this last error can be safely ignored if you are not running a recent version of the TangoTest device');
  %result = -1; return;
end
if (tango_error == 0)
    disp('  `--> passed');
end
%==================================================================================
% READ ATTRIBUTE: ulong64_spectrum (read)
%==================================================================================
disp('-reading ulong64_spectrum_ro');
val_in = tango_read_attribute(dev_name, 'ulong64_spectrum_ro');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  disp('  `--> this last error can be safely ignored if you are not running a recent version of the TangoTest device');
  %result = -1; return;
end
if (tango_error == 0)
    disp('  `--> passed');
end
%==================================================================================
% READ ATTRIBUTE: float_spectrum (read/write)
%==================================================================================
disp('-reading float_spectrum_ro');
val_in = tango_read_attribute(dev_name, 'float_spectrum_ro');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-writing float_spectrum');
tango_write_attribute(dev_name, 'float_spectrum', val_in.value);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-reading float_spectrum');
val_out = tango_read_attribute(dev_name, 'float_spectrum');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-comparing read/write');
if (val_in.value ~= val_out.value) 
  disp('  `--> ERROR');
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% READ ATTRIBUTE: double_spectrum (read/write)
%==================================================================================
disp('-reading float_spectrum_ro');
val_in = tango_read_attribute(dev_name, 'double_spectrum_ro');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-writing double_spectrum');
tango_write_attribute(dev_name, 'double_spectrum', val_in.value);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-reading double_spectrum');
val_out = tango_read_attribute(dev_name, 'double_spectrum');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-comparing read/write');
if (val_in.value ~= val_out.value) 
  disp('  `--> ERROR');
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% READ ATTRIBUTE: boolean_image (read/write)
%==================================================================================
disp('-reading boolean_image_ro');
val_in = tango_read_attribute(dev_name, 'boolean_image_ro');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-writing boolean_image');
tango_write_attribute(dev_name, 'boolean_image', val_in.value);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-reading boolean_image');
val_out = tango_read_attribute(dev_name, 'boolean_image');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-comparing read/write');
if (val_in.value ~= val_out.value) 
  disp('  `--> ERROR');
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% READ ATTRIBUTE: uchar_image (read/write)
%==================================================================================
disp('-reading uchar_image_ro');
val_in = tango_read_attribute(dev_name, 'uchar_image_ro');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-writing uchar_image');
tango_write_attribute(dev_name, 'uchar_image', val_in.value);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-reading uchar_image');
val_out = tango_read_attribute(dev_name, 'uchar_image');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-comparing read/write');
if (val_in.value ~= val_out.value) 
  disp('  `--> ERROR');
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% READ ATTRIBUTE: short_image (read/write)
%==================================================================================
% disp('-reading short_image_ro');
% val_in = tango_read_attribute(dev_name, 'short_image_ro');
% if (tango_error == -1)
%   disp('  `--> ERROR');
%   tango_print_error_stack;
%   result = -1; return;
% end
% disp('  `--> passed');
% disp('-writing short_image');
% tango_write_attribute(dev_name, 'short_image', val_in.value);
% if (tango_error == -1)
%   disp('  `--> ERROR');
%   tango_print_error_stack;
%   result = -1; return;
% end
% disp('  `--> passed');
% disp('-reading short_image');
% val_out = tango_read_attribute(dev_name, 'short_image');
% if (tango_error == -1)
%   disp('  `--> ERROR');
%   tango_print_error_stack;
%   result = -1; return;
% end
% disp('  `--> passed');
% disp('-comparing read/write');
% if (val_in.value ~= val_out.value) 
%   disp('  `--> ERROR');
%   result = -1; return;
% end
% disp('  `--> passed');
%==================================================================================
% READ ATTRIBUTE: ushort_image (read/write)
%==================================================================================
disp('-reading ushort_image_ro');
val_in = tango_read_attribute(dev_name, 'ushort_image_ro');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-writing ushort_image');
tango_write_attribute(dev_name, 'ushort_image', val_in.value);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-reading ushort_image');
val_out = tango_read_attribute(dev_name, 'ushort_image');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-comparing read/write');
if (val_in.value ~= val_out.value) 
  disp('  `--> ERROR');
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% READ ATTRIBUTE: long_image (read/write)
%==================================================================================
disp('-reading long_image_ro');
val_in = tango_read_attribute(dev_name, 'long_image_ro');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-writing long_image');
tango_write_attribute(dev_name, 'long_image', val_in.value);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-reading long_image');
val_out = tango_read_attribute(dev_name, 'long_image');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-comparing read/write');
if (val_in.value ~= val_out.value) 
  disp('  `--> ERROR');
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% READ ATTRIBUTE: ulong_image (read)
%==================================================================================
disp('-reading ulong_image_ro');
val_in = tango_read_attribute(dev_name, 'ulong_image_ro');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  tango_print_error_stack;
  disp('  `--> this last error can be safely ignored if you are not running a recent version of the TangoTest device');
  %result = -1; return;
end
if (tango_error == 0)
    disp('  `--> passed');
end
%==================================================================================
% READ ATTRIBUTE: ulong64_image (read)
%==================================================================================
disp('-reading ulong64_image_ro');
val_in = tango_read_attribute(dev_name, 'ulong64_image_ro');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  disp('  `--> this last error can be safely ignored if you are not running a recent version of the TangoTest device');
  %result = -1; return;
end
if (tango_error == 0)
    disp('  `--> passed');
end
%==================================================================================
% READ ATTRIBUTE: float_image (read/write)
%==================================================================================
disp('-reading float_image_ro');
val_in = tango_read_attribute(dev_name, 'float_image_ro');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-writing float_image');
tango_write_attribute(dev_name, 'float_image', val_in.value);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-reading float_image');
val_out = tango_read_attribute(dev_name, 'float_image');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-comparing read/write');
if (val_in.value ~= val_out.value) 
  disp('  `--> ERROR');
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% READ ATTRIBUTE: double_image (read/write)
%==================================================================================
disp('-reading double_image_ro');
val_in = tango_read_attribute(dev_name, 'double_image_ro');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-writing double_image');
tango_write_attribute(dev_name, 'double_image', val_in.value);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-reading double_image');
val_out = tango_read_attribute(dev_name, 'double_image');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp('-comparing read/write');
if (val_in.value ~= val_out.value) 
  disp('  `--> ERROR');
  result = -1; return;
end
disp('  `--> passed');
%**********************************************************************************
% DEVICE ATTRIBUTES - READ/WRITE ALL ATTRIBUTES IN ONE CALL
%**********************************************************************************
print_sub_title('READING/WRITTING ATTRIBUTES (GLOBALLY)');
%==================================================================================
% WRITE ALL ATTRIBUTES
%==================================================================================
disp('-testing tango_write_attributes (writting several attributes in one call)');
attr_name_val_list(1).name  = 'short_scalar';
attr_name_val_list(1).value = int16(4096);
attr_name_val_list(2).name  = 'long_scalar';
attr_name_val_list(2).value = int32(8192);
attr_name_val_list(3).name  = 'double_scalar';
attr_name_val_list(3).value = -pi;
tango_write_attributes(dev_name, attr_name_val_list);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
else
  disp('  `--> passed');
end
%==================================================================================
% READ ALL ATTRIBUTES
%==================================================================================
attr_list = {'short_scalar', 'long_scalar', 'double_scalar', ...
             'short_spectrum', 'long_spectrum', 'no_value', 'throw_exception', ...
             'double_spectrum', 'short_image', 'long_image', 'double_image'};
disp('-testing tango_read_attributes with <V1.x backward compatibility> enabled [should fail]');
tango_enable_v1_compatibility();
val = tango_read_attributes(dev_name, attr_list);
if (tango_error == -1)
  disp('  `--> OK: GOT EXPECTED ERROR');
  tango_print_error_stack;
else
  disp('  `--> ERROR EXPECTED - THERE IS A BUG SOMEWHERE!');
  result = -1; return;
end
disp('-testing tango_read_attributes with <V1.x backward compatibility> disabled [should pass]');
tango_disable_v1_compatibility();
val = tango_read_attributes(dev_name, attr_list);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
[m, n] = size(val);
disp(val);
for i = 1:n
  disp(val(i));
  if (val(i).has_failed)
    [me, ne] = size(val(i).error);  
    for j = 1:ne
        disp(val(i).error(j))
    end
  end
end
%==================================================================================
% ASYNCH WRITE/READ ATTRIBUTES
%==================================================================================
disp('-testing tango_write_attributes_asynch (asynch. writting several attributes in one call)');
attr_name_val_list(1).name  = 'short_scalar';
attr_name_val_list(1).value = int16(1024);
attr_name_val_list(2).name  = 'long_scalar';
attr_name_val_list(2).value = int32(4096);
attr_name_val_list(3).name  = 'double_scalar';
attr_name_val_list(3).value = 2 * pi;
req_desc = tango_write_attributes_asynch(dev_name, attr_name_val_list);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
else
  disp('  `--> tango_write_attributes_asynch passed');
end
tango_write_attributes_reply(req_desc, 0);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> tango_write_attributes_reply passed');
disp('-testing tango_read_attributes_asynch (asynch. reading all attributes in one call)');
req_desc = tango_read_attributes_asynch(dev_name, attr_list);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> tango_read_attributes_asynch passed');
pause(0.1);
val = tango_read_attributes_reply(req_desc, 0);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> tango_read_attributes_reply passed');
[m, n] = size(val);
for i = 1:n
  disp(val(i));
  if (val(i).has_failed)
    [me, ne] = size(val(i).error);  
    for j = 1:ne
        disp(val(i).error(j))
    end
  end
end
%==================================================================================
%                           ASYNCH ATTRIBUTE READ/WRITE
%==================================================================================
%==================================================================================
% ASYNCH WRITE/READ ATTRIBUTE: short_scalar
%==================================================================================
disp('-asynch. writing short_scalar');
val_in = int16(-1024);
req_desc = tango_write_attribute_asynch(dev_name, 'short_scalar', val_in);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp(sprintf('  `--> tango_write_attribute_asynch passed (wrote %d)', double(val_in)));
tango_write_attribute_reply(req_desc, 0);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> tango_write_attribute_reply passed');
disp('-asynch. reading short_scalar');
req_desc = tango_read_attribute_asynch(dev_name, 'short_scalar');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> tango_read_attribute_asynch passed');
val = tango_read_attribute_reply(req_desc, 0);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> tango_read_attribute_reply passed');
disp(val);
%==================================================================================
% ASYNCH READ ATTRIBUTE: short_spectrum
%==================================================================================
disp('-asynch. reading short_spectrum');
req_desc = tango_read_attribute_asynch(dev_name, 'short_spectrum');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> tango_read_attribute_asynch passed');
val = tango_read_attribute_reply(req_desc, 0);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> tango_read_attribute_reply passed');
disp(val);
%==================================================================================
% ASYNCH READ ATTRIBUTE: short_image
%==================================================================================
disp('-asynch. reading short_image');
req_desc = tango_read_attribute_asynch(dev_name, 'short_image');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> tango_read_attribute_asynch passed');
val = tango_read_attribute_reply(req_desc, 0);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> tango_read_attribute_reply passed');
disp(val);
%**********************************************************************************
%==================================================================================
%                                   POLLING
%==================================================================================
%**********************************************************************************
print_sub_title('TESTING POLLING ORIENTED FUNCTIONS');
%==================================================================================
% CMD POLLING
%==================================================================================
disp('-testing tango_poll_command');
tango_poll_command(dev_name, 'DevVoid', 500);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% CHECK CMD POLLING
%==================================================================================
disp('-testing tango_is_command_polled');
polled = tango_is_command_polled(dev_name, 'DevVoid');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
if (polled ~= 1)
  disp('  `--> ERROR::cmd DevVoid should be polled');
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% CMD POLLING PERIOD
%==================================================================================
disp('-testing tango_get_command_poll_period');
pp = tango_get_command_poll_period(dev_name, 'DevVoid');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
if (pp ~= 500)
  disp(sprintf('  `--> ERROR::polling period should be 500 ms - got %.2fms', pp));
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% ATTR POLLING
%==================================================================================
disp('-testing tango_poll_attribute');
polled_attr_name = 'short_scalar'; %'throw_exception'; %'no_value'; %'short_scalar';
tango_poll_attribute(dev_name, polled_attr_name, 100);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% CHECK ATTR POLLING
%==================================================================================
disp('-testing tango_is_attribute_polled');
polled = tango_is_attribute_polled(dev_name, polled_attr_name);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
if (polled ~= 1)
  disp('  `--> ERROR::attr short_scalar should be polled');
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% ATTR POLLING PERIOD
%==================================================================================
disp('-testing tango_get_attribute_poll_period');
pp = tango_get_attribute_poll_period(dev_name, polled_attr_name);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
if (pp ~= 100)
  disp(sprintf('  `--> ERROR::polling period should be 100 ms - got %.2fms', pp));
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% WAIT A SECOND FOR POLLING BUFFERS TO BE FILLED
%==================================================================================
disp('-waiting for polling buffers to be filled');
pause(0.25);
%==================================================================================
% POLLING STATUS
%==================================================================================
disp('-testing tango_polling_status');
ps = tango_polling_status(dev_name);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
[m, n] = size(ps);
for i = 1:n
  disp(ps(i));
end
%==================================================================================
% DEVICE CMD HISTORY
%==================================================================================
disp('-testing tango_command_history');
hist = tango_command_history(dev_name, 'DevVoid', 5);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  %result = -1; return;
else
  disp('  `--> passed');
  [m, n] = size(hist);
  for i = 1:n
    disp(hist(i));
  end
end
%==================================================================================
% DEVICE ATTRIBUTE HISTORY
%==================================================================================
disp('-testing tango_attribute_history');
hist = tango_attribute_history(dev_name, polled_attr_name, 5);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  %result = -1; return;
else
  disp('  `--> passed');
  [m, n] = size(hist);
  for i = 1:n
    disp(sprintf('\t-Attr.History(%d) ----------------\r', i));
    disp(hist(i));
    if (hist(i).has_failed)
        disp(sprintf('\t-Attr.History(%d).error ----------\r', i));
        [me, ne] = size(hist(i).error);
        for e = 1:ne
            disp(hist(i).error(e));  
        end
    else
        disp(sprintf('\t-Attr.History(%d).value ----------\r', i));
        disp(hist(i).value);  
    end
  end
end
%==================================================================================
% STOP POLLING CMD 
%==================================================================================
disp('-testing tango_stop_poll_command');
tango_stop_poll_command(dev_name, 'DevVoid');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
polled = tango_is_command_polled(dev_name, 'DevVoid');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
if (polled ~= 0)
  disp('  `--> ERROR:: cmd DevVoid should NOT be polled');
  result = -1; return;
end
disp('  `--> passed');
%==================================================================================
% STOP POLLING ATTR 
%==================================================================================
disp('-testing tango_stop_poll_attribute');
tango_stop_poll_attribute(dev_name, polled_attr_name);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
polled = tango_is_attribute_polled(dev_name, polled_attr_name);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
if (polled ~= 0)
  disp('  `--> ERROR::attr should NOT be polled');
  result = -1; return;
end
disp('  `--> passed');
%**********************************************************************************
%==================================================================================
%                                   PROPERTIES
%==================================================================================
%**********************************************************************************
print_sub_title('TESTING PROPERTY ORIENTED FUNCTIONS');
%==================================================================================
% GET SINGLE PROPERTY
%==================================================================================
disp('-testing tango_get_property');
prop = tango_get_property(dev_name, 'mthreaded_impl');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
disp(prop);
%==================================================================================
% GET SEVERAL PROPERTIES
%==================================================================================
disp('-testing tango_get_properties');
prop_list = {'mthreaded_impl', 'sleep_period', 'polled_attr'};
prop = tango_get_properties(dev_name, prop_list);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
[m, n] = size(prop);
for i = 1:n
  disp(prop(i));
end
%==================================================================================
% PUT SINGLE PROPERTY
%==================================================================================
disp('-testing tango_put_property');
tango_put_property(dev_name, 'dummy_0', {'dummy_0_value_1', 'dummy_0_value_2'});
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
prop = tango_get_property(dev_name, 'mthreaded_impl');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp(prop);
%==================================================================================
% PUT SEVERAL PROPERTIES
%==================================================================================
% disp('-testing tango_put_properties');
% clear prop
% prop(1).name = 'mthreaded_impl';
% prop(1).value = {'1'};
% prop(2).name = 'sleep_period';
% prop(2).value = {'100'};
% prop(2).name = 'dummy_1';
% prop(2).value = {'dummy_1_value_1', 'dummy_1_value_2', 'dummy_1_value_3'};
% prop(3).name = 'dummy_2';
% prop(3).value = {'dummy_2_value_1', 'dummy_2_value_2'};
% prop(4).name = 'dummy_3';
% prop(4).value = {'dummy_3_value_1'};
% tango_put_properties(dev_name, prop);
% if (tango_error == -1)
%   disp('  `--> ERROR');
%   tango_print_error_stack;
%   result = -1; return;
% end
% disp('  `--> passed');
% clear prop
% prop = {'mthreaded_impl', 'sleep_period'};
% prop = tango_get_properties(dev_name, prop);
% if (tango_error == -1)
%   disp('  `--> ERROR');
%   tango_print_error_stack;
%   result = -1; return;
% end
% [m, n] = size(prop);
% for i = 1:n
%   disp(prop(i));
% end
%==================================================================================
% DELETE SINGLE PROPERTY
%==================================================================================
disp('-testing tango_del_property');
tango_del_property(dev_name, 'dummy_0');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
clear prop
prop = {'dummy_0', 'dummy_1', 'dummy_2', 'dummy_3'};
prop = tango_get_properties(dev_name, prop);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
[m, n] = size(prop);
for i = 1:n
  disp(prop(i));
end
%==================================================================================
% DELETE SEVERAL PROPERTIES
%==================================================================================
disp('-testing tango_del_properties');
tango_del_properties(dev_name, {'dummy_1', 'dummy_2', 'dummy_3'});
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
disp('  `--> passed');
clear prop
prop = {'dummy_1', 'dummy_2', 'dummy_3'};
prop = tango_get_properties(dev_name, prop);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  result = -1; return;
end
[m, n] = size(prop);
for i = 1:n
  disp(prop(i));
end
%==================================================================================
% END OF REGRESSION TEST
%==================================================================================
result = 0;
return;

%**********************************************************************************
% TANGO GROUP TEST
%**********************************************************************************
function err = test_groups (num_it)
print_sub_title('TESTING TANGO GROUPS');
disp('-testing tango_group_create: bpm');
bpm_gid = tango_group_create('bpm');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  return;
end
disp('-testing tango_group_add: ANS-C*/DG/BPM.*');
tango_group_add(bpm_gid, 'ANS-C*/DG/BPM.*');
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  err = -1;
  return;
end
disp('-testing tango_group_dump');
tango_group_dump(bpm_gid);
disp('-testing tango_group_remove: ANS-C*/DG/*.NOD');
tango_group_remove(bpm_gid, 'ANS-C*/DG/*.NOD', 0);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  err = -1;
  return;
end
disp('-testing tango_group_dump');
tango_group_dump(bpm_gid);
disp('-testing tango_group_ping');
all_alive = tango_group_ping(bpm_gid);
if (all_alive ~= 1)
    disp('  `--> WARNING: some devices do not respond');
end
disp('-testing tango_group_read_attribute');
err = test_tango_group_read_attribute (bpm_gid, 'XPosSA', 0);
if (err == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  return;
end
disp('-testing tango_group_read_attributes');
tango_group_read_attributes(bpm_gid, {'XPosSA', 'ZPosSA'}, 0);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  err = -1;
  return;
end
disp('-testing tango_group_kill');
tango_group_kill(bpm_gid);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  err = -1;
  return;
end
err = 0;
return;

%**********************************************************************************
% test_tango_group_read_attribute
%**********************************************************************************
function err = test_tango_group_read_attribute (grp_id, attr, fwd)
r = tango_group_read_attribute(grp_id, attr, fwd);
if (tango_error == -1)
  disp('  `--> ERROR');
  tango_print_error_stack;
  err = -1;
  return;
end
if (r.has_failed)
  [r_m, r_n] = size(r.replies);
  for i = 1:r_n
      if (r.replies(i).has_failed)
        disp(r.replies(i));
        [err_m, err_n] = size(r.replies(i).error);
        for j = 1:err_n
            disp(r.replies(i).error(j));
        end
      end
  end
end
err = 0;
return;

%**********************************************************************************
% print_sub_title
%**********************************************************************************
function print_sub_title (sub_title) 
disp(cat(2, '-| ', sub_title));  
return;


%**********************************************************************************
% print_title
%**********************************************************************************
function print_title (title)
[m, n] = size(title);
l(1:n) = char('*');
disp(l);
disp(title);
disp(l);
return;


