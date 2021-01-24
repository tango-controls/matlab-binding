classdef DevEnum < dynamicprops
  
  % Device Interface to Tango devices
  %
  % Constructor:
  %   >> de = tango.DevEnum(device_name, attr_name);

  properties
    dev;
    attr;
    values;
  endproperties
  
  methods
    
    % DevEnum constructor
    % ------------------------------------------------
    function o = DevEnum(device_name, attribute_name)
      ac = tango_get_attribute_config(device_name, attribute_name);
      assert(tango_error == 0 && ac.data_type ==  29);
      num_labels = size(ac.enum_labels.labels)(2);
      o.dev = device_name;
      o.attr = attribute_name;
      %o.values = strings(num_labels)
      for l = 1:num_labels
        p = addprop(o, ac.enum_labels.labels{l});
        s.type = '.';
        s.subs = ac.enum_labels.labels{l};
        subsasgn(o, s, ac.enum_labels.ids(l));
        %o.values(l) = ac.enum_labels.labels{l};
      endfor
    endfunction
    
  endmethods
  
end