
classdef Base < cs.Device&tango.Access
    %tango.Base: Base class for tango devices and groups
    
    properties(Access=protected,Hidden)
        devname=''
    end
    properties(Transient,Access=protected,Hidden)
        tangoattributes
        %allattrs
    end
    
    methods(Abstract,Access=protected,Hidden)
        val=get1attr(self,attrinfo)
    end
    methods(Static,Access=protected,Hidden)
        function st=any2state(code)     % Required by cs.Device
            st=tango.DevState.Get(code);
        end
    end
    
    methods
        function set.devname(self,name)
            %fprintf('Set devname to %s\n',name);
            self.devname=name;
            self.handle=name;
            attrlist=self.dvz(@tango_attribute_list_query);
            %v=[{attrlist.name};arrayfun(@scanattributes,attrlist,'UniformOutput',false)];
            %self.allattrs=struct(v{:}); %#ok<MCSUP>
            self.tangoattributes=arrayfun(@scanattributes,attrlist); %#ok<MCSUP>
            cmdlist=self.dvz(@tango_command_list_query);
            arrayfun(@scancommands,cmdlist);
            function info=scanattributes(attrinfo)
                pollperiod=self.dvz(@tango_get_attribute_poll_period,attrinfo.name);
                info=self.attrdefine(name,attrinfo,pollperiod);
                if ~any(strcmp(info.name,{'Status','State'}))
                    if info.writable
                        self.addattribute(info.name,@(dev) dev.get1attr(info),...
                            @(dev,value) dev.set_attribute(info.name,value));
                    else
                        self.addattribute(info.name,@(dev) dev.get1attr(info));
                    end
                end
            end
            function scancommands(cmdinfo)
                cmdname=cmdinfo.cmd_name;
                if ~any(strcmp(cmdname,{'State','Status'}))
                    self.addcommand(cmdname);
                end
            end
        end
        function cmdlist=commands(self)
            % Lists the available Tango commands
            clist=self.dvz(@tango_command_list_query);
            if nargout==0
                fprintf('Commands for device %s\n',char(self));
                arrayfun(@decode,clist,'UniformOutput',false);
            else
                list={clist.cmd_name};
                cmdlist=list(:);
            end
            function decode(cmdinfo)
                cinfo=self.cmddefine(cmdinfo,0);    % Polling period undefined
                fprintf('    %20s  (%s,%s)\n',cinfo.cmd_name,char(cinfo.in_type),char(cinfo.out_type));
            end
        end
        function attrlist=attributes(self)
            % Lists the available Tango attributes
            if nargout==0
                fprintf('Attributes for device %s\n',char(self));
                arrayfun(@decode,self.tangoattributes,'UniformOutput',false);
                %structfun(@decode,self.allattrs,'UniformOutput',false);
            else
                attrlist={self.tangoattributes.name}';
                %attrlist=fieldnames(self.allattrs);
            end
            function decode(attrinfo)
                fprintf('    %20s  (%9s,%-12s)\n',attrinfo.name,char(attrinfo.data_format),char(attrinfo.data_type));
            end
        end
        function info=cmdinfo(self,varargin)
            %Get command information
            %cmdinfo=device.cmdinfo(cmdname)
            info=cellfun(@(cn) self.cmddefine(...
                self.dvz(@tango_command_query,cn),...
                self.dvz(@tango_get_command_poll_period,cn)),...
                varargin);
        end
        function info=attrinfo(self,varargin)
            %Get attribute information
            %attrinfo=device.attrinfo(attrname1[,attrname2,...])
            
            %info=cellfun(@(an) self.allattrs.(an), varargin);
            attrs={self.tangoattributes.name};
            info=cellfun(@sel,varargin);
            function attrinfo=sel(attrname)
                id=find(strcmpi(attrname,attrs),1);
                attrinfo=self.tangoattributes(id);
            end
        end
    end
end
