classdef Device < dynamicprops
    %Base class for Tango control system interface
    
    properties(Abstract,SetAccess=private)
        Name	% Device name (RO)
        State	% State of the device (cs.DevState enumeration) (RO)
        Status	% Status string (RO)
    end
    properties(Abstract)
        Timeout % Client time-out (WO)
    end
    
    methods(Abstract)
        output=cmd(dev,command,input)
        list=attributes(dev)
        list=commands(dev)
        err=error(id,reason,desc)
    end
    
    methods(Abstract,Static,Access=protected,Hidden)
        st=any2state(code)
    end
    methods(Static,Access=protected)
	function varargout = decodeargs(default_values,args)
	%[arg1,arg2,...]=decodeargs(default_values,argin)
	%   Check and expands optional argument lists
	na=min(length(default_values),length(args));
	ok=~cellfun(@isempty,args(1:na));
	default_values(ok)=args(ok);
	if nargout==length(default_values)
	    varargout=default_values;
	else
	    varargout{1}=default_values;
	end
	end
    end
    methods(Access=protected,Hidden)
        function prop=addcommand(self,cmdname)
            try
                prop=self.addprop(cmdname);
                prop.Transient=true;
                self.(cmdname)=@(varargin) self.cmd(cmdname,varargin{:});
                prop.SetAccess='private';
            catch err
                warning(err.identifier,err.message);
            end
        end
        function prop=addattribute(self,attrname,getfunc,setfunc)
            try
                prop=addprop(self,attrname);
                prop.Transient=true;
                prop.Dependent=true;
                if nargin <4
                    prop.SetAccess='private';
                else
                    prop.SetMethod=setfunc;
                end
                prop.GetMethod=getfunc;
            catch err
                warning(err.identifier,err.message);
            end
        end
    end
    
    methods
        function strname=char(self)
            %Convert to character array (string)
            strname=self.Name;
        end
        function disp(self)
            %Display the device definition
            disp([class(self) '(''' self.Name ''')']);
        end
        function ok=isState(self,varargin)
            %Test the device state with one or several proposals
            %ok=device.IsState(state1[,state2,...])
            %state1:    state to be checked (string, integer or
            %           cs.DevState
            dstate=self.State;
            ok=cellfun(@checkargs,varargin);
            function ok=checkargs(arg)
                if ~isa(arg,'cs.DevState')
                    arg=self.any2state(arg);
                end
                ok=(dstate==arg);
            end
        end
        function vok=check(self,varargin)
            %Send a Tango command and checks a condition
            %ok=device.check(testfun,timeout)
            %testfun:   test function, called as testfun(dev)
            %timeout:   maximum time to wait for condition, may be
            %           {t1,t2,t3}: wait t1, start looping on testfun at
            %           interval t3, wait for t2 maximum
            %ok:        true if condition satisfied, false otherwise
            %
            %ok=device.check(command,testfun,timeout)
            %command:   command name, may be {command,argin}
            if ~isempty(varargin) && isa(varargin{1},'function_handle')
                [cmdargs,testfunc,tmout]=self.decodeargs({{},@(dev) true,{}},[{{}},varargin]);
            else
                [cmdargs,testfunc,tmout]=self.decodeargs({{},@(dev) true,{}},varargin);
            end
            if ~iscell(tmout)
                tmout={[],tmout};
            end
            [initim,totaltim,polltim]=self.decodeargs({0,0,0.5},tmout);
            if ~isempty(cmdargs)
                if ~iscell(cmdargs), cmdargs={cmdargs}; end
                self.cmd(cmdargs{:});
            end
            pause(initim);
            ok=testfunc(self);
            if ~ok && (totaltim>0)
                for t=1:totaltim/polltim
                    pause(polltim);
                    ok=testfunc(self);
                    if ok, break; end
                end
            end
            if ~ok && nargout == 0
                self.error('CheckTimeout','Check Timeout','Timeout evaluating the condition').throw();
            elseif nargout > 0
                vok=ok;
            end
        end
        
        % Compatibility function
        
        function nm=dvname(self)
            %Compatibility function: get device name
            nm=self.Name;
        end
    end
end

