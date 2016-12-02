classdef AttrQuality < uint16
    %AttrQuality: Enumeration representing a Tango attribute quality
    %
    %Enumeration members:
    %     VALID
    %     INVALID
    %     ALARM
    %     CHANGING
    %     WARNING
    
    methods(Static)
        function q=Get(qname)
            %returns the AttrQuality value corresponding to a string or an
            %integer
            persistent qval qstr
            if ischar(qname)
                if isempty(qstr)
                    [qval,qstr]=enumeration('tango.AttrQuality');
                end
                q=qval(find(strcmpi(qname,qstr),1));
            else
                q=tango.AttrQuality(qname);
            end
        end
    end
    methods
        function ok=logical(self)
            %tango.AttrQality is true for VALID, false otherwise
            ok=~double(self);
        end
    end
    
    enumeration
        VALID       (0)
        INVALID     (1)
        ALARM       (2)
        CHANGING	(3)
        WARNING     (4)
    end
end

