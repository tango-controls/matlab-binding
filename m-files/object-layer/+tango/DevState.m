classdef DevState < cs.DevState
    %DevState Enumeration representing device states
    % (On,Off,Fault,...)
    %
    % Comparison operators are redefined to use a severity order,
    % where "On" is minimum and "Unknown" is maximum
    %
    % constant state values may be declared as:
    %       tango.DevState.On
    %
    
    methods(Static)
        function state=Get(statename)
            %returns the DevState value corresponding to a string or an
            %integer
            persistent sval sstr
            if ischar(statename)
                if isempty(sstr)
                    [sval,sstr]=enumeration('tango.DevState');
                end
                state=sval(find(strcmpi(statename,sstr),1));
            else
                state=tango.DevState(statename);
            end
        end
    end
    
    enumeration
        On          (0)
        Off         (1)
        Closed      (2)
        Open		(3)
        Inserted	(4)
        Extracted   (5)
        Moving      (6)
        Standby     (7)
        Fault       (8)
        Init		(9)
        Running     (10)
        Run         (10)
        Alarm       (11)
        Disabled	(12)
        Unknown     (13)
    end
end
