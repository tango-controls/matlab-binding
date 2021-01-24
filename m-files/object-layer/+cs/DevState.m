classdef DevState < uint16
    %DevState Enumeration representing device states
    % (On,Off,Fault,...)
    %
    % Comparison operators are redefined to use a severity order,
    % where "On" is minimum and "Unknown" is maximum
    %
    % constant state values may be declared as:
    %       cs.DevState.On
    %
    
    methods(Static,Abstract)
        state=Get(statename)
        state=Undef(varargin)
    end
    
    methods
        function st=DevState(i)
            st@uint16(double(i));
        end
        function is=severity(st)
            %State severity: from On(1) to Unknown(9)
            persistent conv
            if isempty(conv)
                conv=[1,3,5,1,5,1,4,2,8,4,1,6,7,9,... % 0:13
                    6,6,2,7,7,1,1,4,6,1,1,8,4,4,... % 14:27
                    4,4,6,6,4,4,4,7,7,7,7,4,7,4,4,4,3,2,2]; % 28:46
            end
            is=conv(st+1);
        end
        function rgb=color(st)
            %RGB color representing the state
            persistent cmap
            if isempty(cmap)
                cmap=[...
                    0,0.5,0;...	% 1: On: green
                    1,1,0;...	% 2: Standby: yellow
                    1,1,1;...	% 3: Off: white
                    0,1,1;...	% 4: Moving: cyan
                    1,0.5,0;...	% 5: Inserted: orange
                    1,0.5,0;...	% 6: Alarm: orange
                    170/255,0,1;...	% 7: Disabled: magenta
                    1,0,0;...	% 8: Fault: red
                    0.5,0.5,0.5...	% 9: Unknown; grey
                    ];
            end
            rgb=cmap(severity(st),:);
        end
        function h=spot(st,position,sz)
            % Plots a square with the state color
            if nargin<3,sz=1;end
            if nargin<2,position=[0,0]; end
            h=patch(position(1)+sz*[-1 1 1 -1 -1],...
                position(2)+sz*[-1 -1 1 1 -1],...
                st.color());
        end
        function res=times(st1,st2)
            if ~(isscalar(st1) || isscalar(st2) || all(size(st1)==size(st2)))
                error('Cs:DevState:size','Matrix dimensions must agree');
            end
            if isa(st1,'cs.DevState')
                res=localtimes(st1,st2);
            else
                res=localtimes(st2,st1);
            end
            function res=localtimes(st1,coef)
                if isnumeric(coef)
                    if isscalar(st1)
                        res=repmat(st1,size(coef));
                    elseif iscalar(coef)
                        coef=repmat(coef,size(st1));
                        res=st1;
                    end
                    res(~(coef==1))=st1.Undef;
                else
                    error('Cs:DevState:arith','Impossible operation');
                end
            end
        end
        function res=mtimes(st1,st2)
            [m1,n1]=size(st1);
            [m2,n2]=size(st2);
            if ~(isscalar(st1) || isscalar(st2) || ((n1==1)&&(m2==1)))
                error('Cs:DevState:size','Matrix dimensions must agree');
            end
            if isa(st1,'cs.DevState')
                if isnumeric(st2)
                    ok=(st2==1);
                    res=st1.Undef(m1,n2);
                    res(:,ok)=repmat(st1,1,sum(ok));
                else
                    error('Cs:DevState:arith','Impossible operation');
                end
            else
                if isnumeric(st1)
                    ok=(st1==1);
                    res=st2.Undef(m1,n2);
                    res(ok,:)=repmat(st2,sum(ok),1);
                else
                    error('Cs:DevState:arith','Impossible operation');
                end
            end
        end
        function ok=lt(st1,st2)
            ok=st1.severity() < st2.severity();
        end
        function ok=le(st1,st2)
            ok=st1.severity() <= st2.severity();
        end
        function ok=gt(st1,st2)
            ok=st1.severity() > st2.severity();
        end
        function ok=ge(st1,st2)
            ok=st1.severity() >= st2.severity();
        end
        function [vmi,i]=min(x,varargin)
            [~,i]=min(severity(x),varargin{:});
            vmi=x(i);
        end
        function [vma,i]=max(x,varargin)
            [~,i]=max(severity(x),varargin{:});
            vma=x(i);
        end
        function [y,i]=sort(x,varargin)
            [~,i]=sort(severity(x),varargin{:});
            y=x(i);
        end
    end
end
