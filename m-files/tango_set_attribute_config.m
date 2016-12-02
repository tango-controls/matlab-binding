function tango_set_attribute_config (dev_name, attr_config)
%TANGO_SET_ATTRIBUTE_CONFIG Change a single attribute configuration for the specified device.
%
%   See TANGO_SET_ATTRIBUTES_CONFIG for function details.

% forward exec to tango_set_attributes_config
tango_set_attributes_config(dev_name, attr_config);
return;