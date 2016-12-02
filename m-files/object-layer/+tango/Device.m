classdef (ConstructOnLoad) Device < tango.Base
    % Device Interface to Tango devices
    %
    %Constructor:
    %   >> dev=tango.Device(device_name);
    %
    %Attribute access:
    %   >> value=dev.attribute_name;
    %   >> dev.attribute_name=value;
    %
    %Command access:
    %   >> argout=dev.command_name(argin);
    %
    % Device Properties:
    %Name       Name of the device (RO)
    %Source     Source (tango.DevSource.DEV, tango.DevSource.CACHE, tango.DevSource.CACHE_DEV)
    %State      State of the device (tango.DevState enumeration, RO)
    %Status     Status string (RO)
    %Timeout    Client time-out [s] (RW)
    %......     Any Tango attribute is available as a property
    %
    % Device Methods:
    %cmd        Send a command
    %check      Send an optional command and wait for a resulting condition
    %attributes List of Tango attributes
    %commands   List of Tango commands
    %attrinfo   Get attribute information
    %cmdinfo    Get command information
    %isState    Checks the device state
    %set_property   Set a property value
    %get_property   Get a property value
    %del_property   Delete a property
    %......     Any Tango command is available as a method
    
    properties(Dependent=true,SetAccess=private)
        Name	% Device name (RO)
        State	% State of the device (tango.DevState enumeration) (RO)
        Status	% Status string (RO)
    end
    properties(Dependent=true)
        Source	% Device source (RW) (DEV, CACHE, CACHE_DEV)
    end
    
    methods(Access=protected,Hidden)
        function val=get1attr(dev,attrinfo)
            replies=dev.dvz(@tango_read_attribute,attrinfo.name);
            val=dev.attrprocess(attrinfo,replies);
        end
    end
    
    methods
        
        % Constructor
        
        function dev=Device(varargin)
            % build interface to a Tango device
            allnames=cs.allnames(varargin{:});
            n=length(allnames);
            if cs.dbgprintf(2,'tango.Device constructor(%d)',n), dbstack; end;
            if n>1, dev(n,1)=tango.Device(); end
            for i=1:n
                dev(i).devname=allnames{i}; %#ok<AGROW>
            end
        end
        
        % Property access
        
        function nam=get.Name(dev)
            nam=dev.devname;
        end
        function State=get.State(dev)
            replies=dev.dvz(@tango_read_attribute,'State');
            State=tango.DevState.Get(replies.value);
        end
        function Status=get.Status(dev)
            Status=dev.dvz(@tango_status);
        end
        function source=get.Source(dev)
            source=tango.DevSource.Get(dev.dvz(@tango_get_source).value);
        end
        function set.Source(dev,source)
            dev.dvz(@tango_set_source,double(tango.DevSource.Get(source)))
        end
        
        % Methods
        
        function varargout=cmd(dev,cmd_name,varargin)
            %Send a Tango command
            %argout=device.cmd(command,argin)
            %command:   command name
            %argin:     optional command input argument
            %argout:    optional command output argument
            varargout=cell(1,nargout);
            [varargout{:}]=dev.dvz(@tango_command_inout,cmd_name,varargin{:});
        end
        function val=get_attribute(dev,varargin)
            %Get Tango attribute values
            %value=device.get_attribute(attrname1[,attrname2,...])
            %attrname:	attribute name
            %value:     structure with fields "set","read","time","quality"
            replies=dev.dvz(@tango_read_attributes,varargin);
            %             failed=logical([replies.has_failed]);      % Anyway, an INVALID attribute
            %             if any(failed)                                  % generated an error with no stack
            %                 tango.Error(dev.Name,'tango_read_attributes','tango_read_attributes',...
            %                     'has_failed','read attribute failed').throw(); %cat(2,replies(failed).error))
            %             end
            val=arrayfun(@dev.attrprocess,dev.attrinfo(varargin{:}),replies);
        end
        function req=get_attribute_asynch(dev,varargin)
            %Asynchronously get Tango attribute values
            %request=device.get_attribute_asynch(attrname1[,attrname2,...])
            %attrname:	attribute name
            %request:   identifiers used to get the reply
            %
            %Usage:
            %>> request=device.get_attribute_asynch('attrname1','attrname2',...);
            %>> %do something
            %>> value=device.get_attribute_reply(request,0);
            req=dev.dvz(@tango_read_attributes_asynch,varargin);
        end
        function val=get_attribute_reply(dev,req,tmout)
            %Get the reply to "get_attribute_asynch"
            %value=device.get_attribute_reply(request,timeout)
            %request:   identifier returned by "get_attribute_asynch"
            %timeout:   time in ms to wait before generating an error (default 30000)
            %value:     structure with fields "set","read","time","quality"
            %
            %Usage:
            %>> request=device.get_attribute_asynch('attrname1','attrname2',...);
            %>> %do something
            %>> value=device.get_attribute_reply(request,0);
            replies=tango_read_attributes_reply(req,tmout);
            if tango_error < 0
                tango.Error(dev.Name,'tango_read_attributes_reply','tango_read_attributes_reply',...
                    tango_error_stack).throw();
            end
            %             failed=logical([replies.has_failed]);      % Anyway, an INVALID attribute
            %             if any(failed)                                  % generated an error with no stack
            %                 tango.Error(dev.Name,'tango_read_attributes_reply','tango_read_attributes_reply',...
            %                     'has_failed','read attribute failed').throw(); %cat(2,replies(failed).error))
            %             end
            val=arrayfun(@dev.attrprocess,dev.attrinfo(replies.name),replies);
        end
        function set_attribute(dev,varargin)
            %Set Tango attribute values
            %device.set_attribute(attrname1,value1,...)
            %attrname:   attribute name
            %value:
            value=struct('name',varargin(1:2:end),'value',varargin(2:2:end));
            dev.dvz(@tango_write_attributes,value);
        end
        function val=trycatch_attribute(dev,attrname,defval)
            %Get a Tango attribute
            %device.trycatch_attribute(attrname,defaultvalue)
            %attrname:   attribute name
            %value:
            attrinfo=dev.attrinfo(attrname);
            reply=tango_read_attribute(dev.handle,attrname);
            if tango_error >= 0
                val=dev.attrprocess(attrinfo,reply);
            else
                if nargin<3, defval=attrinfo.default; end
                err=tango.Error(dev.Name,'tango_read_attribute','tango_read_attribute',tango_error_stack);
                val=struct('set',defval,'read',defval,'error',err);
            end
        end
        function propval=get_property(dev,propname)
            %Get a device property
            %value=device.get_property(propname)
            %propname:	property name
            prop=dev.dvz(@tango_get_property,propname);
            propval=prop.value;
        end
        function set_property(dev,propname,value)
            %Set a device property
            %device.set_property(propname,value)
            %propname:	property name
            %value:     property value as a cell array of strings
            if ~iscellstr(value)
                dev.error('WrongDataType','Wrong data type',...
                    'Property value must be a cell array of strings.').throw();
            end
            dev.dvz(@tango_put_property,propname,value);
        end
        function del_property(dev,propname)
            %Delete a device property
            %device.del_property(propname)
            %propname:   property name
            dev.dvz(@tango_del_property,propname);
        end
        function attr=attribute(dev,attrname)
            attr=tango.Attribute(struct('devname',dev.Name,'attrname',...
                struct('name',attrname,'info',dev.attrinfo(attrname))));
        end
        
        % Compatibility function
        
        function varargout=dvcmd(dev,varargin)
            %Compatibility function: send a command to the device
            varargout=cell(1,nargout);
            if nargout < 1                  % tango bindings with no return value may
                tango_command_inout(dev.handle,varargin{:}); % return an error code
            elseif nargout < 2
                [varargout{:}]=tango_command_inout(dev.handle,varargin{:});
            else
                [varargout{1:end-1}]=tango_command_inout(dev.handle,varargin{:});
                if tango_error < 0
                    varargout{end}=tango.Error(dev.Name,varargin{1},tango_error_stack);
                else
                    varargout{end}=0;
                end
            end
        end
        function dvclose(dev)
            %Compatibility function: close the connection to the device
            dev.dvz(@tango_close_device);
        end
    end
end
