classdef (ConstructOnLoad) Group < tango.Base
    % Group Interface to Tango group of devices
    %
    % Group(dev1[,dev2,...])	builds an object from the specified devices
    %   dev1, dev2... may be:
    %       a string (device name)
    %       a cell array of strings
    %       an array of Device objects
    %       an array of Group objects
    %
    % Group Properties:
    %Name:      Name of the device group (RO)
    %State:     State of the devices (tango.DevState enumeration, RO)
    %Status:    Status strings (RO)
    %Size:      Number of devices in the group (RO)
    %......:    Any Tango attribute is available as a property
    %Timeout:   Client time-out [s] (RW)
    %
    % Group Methods:
    %add        Add a device to the group
    %remove     Remove a device from the group
    %list       Get the contents of the group
    %contains   Check for patterns
    %cmd        Send a command
    %check      Send an optional command and wait for a resulting condition
    %attributes	List of Tango attributes
    %commands	List of Tango commands
    %attrinfo   Get attribute information
    %cmdinfo    Get command information
    %isState    Checks the device state
    
    properties(Access=private,Hidden)
        groupname
        forward=1
    end
    %properties(Transient,Access=private,Hidden)
    properties(Transient)
        grphandle=[]
    end
    properties(Dependent=true,SetAccess=private)
        Name	% Device name (RO)
        State	% State of the device (tango.DevState enumeration) (RO)
        Status	% Status string (RO)
        Size    % Number of devices in the group (RO)
    end
    
    methods(Static,Access=protected,Hidden)
        function v=counter()
            persistent c
            if isempty(c), c=0; end
            c=c+1;
            v=c;
        end
        function val=attrprocess(attrinfo,replies)
            enabled=[replies.is_enabled];
            failed=[replies.has_failed];
            errs=cellfun(@(err) ~isempty(err),{replies.error});
            ok=enabled&~failed;
            % default values
            sz=size(ok);
            vset=cell(sz);
            vread=cell(sz);
            quality=tango.AttrQuality.INVALID(ones(sz));
            tim=NaN(sz);
            error=num2cell(zeros(sz));
            % successfully read attributes
            vset(ok)=cellfun(@(val) attrinfo.conversion(val),{replies(ok).setpoint},'UniformOutput',false);
            vread(ok)=cellfun(@(val) attrinfo.conversion(val),{replies(ok).value},'UniformOutput',false);
            %             quality(ok)=cat(2,replies(ok).quality);
            %             tim(ok)=cat(2,replies(ok).time);
            quality(ok)=[replies(ok).quality];
            tim(ok)=[replies(ok).time];
            error(errs)=arrayfun(@(rep) tango.Error([rep.dev_name '.' rep.name],...
                'tango_group_read_attributes','tango_group_read_attributes',rep.error),...
                replies(errs),'UniformOutput',false);
            % concatenate matrix-compatible values
            if attrinfo.data_format == tango.AttrDataFormat.SCALAR && attrinfo.data_type.isMatrixCompatible()
                vset(cellfun(@isempty,vset))={attrinfo.default};
                vset=[vset{:}];
                vread(cellfun(@isempty,vread))={attrinfo.default};
                vread=[vread{:}];
            end
            
            val=struct('set',{vset},'read',{vread},'time',{tim},'quality',{quality},'error',{error});
        end
    end
    
    methods(Access=protected,Hidden)
        function varargout=dvg(self,dvfunc,varargin)
            if nargout==0   % tango bindings with no return value may
                dvfunc(self.grphandle, varargin{:}); % return an error code
            else
                varargout=cell(1,nargout);
                [varargout{:}]=dvfunc(self.grphandle, varargin{:});
            end
            if tango_error < 0
                tango.Error(self.Name,func2str(dvfunc),tango_error_stack).throw();
            end
        end
        function val=get1attr(self,attrinfo)
