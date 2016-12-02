function attr_config = tango_get_attribute_config (dev_name, attribute_name)
%TANGO_GET_ATTRIBUTE_CONFIG Query a device for information about a single attribute.
%
%   This function is equivalent to TANGO_ATTRIBUTE_QUERY. 
%   Type 'help tango_attribute_query' for function details.
%  
%   See also TANGO_GET_ATTRIBUTES_CONFIG, TANGO_ATTRIBUTE_LIST_QUERY, TANGO_COMMAND_QUERY

% build 1-by-1 cell array (i.e. an attribute list containing only one attritute)
attr_list = {attribute_name};
% then forward exec to tango_get_attributes_config
attr_config = tango_get_attributes_config(dev_name, attr_list);
return;