function prop2 = property_string(prop1)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

if iscellstr(prop1)
    prop2=prop1;
elseif iscell(prop1)
    pr=cellfun(@tango.property,prop1,'UniformOutput',false);
    prop2=cat(2,pr{:});
else
    if ischar(prop1)
        prop2={prop1};
    elseif isscalar(prop1)
        prop2={num2str(prop1,15)};
    else
        pr=mat2str(prop1);
        prop2={strrep(pr(2:end-1),' ',',');};
    end
end
end

