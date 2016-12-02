classdef (Abstract) DevError < MException
    properties
        devname
    end
    methods (Access=protected)
        function err=DevError(dname,errid,varargin)
            err@MException(['Cs:' errid],varargin{:});
            err.devname=dname;
        end
    end
    
    methods
        function errcode=double(err) %#ok<MANU>
            errcode=-1;
        end
        function iserr=logical(err) %#ok<MANU>
            iserr=true;
        end
        function res=lt(err,value)
            res=(double(err)<value);
        end
        function res=gt(err,value)
            res=(double(err)>value);
        end
        function res=le(err,value)
            res=(double(err)<=value);
        end
        function res=ge(err,value)
            res=(double(err)>=value);
        end
        function mess=char(err)
            mess=err.message;
        end
        function disp(err)
            disp([class(err) '(''' err.devname ''')']);
        end
        
        % Compatibility function
        
        function mess=dverrmess(err)
            mess=char(err);
        end
    end
end

