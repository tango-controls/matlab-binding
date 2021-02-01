function dev = cache(devname,classptr)

global MACHDEVCACHE

if nargin<2, classptr=@tango.Device; end
if isempty(MACHDEVCACHE)
    MACHDEVCACHE=containers.Map('KeyType','char','ValueType','any');
end
if nargin >= 1
    try
        dev=MACHDEVCACHE(devname);
    catch
        dev=classptr(devname);
        MACHDEVCACHE(devname)=dev;
    end
else
    MACHDEVCACHE.remove(MACHDEVCACHE.keys);
end
end
