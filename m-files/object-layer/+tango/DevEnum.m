classdef DevEnum
  
  % Device Interface to Tango devices
  %
  % Constructor:
  %   >> de = tango.DevEnum(device_name, attr_name);
  
  properties
    device;
    attribute;
    labels;
    values;
    value2label;
    label2value;
  end
  
  methods
    % DevEnum constructor
    % ------------------------------------------------
    function o = DevEnum(device_name, attribute_name)
      ac = tango_get_attribute_config(device_name, attribute_name);
      assert(tango_error == 0 && ac.data_type ==  29);
      num_labels = size(ac.enum_labels.labels, 2);
      o.device = device_name;
      o.attribute = attribute_name;
      o.labels = cell(1, num_labels);
      o.values = int16(zeros(1, num_labels));
      o.value2label = containers.Map("KeyType", "uint32", "ValueType", "char");
      o.label2value = containers.Map("KeyType", "char", "ValueType", "uint32");
      for l = 1:num_labels;
        o.labels{l} = ac.enum_labels.labels{l};
        o.values(l) = int16(ac.enum_labels.ids(l));
        o.value2label(uint32(ac.enum_labels.ids(l))) = ac.enum_labels.labels{l};
        o.label2value(ac.enum_labels.labels{l}) = uint32(ac.enum_labels.ids(l));
      end
    end
  end % methods

end % classdef
