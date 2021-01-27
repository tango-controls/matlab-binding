function varargout=planeid(plane,varargin)
%idx=selectplane(plane)
%   plane selector: h|H|x|X|1, v|V|z|Z|2
%idx: 1 for horizontal
%     2 for vertical
%     error for other
%
%[result1,result2,...]=selectplane(plane,selection1,selection2,...)
%   plane selector: h|H|x|X|1, v|V|z|Z|2, v2h|V2H|3, h2v|H2V|4
%idx: selection(1) for horizontal
%     selection(2) for vertical
%     selection(3) for v2h
%     selection(4) for h2v
%
%[...]=selectplane(...,'Choices',{choice1,choice2},...)
%   Gives the possible choices for the plane selector. Each choice is a
%   string or a cell array of strings
%   default: {{'X','H'},{'Z','V'},'V2H','H2V'}
%

selection={{1,2}};
[choices,arguments]=getoption(varargin,'Choices',{{'x','h'},{'z','v'},'v2h','h2v'});
selection(1:length(arguments))=arguments;
nmax=min([length(choices) cellfun(@length,selection)]);
idx=0;
if ischar(plane)
    for i=1:nmax
        if any(strcmpi(plane,choices{i}))
            idx=i;
            break;
        end
    end
elseif isscalar(plane) && isnumeric(plane)
    idx=plane;
end
if idx <= 0 || idx > nmax
    error('cs:plane','plane should be %s',allchoices(choices(1:nmax)));
end
varargout=cellfun(@(arg) arg{idx},selection,'UniformOutput',false);

    function res=allchoices(choices)
        k=cellfun(@expand,num2cell(1:length(choices)),choices,'UniformOutput',false);
        res=cat(2,k{:});
        res=res(3:end);
        function res=expand(i,choice)
            if ischar(choice)
                l={cat(2,'|',lower(choice),'|',upper(choice))};
            else
                l=cellfun(@(o) cat(2,'|',lower(o),'|',upper(o)),choice,'UniformOutput',false);
            end
            res=cat(2,', ',int2str(i),l{:});
        end
    end

end
