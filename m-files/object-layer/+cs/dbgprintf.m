function varargout=dbgprintf(level,format,varargin)
%DBGPRINTF Print debug information
%   DBGPRINTF(LEVEL,FORMAT,...)
%       Display the formatted message if the level is selected
%
%   DBGPRINTF(PRINTFUNC)
%       Use PRINTFUNC to display the messages. PRINTFUNC takes only one
%       argument: the message to be displayed
%
%   DBGPRINTF(MASK)
%       Set the mask selecting the displayed messages
%
%   DBGPRINTF()
%       Revert to the default options: MASK=0, display in the command
%       window
%

persistent MASK PRINTFUNC
if isempty(PRINTFUNC)
    PRINTFUNC=@(message) disp(message);
    dbglevel=getenv('DBGLEVEL');
    if isempty(dbglevel)
        setmask(0);
    else
        setmask(str2double(dbglevel));
    end
end

if nargin < 1
    setmask(0);
    setmask(@(message) disp(message));
elseif nargin < 2 && nargout == 0
    setmask(level);
else
    ok=bitand(level,MASK) || (level==0);
    if ok && nargin >= 2
        PRINTFUNC(sprintf(['-%d- ' format],level, varargin{:}));
    end
    if nargout > 0
        varargout={ok};
    end
end

    function setmask(msk)
        if isnumeric(msk)
            MASK=uint8(msk);
            cs.dbgprintf(1,'Set debug level to %d',msk);
        elseif isa(msk,'function_handle')
            PRINTFUNC=msk;
            cs.dbgprintf(1,'Set display function to %s',char(msk));
        end
    end
end
