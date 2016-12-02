function nms=allnames(varargin)
nms=decode({},varargin{:});
    function res=decode(res,varargin)
        for i=1:numel(varargin)
            arg=varargin{i};
            if ischar(arg)  % char array
                res=[res;cellstr(arg)]; %#ok<AGROW>
            elseif iscell(arg)      % cell array
                res=decode(res,arg{:});
            elseif isa(arg,'tango.Group')
                res=[res;{arg.Name}']; %#ok<AGROW>
            elseif isa(arg,'cs.Device')
                res=[res;{arg.Name}']; %#ok<AGROW>
            else
                error('Cs:WrongDeviceName',...
                    ['Argument must be string,cell array of strings, ',...
                    'cs.Device']);
            end
        end
    end
end
