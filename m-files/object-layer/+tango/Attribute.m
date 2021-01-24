classdef (ConstructOnLoad) Attribute  < tango.Access
    %Tango Attribute
    %
    %Constructor:
    %   >> attr=tango.Attribute(attribute_name);
    %   >> attr=tango.Attribute(...,'Default',default_value)
    %
    %Access to read value:
    %   >> readvalue=attr.read;
    %   >> readvalue=attr.data.read;
    %   >> readvalue=attr.value();
    %   >> readvalue=attr.value(default_value);
    %   >> readvalue=attr.value(...,'formatted');
    %
    %Access to setpoint:
    %   >> setpoint=attr.set;
    %   >> setpoint=attr.data.set;
    %   >> setpoint=attr.setpoint();
    %   >> setpoint=attr.setpoint(default_value);
    %   >> setpoint=attr.setpoint(...,'formatted');
    %
    %To set the attribute value:
    %   >> attr.set=value;
    %   >> attr.write(value);
    
    properties(Access=protected,Hidden)
        devname=''
        attrname=''
        defval
    end
    properties(Transient,SetAccess=private)
        attrinfo	% Structure containing the Tango properties of the attribute
    end
    properties(Dependent=true,SetAccess=private)
        Description	% Attribute description (RO)
        device      % Parent device [RO]
        data        % Structure with set,read,time,quality,error fields [RO]
        read        % Read value [RO]
    end
    properties(Dependent=true)
        set         % Setpoint [RW]
    end
    
    methods(Access=protected,Hidden)
        function val=getdt(self,defv)
            try
                reply=self.dvz(@tango_read_attribute,self.attrinfo.name);
                val=self.attrprocess(self.attrinfo,reply);
            catch err
                if ~isstruct(defv) && strncmp(err.identifier,'Cs:Tango',8)
                    warning(err.identifier,err.message);
                    val=struct('set',defv,'read',defv);
                else
                    err.rethrow();
                end
            end
        end
        function res=decfmt(self,values,fmtflag)
            if fmtflag
                res=cellfun(@(attr,val) [sprintf(attr.attrinfo.format,attr.attrinfo.display_unit*val) ...
                    ' ' attr.attrinfo.unit],num2cell(self),values,'UniformOutput',false);
            else
                res=values;
            end
        end
        function [data,formatted]=decdata(self,varargin)
            [formatted,args]=cs.getflag(varargin,'formatted');
            [~,args]=cs.getflag(args,'');   % For backward compatibility
            data=self.getdt(cs.getargs(args,{self.defval}));
        end
    end
    
    methods
        function attr=Attribute(varargin)
            %attr=tango.Attribute(attribute_name)
            %attr=tango.Attribute(...,'Default',default_value)
            %
            %attribute_name may be a structure with fields 'devname' and 'attrname'
            %
            [defval,args]=cs.getoption(varargin,'Default',struct());
            % struct() is considered as an impossible value for a tango
            % attribute and so is taken as the "undefined" value, while []
            % and NaN are valid attribute values
            if length(args)==1 && isstruct(args{1})
                dnames={args{1}.devname};
                anames={args{1}.attrname};
            else
                [dnames,anames]=cellfun(@splitname,cs.allnames(args{:}),'UniformOutput',false);
            end
            n=length(dnames);
            if n>1, attr(1,n)=tango.Attribute(); end
            for i=1:n
                attr(i).devname=dnames{i}; %#ok<AGROW>
                attr(i).attrname=anames{i}; %#ok<AGROW>
                attr(i).defval=defval; %#ok<AGROW>
            end
            function [dn,an]=splitname(name)
                delim=strfind(name,'/');
                dn=name(1:delim(end)-1);
                an=name(delim(end)+1:end);
            end
        end
        function set.devname(self,name)
            self.devname=name;
            self.handle=name;
        end
        function set.attrname(self,attrname)
            if isstruct(attrname)
                self.attrinfo=attrname.info; %#ok<MCSUP>
                self.attrname=attrname.name;
            else
                self.attrinfo=self.attrdefine(...
                    self.dvz(@tango_attribute_query,attrname),...
                    self.dvz(@tango_get_attribute_poll_period,attrname)); %#ok<MCSUP>
                self.attrname=attrname;
            end
        end
        function set.set(self,val)
            self.dvz(@tango_write_attribute,self.attrinfo.name,val);
        end
        function val=get.set(self)
            val=self.data.set;
        end
        function val=get.data(self)
            val=self.getdt(self.defval);
        end
        function val=get.read(self)
            val=self.data.read;
        end
        function dev=get.device(self)
            dev=tango.Device(self.devname);
        end
        function desc=get.Description(self)
            desc=self.attrinfo.description;
        end
        function varargout=value(self,varargin)
            %readv=attr.value(format,default_value)
            %   format: 'formatted' for getting the formatted value as a char array
            %   default_value: value attributed in case of a Tango error
            
            %data=arrayfun(@(obj) decdata(obj,varargin{:}),self);
            %dspfunc=arrayfun(@(obj) decfmt(obj,varargin{:}),self,'UniformOutput',false);
            %varargout=cellfun(@(fun,dt) fun(dt),dspfunc,{data.read},'UniformOutput',false);
            [datav,formatted]=self.decdata(varargin{:});
            varargout=self.decfmt({datav.read},formatted);
        end
        function varargout=setpoint(self,varargin)
            %setp=attr.setpoint(format,default_value)
            %   format: 'formatted' for getting the formatted value as a char array
            %   default_value: value attributed in case of a Tango error
            [datav,formatted]=self.decdata(varargin{:});
            varargout=self.decfmt({datav.set},formatted);
        end
        function data=history(self,nb)
            %data=attr.history(nb)
            %   get the nb last values stored for the attribute
            reply=self.dvz(@tango_attribute_history,self.attrinfo.name,nb);
            data=arrayfun(@(rep) self.attrprocess(self.attrinfo,rep),reply);
        end
        function write(self,val)
            %attr.write(value)
            %   set the attribute to the specified value
            %   equivalent to attr.set=value
            self.dvz(@tango_write_attribute,self.attrinfo.name,val);
        end
        function strname=char(self)
            %Convert to character array (string)
            strname=[self.devname '/' self.attrname];
        end
        function disp(self)
            %Display the device definition
            disp([class(self) '(''' char(self) ''')']);
        end
    end
    
end

