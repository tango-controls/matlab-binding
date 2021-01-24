function varargout = split(fname)
%TANGO.SPLIT Split a device name into its components
%
%COMPONENTS=TANGO.SPLIT(DEVNAME)
%COMPONENTS:    Cell array of strings:
%               {protocol,host,domain,family,member,attribute}
%
%[PROTOCOL,HOST,DOMAIN,FAMILY,MEMBER.ATTRIBUTE]=TANGO.SPLIT(DEVNAME)
%   Return each component in a single string variable

toks=[strsplit(fname,'/','CollapseDelimiters',false),{'','','','','',''}];
hr=strfind(fname,'://');
if isempty(hr)
    ph=[{'tango'},strsplit(toks{1},':')];
    [protocol,domain]=deal(ph{end-1:end});
    [family,member,attribute]=deal(toks{2:4});
    host=getenv('TANGO_HOST');
else
    protocol=fname(1:hr(1)-1);
    [host,domain,family,member,attribute]=deal(toks{3:7});
end
if nargout==1
    varargout{1}={protocol,host,domain,family,member,attribute};
else
    varargout={protocol,host,domain,family,member,attribute};
end
