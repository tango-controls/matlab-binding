classdef (ConstructOnLoad) TestDevice < tango.Device
    
    properties(Dependent=true)   % New property <=> pseudo-attribute
        twice_double_scalar
    end
    
    methods
        % Constructor
        function dev=TestDevice(varargin)
            dev=dev@tango.Device(varargin{:});
            disp(mfilename('class'));
        end
        
        % Access to the new property
        function v=get.twice_double_scalar(dev)
            v=2*dev.double_scalar.set;
        end
        function set.twice_double_scalar(dev,value)
            dev.double_scalar=2*value;
        end
        
        % New method
        function [v1,v2]=doublecmd(dev,cmd1,cmd2,arg1,arg2)
            v1=dev.cmd(cmd1,arg1); % Syntax 1
            v2=dev.(cmd2)(arg2);   % Syntax 2
        end
        
        function pl=testplane(dev,varargin)
            dev.error('CheckTimeout','test','this is a test of error method').throw();
            pl=cs.planeid(varargin{:});
        end
    end
    
end