%   tango_group_read_attribute fails if one device fails
%             replies=self.dvg(@tango_group_read_attribute,attrinfo.name,self.forward).replies;
%             val=self.attrprocess(attrinfo,replies);
%   tango_group_read_attributes gives corect results for non-failing devices
            attr=self.dvg(@tango_group_read_attributes,{attrinfo.name},self.forward);
            vv=cat(2,attr.dev_replies.attr_values);
            val=self.attrprocess(attrinfo,vv);
        end
        function scan(self,action,varargin)
            grps=cellfun(@(arg) isa(arg,'tango.Group'), varargin);
            allnames=cs.allnames(varargin{~grps});
            cellfun(@(name) action(self,name),allnames);
            if any(grps)
                allgroups=cat(1,varargin{grps});
                cellfun(@(id,name) action(self,id),{allgroups.grphandle});
            end
        end
    end
    
    methods
        function dev=Group(varargin)
            dev.groupname=sprintf('group%i',tango.Group.counter());
            dev.grphandle=tango_group_create(dev.groupname);
            if tango_error < 0
                err=tango.Error(dev.groupname,'tango_group_create',...
                    tango_error_stack);
                err.throw();
            end
            dev.add(varargin{:});
        end
        function add(self,varargin)
            %Add a device to the group
            %group.add(name1[,name2...])
            self.scan(@adddev,varargin{:});
            if isempty(self.devname)
                allnames=self.list();
                if ~isempty(allnames)
                    self.devname=allnames{1};
                end
            end
            function adddev(self,elt)
                self.dvg(@tango_group_add,elt);
            end
        end
        function remove(self,varargin)
            %Remove a device from the group
            %group.remove(name1[,name2...])
            self.scan(@removedev,varargin{:});
            function removedev(self,elt)
                self.dvg(@tango_group_remove,elt,self.forward);
            end
        end
        function lst=list(self)
            %Get a list of names of the devices belonging to the group
            %devlist=group.list()
            replies=self.dvg(@tango_group_read_attribute,'State',self.forward).replies;
            lst={replies.dev_name};
        end
        function ok=contains(self,varargin)
            %Check if group contains given patterns
            %ok=group.contains(pattern1,pattern2,...)
            ok=logical(cellfun(@(pattern) self.dvg(@tango_group_contains,pattern,self.forward),varargin));
        end
        
        % Property access
        
        function nam=get.Name(self)
            nam=self.groupname;
        end
        function state=get.State(self)
            replies=self.dvg(@tango_group_read_attribute,'State',self.forward).replies;
            enabled=logical([replies.is_enabled]);
            failed=logical([replies.has_failed]);
            state=tango.DevState.Unknown(ones(size(replies)));
            state(enabled&~failed)=tango.DevState.Get([replies(enabled&~failed).value]);
        end
        function status=get.Status(self)
            replies=self.dvg(@tango_group_read_attribute,'Status',self.forward).replies;
            enabled=logical([replies.is_enabled]);
            failed=logical([replies.has_failed]);
            status=cell(size(replies));
            status(enabled&~failed)={replies(enabled&~failed).value};
        end
        function sz=get.Size(self)
            sz=self.dvg(@tango_group_size,self.forward);
        end
        
        % Methods
        
        function result=cmd(self,cmd_name,varargin)
            %Send a Tango command
            %argout=group.cmd(command,argin)
            %command:   command name
            %argin:     optional command input argument
            %argout:    optional command output argument
            res=self.dvg(@tango_group_command_inout,cmd_name,self.forward,varargin{:});
            result={res.replies.data}';
        end
        function val=get_attribute(self,varargin)
            %Get Tango attribute values
            %value=group.get_attribute(attrname1[,attrname2,...])
            %attrname:	attribute name
            %value:     structure with fields "set","read","time","quality"
            attr=self.dvg(@tango_group_read_attributes,varargin,self.forward);
            vv=cat(1,attr.dev_replies.attr_values);    % Ndev x Nattr structure
            val=cellfun(@self.attrprocess,num2cell(self.attrinfo(varargin{:})),num2cell(vv,1));
        end
        function req=get_attribute_asynch(self,varargin)
            %Asynchronously get Tango attribute values
            %value=group.get_attribute_reply(request,timeout)
            %request:   identifiers returned by "get_attribute_asynch"
            %timeout:   time in ms to wait before generating an error (default 30000)
            %value:     structure with fields "set","read","time","quality"
            %
            %Usage:
            %>> request=group.get_attribute_asynch('attrname1','attrname2',...);
            %>> %do something
            %>> value=group.get_attribute_reply(request,0);
            req=self.dvg(@tango_group_read_attributes_asynch,varargin,self.forward);
        end
        function val=get_attribute_reply(self,req,tmout)
            %Get the reply to "get_attribute_asynch"
            %value=group.get_attribute_reply(request,timeout)
            %request:   identifiers returned by "get_attribute_asynch"
            %timeout:   time in ms to wait before generating an error (default 30000)
            %value:     structure with fields "set","read","time","quality"
            %
            %Usage:
            %>> request=group.get_attribute_asynch('attrname1','attrname2',...);
            %>> %do something
            %>> value=group.get_attribute_reply(request,0);
            attr=tango_group_read_attributes_reply(req,tmout);
            if tango_error < 0
                tango.Error(self.Name,'tango_group_read_attributes_reply',...
                    tango_error_stack).throw();
            end
            vv=cat(1,attr.dev_replies.attr_values);    % Ndev x Nattr structure
            val=cellfun(@self.attrprocess,num2cell(self.attrinfo(vv(1,:).attr_name)),num2cell(vv,1));
        end
        function set_attribute(self,attrname,attrvalue)
            %Set a Tango attribute value
            %group.set_attribute(attrname1,value1,...)
            %attrname:   attribute name
            %value:
            dev.dvg(@tango_group_write_attribute,attrname,self.forward,attrvalue);
        end
        function disp(self)
            %Display the device definition
            self.dvg(@tango_group_dump);
        end
        function delete(self)
            if ~isempty(self.grphandle)
                cs.dbgprintf(1,'Deleting group <%s>',self.Name);
                self.dvg(@tango_group_kill);
            else
                cs.dbgprintf(1,'Deleting empty group\n');
            end
        end
    end
    
end

