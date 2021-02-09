
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
        function obj = DevEnum(device_name, attribute_name)
            ac = tango_get_attribute_config(device_name, attribute_name);
            assert(tango_error == 0 && ac.data_type ==  29);
            num_labels = size(ac.enum_labels.labels, 2);
            obj.device = device_name;
            obj.attribute = attribute_name;
            obj.labels = tango.DevEnumLabels(ac.enum_labels);
            obj.values = int16(zeros(1, num_labels));
            obj.value2label = containers.Map("KeyType", "uint32", "ValueType", "char");
            obj.label2value = containers.Map("KeyType", "char", "ValueType", "uint32");
            for l = 1:num_labels
                obj.values(l) = int16(ac.enum_labels.ids(l));
                obj.value2label(uint32(ac.enum_labels.ids(l))) = ac.enum_labels.labels{l};
                obj.label2value(ac.enum_labels.labels{l}) = uint32(ac.enum_labels.ids(l));
            end
        end
    end % methods
   

end % classdef
