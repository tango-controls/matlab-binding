function idx=planeid(plane,choices)
%idx=planeid(plane)
%   plane: h|H|x|X|1, v|V|z|Z|2
%idx: 1 for horizontal
%     2 for vertical
%     error for other
%
%idx=planeid(plane,choices)
%   plane: h|H|x|X|1, v|V|z|Z|2
%idx: choices(1) for horizontal
%     choices(2) for vertical
%     choices(3) for other, or error

idx=3;
if ischar(plane)
    switch upper(plane)
        case {'H','X'}
            idx=1;
        case {'V','Z'}
            idx=2;
    end
elseif isscalar(plane) && isnumeric(plane) && (plane==1 || plane==2)
    idx=plane;
end
if nargin >= 2 && (idx <= length(choices))
    if iscell(choices)
        idx=choices{idx};
    else
        idx=choices(idx);
    end
elseif idx > 2
    error('cs:plane','plane should be h|H|x|X|1 or v|V|z|Z|2');
end
end
