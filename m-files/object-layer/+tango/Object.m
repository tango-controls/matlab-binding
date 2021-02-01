classdef Object < handle
    %tango.Object Database object
    
    properties (Constant)
        db=py.PyTango.Database()
    end
    properties
        name
    end
    
    methods
        function obj=Object(name)
            %Object(name)
            obj.name=name;
        end
        
        function del_property(obj,varargin)
            %Delete an object property (free property)
            %object.del_property(propname1,propname2,...)
            %propname:	property name
            %value:     property value as a cell array of strings
            obj.db.delete_property(obj.name,varargin);
        end
        
        function varargout=get_property(obj,varargin)
            %Get an object property (free property)
            %
            %[value1,value2,...]=object.get_property(propname1,propname2,...)
            %propname:	property name
            %value:     property value
            v=obj.db.get_property(obj.name,varargin);
            varargout=cellfun(@getp,varargin,'UniformOutput',false);
            function val=getp(propname)
                val=cellfun(@char,cell(py.list(get(v,propname))),'UniformOutput',false);
            end
        end
        
        function set_property(obj,varargin)
            %Set an object property (free property)
            %
            %object.set_property(propname,value)
            %propname:	property name
            %value:     property value as a cell array of strings
            %
            %object.set_property(propstruct)
            %propstruct:    struct where fieldname is the property name and
            %               value is the property value
            if length(varargin)==1 && isstruct(varargin{1})
                propnames=fieldnames(varargin{1})';
                propvalues=cellfun(@tango.property_string,struct2cell(varargin{1})','UniformOutput',false);
            else
                propnames=varargin(1:2:end);
                propvalues=cellfun(@tango.property_string, varargin(2:2:end),'UniformOutput',false);
            end
            def=[propnames;propvalues];
            obj.db.put_property(obj.name,struct(def{:}));
        end
    end
    
end
