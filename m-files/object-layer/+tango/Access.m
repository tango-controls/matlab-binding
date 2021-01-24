classdef Access < handle
    %tango.Access: handle connections with tango devices
    
    %#function tango.Init
    properties(Constant,Access=protected,Hidden)
        ini=tango.Init.instance()
    end
    
    properties(Transient,Access=protected,Hidden)
        handle=''
    end
    
    properties(Dependent=true, SetAccess=private)
        Info        % Device information (server, host...) (RO)
        Admin_name  % Administrator Device (RO)
    end
    properties(Dependent=true)
        Timeout     % Client time-out [s] (RW)
    end
    
    properties(Abstract,Access=protected,Hidden)
        devname
    end
    
    methods(Static,Access=protected,Hidden)
        function attr = attrdefine(attrinfo, pollperiod)
            attr=rmfield(attrinfo,{'writable','writable_str','data_format',...
                'data_format_str','data_type','data_type_str'});
            attr.writable=tango.AttrWriteType(attrinfo.writable);
            attr.data_format=tango.AttrDataFormat(attrinfo.data_format);
            attr.data_type=tango.Type(attrinfo.data_type);
            attr.standard_unit=str2double(attrinfo.standard_unit);
            attr.display_unit=str2double(attrinfo.display_unit);
            attr.min_value=str2double(attrinfo.min_value);
            attr.max_value=str2double(attrinfo.max_value);
            attr.min_alarm=str2double(attrinfo.min_alarm);
            attr.max_alarm=str2double(attrinfo.max_alarm);
            if attr.data_format == tango.AttrDataFormat.SCALAR
                attr.default=attr.data_type.Undefined();
            else
                attr.default=[];
            end
            attr.is_enum = false
            if attr.data_type == tango.Type.DEV_STATE
                attr.conversion=@(x) tango.DevState.Get(x);
            elseif attr.data_type == tango.Type.DEV_ENUM
                attr.is_enum = true
                attr.enum = tango.DevEnum(attrinfo)
                %attr.conversion=@(x) tango.DevEnum.Get(x);
            else
                attr.conversion=@(x) x;
            end
            attr.poll_period=pollperiod;
        end
        function val=attrprocess(attrinfo,reply)
            if isempty(reply.error)
                error=0;
            else
                error=tango.Error(reply.name,'tango_read_attribute',...
                    'tango_read_attribute',reply.error);
            end
            val=struct(...
                'set',{attrinfo.conversion(reply.setpoint)},...
                'read',{attrinfo.conversion(reply.value)},...
                'time',{reply.time},...
                'quality',{tango.AttrQuality(reply.quality)},...
                'error',{error}...
                );
        end
        function info=cmddefine(cmdinfo,pollperiod)
            info=rmfield(cmdinfo,{'in_type','out_type',...
                'devin_type','devout_type'});
            info.in_type=tango.Type(cmdinfo.devin_type);
            info.out_type=tango.Type(cmdinfo.devout_type);
            info.poll_period=pollperiod;
        end
    end
    
    methods(Access=protected,Hidden)
        function varargout=dvz(self,dvfunc,varargin)
            if nargout==0   % tango bindings with no return value may
                dvfunc(self.handle, varargin{:}); % return an error code
            else
                varargout=cell(1,nargout);
                [varargout{:}]=dvfunc(self.handle, varargin{:});
            end
            if tango_error < 0
                tango.Error(self.devname,func2str(dvfunc),func2str(dvfunc),tango_error_stack).throwAsCaller();
            end
        end
    end
    
    methods
        function inf=get.Info(self)
            inf=self.dvz(@tango_info);
        end
        function adname=get.Admin_name(self)
            adname=self.dvz(@tango_admin_name);
        end
        function tmo=get.Timeout(self)
            tmo=0.001*self.dvz(@tango_get_timeout);
        end
        function set.Timeout(self,Value)
            self.dvz(@tango_set_timeout,1000*Value);
        end
        function set.handle(self,name)
            %fprintf('Set devname to %s\n',name);
            if tango_open_device(name) == 0
                self.handle=name;
                cs.dbgprintf(1,'Connected to <%s>',name);
            else
                tango.Error(name,'tango_open_device','tango_open_device',tango_error_stack).throw();
            end
        end
        function err=error(self,id,varargin)
            %Generates a Tango error
            %error(id,errorstack)
            %error(id,reason,desc)
            origin='';
            st=dbstack(1);
            if length(st)>=1, origin=st(1).name; end
            err=tango.Error(self.devname,id,origin,varargin{:});
        end
        function delete(self)
            if cs.dbgprintf(2,'tango.Access destructor'), dbstack; end
            if ~isempty(self.handle)
                cs.dbgprintf(1,'Disconnected from <%s>',self.devname);
                self.dvz(@tango_close_device);
            else
                cs.dbgprintf(1,'Deleting empty device');
            end
        end
    end
end
