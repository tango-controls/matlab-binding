classdef DevEnum %< dynamicprops
  
  % Device Interface to Tango devices
  %
  % Constructor:
  %   >> de = tango.DevEnum(device_name, attr_name);

  properties
    dev;
    attr;
    labels;
    values;
  endproperties
  
  properties(Access=private,Hidden)
    value2label;
    label2value;
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
      o.value2label = containers.Map("KeyType", "uint32", "ValueType", "char");
      o.label2value = containers.Map("KeyType", "char", "ValueType", "uint32");
      for l = 1:num_labels
        %p = addprop(o, ac.enum_labels.labels{l});
        %s.type = '.';
        %s.subs = ac.enum_labels.labels{l};
        %subsasgn(o, s, ac.enum_labels.ids(l));
        o.value2label(uint32(ac.enum_labels.ids(l))) = ac.enum_labels.labels{l};
        o.label2value(ac.enum_labels.labels{l}) = uint32(ac.enum_labels.ids(l));
      endfor
    endfunction
   
    % subsref overloading
    % ------------------------------------------------
    function sref = subsref(obj,s)
      % obj(i) is equivalent to obj.Data(i)
      %disp(s(1).subs)
      %disp(class(s(1).subs))
      switch s(1).type
        case '.'
          if strcmp(s(1).subs, "labels")
            sref = obj.label2value.keys;
          elseif strcmp(s(1).subs, "values") 
            sref = obj.value2label.keys;
          elseif strcmp(s(1).subs, "dev")
            sref = obj.dev;
          elseif strcmp(s(1).subs, "attr")
            sref = obj.attr;
          else
            try 
              sref = obj.label2value(s(1).subs);
            catch 
              error('invalid DevEnum label specified');
            end
          endif
        case '()'
          try 
            sref = obj.value2label(uint32(s(1).subs{1}));
          catch 
            error('invalid DevEnum value specified');
          end
        otherwise
          sref = builtin('subsref',obj,s);
      end
    end
    
  endmethods
  
end