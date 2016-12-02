classdef Init < tango.Singleton
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    methods(Static)
        function obj=instance()
            persistent singleobj
            if isempty(singleobj)
                singleobj=tango.Init();
            end
            obj=singleobj;
        end
    end
    methods(Access=private)
        function ini=Init()
            cs.dbgprintf(1,'Initializing tango access')
            tango_disable_v1_compatibility();
        end
        function delete(self) %#ok<INUSD>
            cs.dbgprintf(1,'Cleaning tango access')
        end
    end
    
end
