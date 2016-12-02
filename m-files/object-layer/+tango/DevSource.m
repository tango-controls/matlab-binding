classdef DevSource < uint16
    %AttrQuality: Enumeration representing a Tango attribute quality
    %
    %Enumeration members:
    %     DEV
    %     CACHE
    %     CACHE_DEV
    
    methods(Static)
        function q=Get(qname)
            %returns the AttrQuality value corresponding to a string or an
            %integer
            persistent qval qstr
            if ischar(qname)
                if isempty(qstr)
                    [qval,qstr]=enumeration('tango.DevSource');
                end
                q=qval(find(strcmpi(qname,qstr),1));
            else
                q=tango.DevSource(qname);
            end
        end
    end
    
    enumeration
        DEV         (0)
        CACHE       (1)
        CACHE_DEV	(2)
    end
end

