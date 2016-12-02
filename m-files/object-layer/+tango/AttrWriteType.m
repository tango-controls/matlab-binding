classdef AttrWriteType < uint8
%AttrWriteType Enumeration representing writable state
    
    methods
        function ok=logical(attr)
            persistent wrt
            if isempty(wrt)
                wrt=[false,false,true,true];
            end
            ok=wrt(attr+1);
        end
        function ok=isWritable(attr)
            ok=logical(attr);
        end
    end
    
    enumeration
        READ       (0)
        READ_WITH_WRITE     (1)
        WRITE       (2)
        READ_WRITE	(3)
    end
end

