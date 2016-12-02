%TANGO_BINDING 
% The TANGO binding for Matlab main help page.   
%
% Miscellaneous methods    
%    tango_info                         - get device information
%    tango_description                  - get device description
%    tango_admin_name                   - get the administrator device name
%    tango_state                        - get device state
%    tango_status                       - get device status
%    tango_ping                         - ping device
%    tango_get_timeout                  - get device timeout
%    tango_set_timeout                  - set device timeout
%    tango_get_source                   - get device source
%    tango_set_source                   - set device source
%    tango_black_box                    - get last commands for device
%
% Error Handling
%    tango_error                        - get TANGO error code
%    tango_error_stack                  - get TANGO error stack
%    tango_print_error_stack            - print TANGO error code
%
% Command related methods
%    tango_command_query                - get command information
%    tango_command_list_query           - get command list
%    tango_command_inout                - execute command
%    tango_command_inout_asynch         - execute asynchronous command
%    tango_command_inout_reply          - get result of asynchronous command
%    tango_command_history              - get command history
%    tango_print_cmd_list               - print command list
%
% Attribute related methods
%    tango_attribute_query              - get attribut information
%    tango_attribute_list_query         - get attribut information
%    tango_attribute_history            - get attribute history
%    tango_get_attribute_list           - get attribute list
%    tango_get_attributes_list          - get attribute list (several)
%    tango_get_attribute_config         - get attribute information
%    tango_set_attribute_config         - set attribute information
%    tango_set_attributes_config        - set attribute information (several)
%    tango_read_attribute               - read attribute
%    tango_read_attributes              - read attributes 
%    tango_read_attribute_asynch        - read asynchronously attribute
%    tango_read_attributes_asynch       - read asynchronously attributes
%    tango_read_attribute_reply         - get result of asynchronous reading
%    tango_read_attributes_reply        - get result of asynchronous readings
%    tango_write_attribute              - write attribute 
%    tango_write_attributes             - write attributes  
%    tango_write_attribute_asynch       - write asynchronously attribute 
%    tango_write_attributes_asynch      - write asynchronously attributes
%    tango_write_attribute_reply        - get result of asynchronous writting
%    tango_write_attributes_reply       - get result of asynchronous writtings
%
% Group handling
%    tango_group_add                    - add objects (group or devices) to an existing TANGO group       
%    tango_group_create                 - creates a new TANGO group 
%    tango_group_read_attribute         - read a single attribute on a TANGO group
%    tango_group_write_attribute        - write a single attribute on a TANGO group 
%    tango_group_command_inout          - executes asynchronously the TANGO command <cmd_name> on the specified group
%    tango_group_dump                   - dump a TANGO group into the command window 
%    tango_group_read_attribute_asynch  - read asynchronously a single attribute on a TANGO group
%    tango_group_write_attribute_asynch - writes asynchronously a single attribute on a TANGO group
%    tango_group_command_inout_asynch   - executes asynchronously the TANGO command <cmd_name> on the specified group
%    tango_group_id                     - returns the id of the group named <group_name>
%    tango_group_read_attribute_reply   - returns the result of a pending attribute reading
%    tango_group_write_attribute_reply  - returns the result of a pending attribute writting
%    tango_group_command_inout_reply    - returns the result of a pending asynch. command
%    tango_group_kill                   - kill/Release a TANGO group and its sub-groups
%    tango_group_remove                 - remove object(s) (group or devices) from an existing TANGO group
%    tango_group_ping                   - ping all device in a group
%    tango_group_contains               - looks for element in a group
%    tango_group_size                   - returns size of a group
%    tango_group_disable                - disables a member of a group without removing it from the group
%    tango_group_enable                 - enables a member of a group whose status is disabled
%    tango_group_read_attributes        - read several attributes in a group
%    tango_group_read_attributes_reply  - returns the result of a pending attribute reading
%
% Polling related methods
%    tango_polling_status               - get polling device status
%   ATTRIBUTE
%    tango_is_attribute_polled          - check if attribute is polled 
%    tango_get_attribute_poll_period    - get attribute polling period
%    tango_poll_attribute               - start polling attribute
%    tango_stop_poll_attribute          - stop polling attribute
%   COMMAND
%    tango_is_command_polled            - check if command is polled
%    tango_get_command_poll_period      - get polling status
%    tango_poll_command                 - start polling command
%    tango_stop_poll_command            - stop polling command
%
% Property related methods
%    tango_get_property                 - set device property
%    tango_get_properties               - set device propertie
%    tango_put_property                 - set device property
%    tango_put_properties               - set device properties
%    tango_del_property                 - delete device property
%    tango_del_properties               - delete device properties
%
% Others
%   BACKWARD COMPATIBILITY
%    tango_enable_v1_compatibility      - enable <tango_read_attributes> backward compatibility (default behaviour)
%    tango_disable_v1_compatibility     - disable <tango_read_attributes> backward compatibility
%   PROXY
%    tango_open_device                  - open device connection (reload device interface)
%    tango_close_device                 - close device connection
%   VERSION 
%    tango_idl_version                  - get idl version
%    tango_mex_version                  - get mex version
%    tango_version                      - get tango version
% 
%
%See help method name for details and examples
% eg: help tango_error