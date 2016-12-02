classdef Error < cs.DevError
    %Error Exception thrown by a TANGO access error
    
    properties
        errorstack
    end
    
    methods(Static,Access=private)
        function message=disp_error_stack(err_stack)
            % get the error stack
            message=[];
            % display each DevError in the array
            stars(1:60) = char('*');
            message=[message sprintf('%s\n',stars)];
            message=[message sprintf('%s\n',...
                '*                    TANGO ERROR STACK                     *')];
            message=[message sprintf('%s\n',stars)];
            for i = 1:length(err_stack)
                message=[message sprintf('- ERROR %d\n', i)]; %#ok<*AGROW>
                message=[message msplit('reason..',err_stack(i).reason)];
                message=[message msplit('desc....',err_stack(i).desc)];
                message=[message sprintf('    |-origin.....%s\n',...
                    err_stack(i).origin)];
                message=[message sprintf('    |-severity...%s (%d)\n',...
                    err_stack(i).severity_str, err_stack(i).severity)];
            end
            message=[message sprintf('%s\n',stars)];
            function mess2=msplit(code,mess1)
                prefix={['    |-' code '...'],'    |            '};
                pos=strfind(mess1,char(10));
                cmess=cellfun(@(prefix,suffix) [prefix suffix],...
                    prefix([1 2*ones(1,length(pos))]),...
                    arrayfun(@(bg,nd) mess1(bg:nd),...
                    [0 pos]+1,[pos length(mess1)],'UniformOutput',false),...
                    'UniformOutput',false);
                mess2=[cmess{:} char(10)];
            end
        end
    end
    
    methods
        function err=Error(tangoname,varargin)
            %Error(devname,funcname,tangostack)
            %Error(devname,identifier,funcname,tangostack)
            %Error(devname,identifier,funcname,reason,desc)
            if length(varargin)==2
                [id,fname,errstack]=deal(varargin{[1 1:end]});
            elseif isstruct(varargin{3})
                [id,fname,errstack]=deal(varargin{1:3});
            else
                [id,fname,reason,desc]=deal(varargin{1:4});
                errstack=struct('reason',reason,'desc',desc,...
                    'origin',fname,'severity',1,'severity_str','Error');
            end
            err@cs.DevError(tangoname,['Tango:' id],...
                'Error evaluating %s(''%s''):\n%s',fname,tangoname,...
                tango.Error.disp_error_stack(errstack));
            err.errorstack=errstack;
        end
    end
end
