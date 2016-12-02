classdef (ConstructOnLoad) Attribute  < tango.Access
    %Tango Attribute
    %
    %Constructor:
    %   >> attr=tango.Attribute(attribute_name);
    %
    %Access to read value:
    %   >> readvalue=attr.read;
    %   >> readvalue=attr.data.read;
    %   >> readvalue=attr.value;
    %   >> readvalue=attr.value('formatted');
    %   >> readvalue=attr.value('',default_value);
    %
    %Access to setpoint:
    %   >> setpoint=attr.set;
    %   >> setpoint=attr.data.set;
    %   >> setpoint=attr.setpoint;
    %   >> setpoint=attr.setpoint('formatted');
    %   >> setpoint=attr.setpoint('',default_value);
    %
    %To set the attribute value:
    %   >> attr.set=value;
    %   >> attr.write(value);
    
    properties(Access=protected,Hidden)
        devname=''
        attrname=''
    end
    properties(Transient,SetAccess=private)
        attrinfo	% Structure containing the Tango properties of the attribute
    end
    properties(Dependent=true,SetAccess=private)
        device	% Parent device [RO]
        data	% Sructure with set,read,time,quality,error fields [RO]
        read    % Read value [RO]
    end
    properties(Dependent=true)
        set     % Setpoint [RW]
    end
    
    methods(Access=protected,Hidden)
        function res=decfmt(self,values,fmtflag,varargin)
            if nargin >=3 && strcmpi(fmtflag,'formatted')
                res=cellfun(@(attr,val) [sprintf(attr.attrinfo.format,attr.attrinfo.display_unit*val) ...
                    ' ' attr.attrinfo.unit],num2cell(self),values,'UniformOutput',false);
            else
                res=values;
            end
        end
        function data=decdata(self,~,varargin)
            try
                data=[self.data];
            catch err
                if nargin >= 3 && strncmp(err.identifier,'Cs:Tango',8)
                    def=cell(1,length(self));
                    def(:)=varargin;
                    data=struct('set',def,'read',def);
                else
                    err.rethrow();
                end
            end
        end
    end
    
    methods
        function attr=Attribute(varargin)
            %attr=tango.Attribute(attribute_name)
            
            %fprintf('Attribute constructor with %d arguments\n',nargin);
            if nargin==1 && isstruct(varargin{1})
                attr.devname=varargin{1}.devname;
                attr.attrname=varargin{1}.attrname;
            else
                attnames=cs.allnames(varargin{:});
                n=length(attnames);
                [dnames,anames]=cellfun(@splitname,attnames,'UniformOutput',false);
                if n>1, attr(1,n)=tango.Attribute(); end
                for i=1:n
                    attr(i).devname=dnames{i}; %#ok<AGROW>
                    attr(i).attrname=anames{i}; %#ok<AGROW>
                end
            end
            function [dn,an]=splitname(name)
                delim=strfind(name,'/');
                dn=name(1:delim(3)-1);
                an=name(delim(3)+1:end);
            end
        end
        function set.devname(self,name)
            %fprintf('Set devname to %s\n',name);
            self.devname=name;
            self.handle=name;
        end
        function set.attrname(self,attrname)
            %fprintf('Set attrname(%s) to %s\n',self.devname,attrname);
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
            reply=self.dvz(@tango_read_attribute,self.attrinfo.name);
            val=self.attrprocess(self.attrinfo,reply);
        end
        function val=get.read(self)
            val=self.data.read;
        end
        function dev=get.device(self)
            dev=tango.Device(self.devname);
        end
        function varargout=value(self,varargin)
            %readv=attr.value(format,default_value)
            %   format: 'formatted' for getting the formatted value as a char array
            %   default_value: value attributed in case of a Tango error
            
            %data=arrayfun(@(obj) decdata(obj,varargin{:}),self);
            %dspfunc=arrayfun(@(obj) decfmt(obj,varargin{:}),self,'UniformOutput',false);
            %varargout=cellfun(@(fun,dt) fun(dt),dspfunc,{data.read},'UniformOutput',false);
            datav=self.decdata(varargin{:});
            varargout=self.decfmt({datav.read},varargin{:});
        end
        function varargout=setpoint(self,varargin)
            %setp=attr.setpoint(format,default_value)
            %   format: 'formatted' for getting the formatted value as a char array
            %   default_value: value attributed in case of a Tango error
            datav=self.decdata(varargin{:});
            varargout=self.decfmt({datav.set},varargin{:});
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

